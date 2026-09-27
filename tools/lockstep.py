#!/usr/bin/env python3
"""Native vs interpreter lockstep check.

Runs the headless probe twice on the same arguments, once with native
dispatch and once with --interp-only, each writing a per-frame state hash
(--hash-log: CPU, WRAM, SRAM, VRAM, CGRAM, OAM, frame, APU RAM and DSP
registers at each frame edge), and compares them frame by frame.

usage: lockstep.py PROBE [probe args...]

Exit 0 when every frame matches and both runs exit 0; 1 on the first
divergent frame (reported with both hashes), when the runs end
differently, or when they fail alike; 2 on bad usage.
"""
import os
import subprocess
import sys
import tempfile


def main() -> int:
    if len(sys.argv) < 2:
        print("usage: lockstep.py PROBE [probe args...]", file=sys.stderr)
        return 2
    probe, args = sys.argv[1], sys.argv[2:]
    with tempfile.TemporaryDirectory() as tmp:
        logs = {m: os.path.join(tmp, m + ".log") for m in ("native", "interp")}
        procs = {}
        for mode, log in logs.items():
            cmd = [probe, *args, "--hash-log", log]
            if mode == "interp":
                cmd.append("--interp-only")
            procs[mode] = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                           text=True)
        outs = {m: p.communicate()[0] for m, p in procs.items()}
        codes = {m: p.returncode for m, p in procs.items()}
        hashes = {}
        for mode, log in logs.items():
            with open(log) as f:
                hashes[mode] = [line.split() for line in f if line.strip()]

    nat, ref = hashes["native"], hashes["interp"]
    for k in range(min(len(nat), len(ref))):
        if nat[k] != ref[k]:
            print(f"lockstep: first divergent frame {ref[k][0]}: native {nat[k][1]}, "
                  f"interpreter {ref[k][1]}")
            return 1
    last = {m: outs[m].strip().splitlines()[-1:] for m in outs}
    if len(nat) != len(ref) or codes["native"] != codes["interp"]:
        print(f"lockstep: runs ended differently after {min(len(nat), len(ref))} matching "
              f"frames: native {len(nat)} frames, exit {codes['native']} {last['native']}; "
              f"interpreter {len(ref)} frames, exit {codes['interp']} {last['interp']}")
        return 1
    print(f"lockstep: {len(ref)} frames identical, both exit {codes['native']}")
    if codes["native"]:
        print(f"lockstep: both runs failed: {last['native']}")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
