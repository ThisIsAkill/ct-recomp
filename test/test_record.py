#!/usr/bin/env python3
"""ct_sdl --record round trip.

usage: test_record.py FRONTEND PROBE SCRIPT FRAMES

Plays SCRIPT in the frontend (headless, unpaced) while recording pad 1,
then runs the probe on the recording and on SCRIPT and requires the
per-frame state hashes (--hash-log) to match on every frame: the recording
reproduces the run exactly. Exit 0 on a match, 1 otherwise.
"""
import os
import subprocess
import sys
import tempfile


def main() -> int:
    if len(sys.argv) != 5:
        print("usage: test_record.py FRONTEND PROBE SCRIPT FRAMES", file=sys.stderr)
        return 2
    frontend, probe, script, frames = sys.argv[1:]
    env = dict(os.environ, SDL_VIDEODRIVER="dummy", SDL_AUDIODRIVER="dummy")
    with tempfile.TemporaryDirectory() as tmp:
        rec = os.path.join(tmp, "rec.txt")
        run = subprocess.run([frontend, "--fast", "--frames", frames, "--script", script,
                              "--record", rec], env=env, capture_output=True, text=True)
        if run.returncode:
            print(f"test_record: frontend exit {run.returncode}: {run.stderr.strip()[-300:]}")
            return 1
        logs = {}
        procs = {}
        for name, src in (("recorded", rec), ("original", script)):
            logs[name] = os.path.join(tmp, name + ".log")
            procs[name] = subprocess.Popen([probe, "--frames", frames, "--script", src,
                                            "--hash-log", logs[name]],
                                           stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        for name, p in procs.items():
            if p.wait():
                print(f"test_record: probe on the {name} script exit {p.returncode}")
                return 1
        a = open(logs["recorded"]).read().splitlines()
        b = open(logs["original"]).read().splitlines()
    for x, y in zip(a, b):
        if x != y:
            print(f"test_record: first divergent frame {y.split()[0]}: recorded {x.split()[1]}, "
                  f"original {y.split()[1]}")
            return 1
    if len(a) != len(b) or not a:
        print(f"test_record: {len(a)} recorded frames vs {len(b)} original")
        return 1
    print(f"test_record: {len(a)} frames identical")
    return 0


if __name__ == "__main__":
    sys.exit(main())
