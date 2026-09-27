#!/usr/bin/env python3
"""Compare the headless probe against a reference emulator on the same input.

usage: ref_compare.py --probe PATH --mesen PATH --rom ROM --frames N
                      [--script FILE] [--offset D] [--setting Name.Path=value ...]

Runs `PROBE --ref-log` and tools/mesen_ref.py side by side for N frames
with the same input script, and compares the per-frame hashes (WRAM, and
the visible frame as 15-bit pixels): our frame f against the reference's
frame f + D (default 0; both count frames completed). Reports the first
frame where WRAM differs and the first where the image differs; for the
WRAM one, runs both again to that frame and lists the differing WRAM
ranges. The reference's first frame is skipped (it starts mid-frame).

Exit 0 if nothing differs, 1 if something does (a report, not a test
verdict: timing is approximate by design), 2 on bad usage.
"""
import argparse
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))


def read_log(path):
    out = {}
    for line in open(path):
        f, w, v = line.split()
        out[int(f)] = (w, v)
    return out


def ranges(a, b, limit=24):
    """Differing byte ranges of two equal-length buffers, as (start, end)."""
    out, start = [], None
    for k in range(len(a) + 1):
        diff = k < len(a) and a[k] != b[k]
        if diff and start is None:
            start = k
        elif not diff and start is not None:
            out.append((start, k - 1))
            start = None
    return out[:limit], len(out)


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--probe", required=True)
    ap.add_argument("--mesen", required=True)
    ap.add_argument("--rom", required=True)
    ap.add_argument("--frames", type=int, required=True)
    ap.add_argument("--script")
    ap.add_argument("--offset", type=int, default=0)
    ap.add_argument("--setting", action="append", default=[])
    a = ap.parse_args()
    env = dict(os.environ, CT_ROM=a.rom)
    script = ["--script", a.script] if a.script else []
    settings = [x for s in a.setting for x in ("--setting", s)]
    with tempfile.TemporaryDirectory() as tmp:
        ours_log, ref_log = os.path.join(tmp, "ours.log"), os.path.join(tmp, "ref.log")
        ref_frames = a.frames + max(a.offset, 0)
        ours = subprocess.Popen([a.probe, "--frames", str(a.frames), "--ref-log", ours_log,
                                 *script], env=env, stdout=subprocess.PIPE,
                                stderr=subprocess.STDOUT, text=True)
        ref = subprocess.run([sys.executable, os.path.join(HERE, "mesen_ref.py"), "--mesen",
                              a.mesen, "--rom", a.rom, "--frames", str(ref_frames), "--out",
                              ref_log, *script, *settings], capture_output=True, text=True)
        ours_out = ours.communicate()[0]
        if ref.returncode:
            print(f"ref_compare: reference run failed: {ref.stdout.strip()[-300:]}")
            return 2
        mine, theirs = read_log(ours_log), read_log(ref_log)
        if not mine:
            print(f"ref_compare: probe logged nothing: {ours_out.strip()[-300:]}")
            return 2
        first = {"wram": None, "frame": None}
        for f in sorted(mine):
            g = f + a.offset
            if g < 2 or g not in theirs:
                continue
            for k, name in ((0, "wram"), (1, "frame")):
                if first[name] is None and mine[f][k] != theirs[g][k]:
                    first[name] = f
        compared = sum(1 for f in mine if f + a.offset >= 2 and f + a.offset in theirs)
        print(f"ref_compare: {compared} frames compared (ours f vs reference f{a.offset:+d})")
        for name in ("wram", "frame"):
            f = first[name]
            print(f"ref_compare: first {name} divergence: " +
                  (f"frame {f}" if f is not None else "none"))
        f = first["wram"]
        if f is not None:
            ours_wram, ref_wram = os.path.join(tmp, "ours.wram"), os.path.join(tmp, "ref.wram")
            subprocess.run([a.probe, "--frames", str(f), "--wram", ours_wram, *script], env=env,
                           capture_output=True)
            subprocess.run([sys.executable, os.path.join(HERE, "mesen_ref.py"), "--mesen",
                            a.mesen, "--rom", a.rom, "--frames", str(f + a.offset), "--out",
                            os.path.join(tmp, "ref2.log"), "--wram-at", str(f + a.offset),
                            "--wram-out", ref_wram, *script, *settings], capture_output=True)
            if os.path.exists(ours_wram) and os.path.exists(ref_wram):
                x, y = open(ours_wram, "rb").read(), open(ref_wram, "rb").read()
                rs, n = ranges(x, y)
                print(f"ref_compare: frame {f}: {sum(p != q for p, q in zip(x, y))} WRAM bytes "
                      f"differ in {n} ranges; first:")
                for lo, hi in rs:
                    print(f"  $7E{lo:04X}" if lo < 0x10000 else f"  $7F{lo - 0x10000:04X}",
                          f"({hi - lo + 1} bytes) ours {x[lo:min(hi + 1, lo + 8)].hex(' ')}"
                          f" ref {y[lo:min(hi + 1, lo + 8)].hex(' ')}")
        return 0 if first["wram"] is None and first["frame"] is None else 1


if __name__ == "__main__":
    sys.exit(main())
