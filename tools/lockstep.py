#!/usr/bin/env python3
"""Native vs interpreter lockstep check.

Runs the headless probe twice on the same arguments, once with native
dispatch and once with --interp-only, each writing per-frame state hashes
(--hash-log: CPU, WRAM, SRAM, VRAM, CGRAM, OAM, frame, APU at each frame
edge), and compares them frame by frame. At the first divergent frame it
names the components that differ, reruns both to that frame with --wram
and --vram dumps, and lists the first differing WRAM, VRAM, CGRAM and OAM
ranges.

usage: lockstep.py PROBE [probe args...]

Exit 0 when every frame matches and both runs exit 0; 1 on the first
divergent frame (reported with both hashes), when the runs end
differently, or when they fail alike; 2 on bad usage.
"""
import os
import subprocess
import sys
import tempfile


def ranges(a, b):
    out, start = [], None
    for k in range(len(a) + 1):
        d = k < len(a) and a[k] != b[k]
        if d and start is None:
            start = k
        elif not d and start is not None:
            out.append((start, k - 1))
            start = None
    return out


def detail(probe, args, frame):
    """Dump both runs at `frame` and print the first differing ranges."""
    with tempfile.TemporaryDirectory() as tmp:
        dumps = {}
        for mode in ("native", "interp"):
            w, v = os.path.join(tmp, mode + ".wram"), os.path.join(tmp, mode + ".vram")
            cmd = [probe, *strip_frames(args), "--frames", str(frame), "--wram", w, "--vram", v]
            if mode == "interp":
                cmd.append("--interp-only")
            subprocess.run(cmd, capture_output=True)
            if not (os.path.exists(w) and os.path.exists(v)):
                print(f"lockstep: could not dump the {mode} run at frame {frame}")
                return
            vv = open(v, "rb").read()
            dumps[mode] = {"wram": open(w, "rb").read(), "vram": vv[:0x10000],
                           "cgram": vv[0x10000:0x10200], "oam": vv[0x10200:]}
    for name in ("wram", "vram", "cgram", "oam"):
        a, b = dumps["native"][name], dumps["interp"][name]
        rs = ranges(a, b)
        if not rs:
            continue
        print(f"lockstep:   {name}: {sum(x != y for x, y in zip(a, b))} bytes in {len(rs)} ranges")
        for lo, hi in rs[:8]:
            print(f"lockstep:     ${lo:05X}-${hi:05X} native {a[lo:min(hi + 1, lo + 8)].hex(' ')}"
                  f" interp {b[lo:min(hi + 1, lo + 8)].hex(' ')}")


def strip_frames(args):
    out, skip = [], False
    for x in args:
        if skip:
            skip = False
        elif x == "--frames":
            skip = True
        else:
            out.append(x)
    return out


def main() -> int:
    if len(sys.argv) < 2:
        print("usage: lockstep.py PROBE [probe args...]", file=sys.stderr)
        return 2
    probe, args = sys.argv[1], sys.argv[2:]
    with tempfile.TemporaryDirectory() as tmp:
        logs = {m: os.path.join(tmp, m + ".log") for m in ("native", "interp")}
        procs = {}
        for mode, log in logs.items():
            # native overlay code must never outlive a change to its bytes here
            cmd = [probe, *args, "--hash-log", log, "--overlay-strict"]
            if mode == "interp":
                cmd.append("--interp-only")
            procs[mode] = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                           text=True)
        outs = {m: p.communicate()[0] for m, p in procs.items()}
        codes = {m: p.returncode for m, p in procs.items()}
        hashes = {}
        for mode, log in logs.items():
            if not os.path.exists(log):   # the probe refused its arguments, or crashed early
                print(f"lockstep: {mode} run wrote no hash log (exit {codes[mode]}): "
                      f"{outs[mode].strip()[-300:]}")
                return 1
            with open(log) as f:
                hashes[mode] = [line.split() for line in f if line.strip()]

    nat, ref = hashes["native"], hashes["interp"]
    for k in range(min(len(nat), len(ref))):
        if nat[k] != ref[k]:
            frame = ref[k][0]
            parts = [a.split("=")[0] for a, b in zip(nat[k][1:], ref[k][1:]) if a != b]
            print(f"lockstep: first divergent frame {frame}: {', '.join(parts)} differ")
            detail(probe, args, int(frame))
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
