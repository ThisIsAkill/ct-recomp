#!/usr/bin/env python3
"""ctest results recorded per source tree, so the same tests are not rerun
on the same code.

usage: ctest_cache.py BUILD_DIR [ctest args...]

Builds BUILD_DIR, runs ctest there with the given arguments (e.g. -E
diff_all, -R lockstep) and records each test's result under the key of
the working tree; tools/progress.py reuses the records and runs only the
tests that have no passing record for the current key.

The key is the git tree id of the working tree as it is (tracked and
untracked files, .gitignore respected; PROGRESS.md files and README.md
left out, since tools/progress.py writes them from the results) plus
$CT_ROM. Committing the tree does not change it; any edit to a source,
test or tool does. Records live in BUILD_DIR/ctest_cache.json, the last
KEEP keys. Failures are recorded but never reused.

Exit status: ctest's.
"""
from __future__ import annotations

import json
import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
KEEP = 8
GENERATED = ('README.md', ':(glob)**/PROGRESS.md')


def tree_key(root: str | None = None) -> str | None:
    """Git tree id of the working tree (generated files left out) and
    $CT_ROM, or None outside a git checkout."""
    root = root or ROOT
    with tempfile.TemporaryDirectory() as tmp:
        env = dict(os.environ, GIT_INDEX_FILE=os.path.join(tmp, 'index'))
        git = lambda *a: subprocess.run(['git', *a], cwd=root, env=env, capture_output=True,
                                        text=True)
        if git('add', '-A', '.').returncode:
            return None
        git('rm', '-q', '--cached', '--ignore-unmatch', '--', *GENERATED)
        r = git('write-tree')
        if r.returncode:
            return None
    return f'{r.stdout.strip()} {os.environ.get("CT_ROM", "")}'


def load(build: str) -> dict:
    try:
        with open(os.path.join(build, 'ctest_cache.json')) as f:
            return json.load(f)
    except (OSError, ValueError):
        return {}


def save(build: str, cache: dict) -> None:
    keys = list(cache)[-KEEP:]
    with open(os.path.join(build, 'ctest_cache.json'), 'w') as f:
        json.dump({k: cache[k] for k in keys}, f, indent=1)


def parse(out: str) -> dict[str, dict]:
    """Per-test results of `ctest -V` output: {name: {status, checks}};
    checks counts the "N checks, 0 failed" lines the test printed."""
    names, res = {}, {}
    for num, name, status in re.findall(r'^\s*\d+/\d+ Test\s+#(\d+): (\S+) \.+\s*(\*{0,3}\w+)',
                                        out, re.M):
        names[num] = name
        res[name] = {'status': status.strip('*'), 'checks': 0}
    for num, n in re.findall(r'^(\d+): \S+: (\d+) checks, 0 failed$', out, re.M):
        if num in names:
            res[names[num]]['checks'] += int(n)
    return res


def build_tree(build: str) -> None:
    jobs = str(os.cpu_count() or 1)
    p = subprocess.run(['cmake', '--build', build, '-j', jobs], capture_output=True, text=True)
    if p.returncode:
        raise SystemExit(f'ctest_cache: build failed\n{(p.stdout + p.stderr)[-2000:]}')


def run(build: str, args: list[str], log: str | None = None) -> tuple[int, dict[str, dict]]:
    """Run `ctest -V ARGS` in BUILD and record the results; (exit, results)."""
    jobs = str(os.cpu_count() or 1)
    key = tree_key()
    p = subprocess.run(['ctest', '--test-dir', build, '-V', '-j', jobs, *args],
                       capture_output=True, text=True)
    if log:
        with open(log, 'w') as f:
            f.write(p.stdout + p.stderr)
    res = parse(p.stdout)
    recorded = key is not None and key == tree_key()   # nothing changed while it ran
    if recorded:
        cache = load(build)
        entry = cache.pop(key, {})
        entry.update(res)
        cache[key] = entry
        save(build, cache)
    run.recorded = recorded
    return p.returncode, res


def all_tests(build: str) -> list[str]:
    p = subprocess.run(['ctest', '--test-dir', build, '-N'], capture_output=True, text=True)
    return re.findall(r'^\s*Test\s+#\d+: (\S+)$', p.stdout, re.M)


def cached_run(build: str, log: str | None = None) -> tuple[dict[str, dict], list[str]]:
    """Every test's result for the current tree: reused where a passing
    record exists, run otherwise. Returns ({name: result}, names run)."""
    build_tree(build)
    names = all_tests(build)
    key = tree_key()
    have = load(build).get(key, {}) if key else {}
    todo = [n for n in names if have.get(n, {}).get('status') != 'Passed']
    res = {n: have[n] for n in names if n not in todo}
    if todo:
        regex = '^(' + '|'.join(re.escape(n) for n in todo) + ')$'
        _, ran = run(build, ['-R', regex], log)
        res.update(ran)
    return {n: res.get(n, {'status': 'NotRun', 'checks': 0}) for n in names}, todo


def main() -> int:
    if len(sys.argv) < 2:
        print(__doc__.strip().splitlines()[3], file=sys.stderr)
        return 2
    build = sys.argv[1]
    build_tree(build)
    code, res = run(build, sys.argv[2:])
    failed = [n for n, r in res.items() if r['status'] != 'Passed']
    print(f'ctest_cache: {len(res) - len(failed)}/{len(res)} passed, '
          + ('recorded' if run.recorded else 'NOT recorded (the tree changed while it ran)')
          + (f'; failed: {", ".join(failed)}' if failed else ''))
    return code


if __name__ == '__main__':
    sys.exit(main())
