#!/usr/bin/env python3
"""tools/sync_symbols.py must be deterministic: given the same funcs.toml,
ChronoRET checkout, and dscotton checkout, syncing a bank twice (into two
independent copies of funcs.toml) must produce byte-identical output, and
syncing an already-synced copy a second time (idempotency) must leave it
unchanged. Operates on scratch copies; never touches the real funcs.toml
or unresolved.toml.

Skips (exit 77) if DSCOTTON_DIR isn't set, since that's an external
checkout not every environment has.
"""
import os
import shutil
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SYNC = os.path.join(ROOT, 'tools', 'sync_symbols.py')
BANK = 'C0'


def run_sync(funcs_toml: str, unresolved_toml: str) -> None:
    subprocess.run(
        [sys.executable, SYNC, BANK, '--funcs-toml', funcs_toml, '--unresolved-toml', unresolved_toml],
        check=True, cwd=ROOT, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)


def read(path: str) -> str:
    if not os.path.isfile(path):
        return ''
    with open(path, encoding='utf-8') as f:
        return f.read()


def main() -> int:
    if not os.environ.get('DSCOTTON_DIR'):
        print('skip: DSCOTTON_DIR not set')
        return 77

    with tempfile.TemporaryDirectory() as d:
        funcs_src = os.path.join(ROOT, 'funcs.toml')
        a_funcs, a_unres = os.path.join(d, 'a_funcs.toml'), os.path.join(d, 'a_unresolved.toml')
        b_funcs, b_unres = os.path.join(d, 'b_funcs.toml'), os.path.join(d, 'b_unresolved.toml')
        shutil.copy(funcs_src, a_funcs)
        shutil.copy(funcs_src, b_funcs)

        run_sync(a_funcs, a_unres)
        run_sync(b_funcs, b_unres)
        a_funcs_text, b_funcs_text = read(a_funcs), read(b_funcs)
        a_unres_text, b_unres_text = read(a_unres), read(b_unres)
        if a_funcs_text != b_funcs_text:
            print('FAIL: two independent syncs produced different funcs.toml output')
            return 1
        if a_unres_text != b_unres_text:
            print('FAIL: two independent syncs produced different unresolved.toml output')
            return 1

        before_funcs, before_unres = a_funcs_text, a_unres_text
        run_sync(a_funcs, a_unres)   # re-sync an already-synced copy
        if read(a_funcs) != before_funcs:
            print('FAIL: re-syncing an already-synced funcs.toml changed it (not idempotent)')
            return 1
        if read(a_unres) != before_unres:
            print('FAIL: re-syncing an already-synced unresolved.toml changed it (not idempotent)')
            return 1

    print('ok: sync_symbols.py is deterministic and idempotent for bank', BANK)
    return 0


if __name__ == '__main__':
    sys.exit(main())
