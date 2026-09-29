#!/usr/bin/env python3
"""Compare two probe builds' speed on the same run.

usage: bench.py --base PROBE --new PROBE --frames N [--script FILE]
                [--runs K] [probe args...]

Runs `PROBE --frames N [--script FILE] --ref-log LOG` for both probes,
alternating, K times each (default 5), and reports each one's median CPU
time (user + system, getrusage of the child) and the change from base to
new. Both runs must log the same per-frame hashes (WRAM and picture): a
speed comparison between runs that emulate differently means nothing, so
that is an error. Extra arguments go to both probes.

Exit 0 if the runs match, 1 if they don't, 2 on bad usage or a run that
fails.
"""
from __future__ import annotations

import argparse
import os
import resource
import statistics
import subprocess
import sys
import tempfile


def run(probe: str, args: list[str], log: str) -> float:
    before = resource.getrusage(resource.RUSAGE_CHILDREN)
    p = subprocess.run([probe, *args, '--ref-log', log], capture_output=True, text=True)
    after = resource.getrusage(resource.RUSAGE_CHILDREN)
    if p.returncode:
        raise SystemExit(f'bench: {probe} exit {p.returncode}: {(p.stdout + p.stderr)[-300:]}')
    return (after.ru_utime - before.ru_utime) + (after.ru_stime - before.ru_stime)


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('--base', required=True)
    ap.add_argument('--new', required=True)
    ap.add_argument('--frames', type=int, required=True)
    ap.add_argument('--script')
    ap.add_argument('--runs', type=int, default=5)
    a, extra = ap.parse_known_args()
    if a.runs < 1:
        print('bench: --runs must be at least 1')
        return 2
    args = ['--frames', str(a.frames), *(['--script', a.script] if a.script else []), *extra]
    times: dict[str, list[float]] = {'base': [], 'new': []}
    with tempfile.TemporaryDirectory() as tmp:
        logs = {k: os.path.join(tmp, k + '.log') for k in times}
        for _ in range(a.runs):
            for k, probe in (('base', a.base), ('new', a.new)):
                times[k].append(run(probe, args, logs[k]))
        same = open(logs['base']).read() == open(logs['new']).read()
    med = {k: statistics.median(v) for k, v in times.items()}
    for k in ('base', 'new'):
        print(f'bench: {k:4} median {med[k]:.3f} s CPU over {a.runs} runs '
              f'(min {min(times[k]):.3f}, max {max(times[k]):.3f})')
    print(f'bench: new vs base {100 * (med["new"] - med["base"]) / med["base"]:+.2f}%')
    if not same:
        print('bench: the two runs logged different frames (WRAM or picture): not comparable')
        return 1
    print(f'bench: both runs logged the same {a.frames} frames')
    return 0


if __name__ == '__main__':
    sys.exit(main())
