#!/usr/bin/env python3
"""Compare the headless probe against a reference emulator on the same input.

usage: ref_compare.py --probe PATH --mesen PATH --rom ROM --frames N
                      [--script FILE] [--offset D] [--setting Name.Path=value ...]
                      [--no-known-differences] [--inject-wram F[:ADDR]]

Runs `PROBE --ref-log` and tools/mesen_ref.py side by side for N frames
with the same input script, and compares the per-frame hashes (WRAM, and
the visible frame as 15-bit pixels): our frame f against the reference's
frame f + D (default 0; both count frames completed). Reports the first
frame where WRAM differs and the first where the image differs; for the
WRAM one, runs both again to that frame and lists the differing WRAM
ranges. The reference's first frame is skipped (it starts mid-frame).

Frames the probe marks as ending inside a general DMA skip the WRAM
comparison: the probe moves all of a DMA's bytes at its start and then lets
the time pass, where hardware (and the reference) moves one byte per 8
clocks, so a WRAM snapshot there catches the reference partway. Nothing can
observe the difference (the CPU is stopped for the DMA); the picture is
still compared.

Known differences: where the reference emulator is known to differ from
hardware and ct-recomp follows hardware, KNOWN_DIFFERENCES below lists it,
and the probe reproduces the reference's behavior for that one point
(`--ref-quirk`) so everything else is still compared exactly. Nothing else
is tolerated; --no-known-differences compares plain hardware behavior.

Every probe frame from 2 on must be in the reference log: a missing frame
is an error (exit 2), never skipped.

Self-test: --inject-wram F[:ADDR] flips one WRAM byte in the probe at the
edge of frame F (ADDR hex, default 1FFFF; `ct_boot --poke-wram F:ADDR^FF`) and runs
the same comparison; it passes (exit 0) only if the first WRAM divergence
reported is exactly frame F. F must be a frame that otherwise matches.

Exit 0 if nothing differs, 1 if something does (a report, not a test
verdict), 2 on bad usage or a run that fails.
"""
import argparse
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))

# (probe --ref-quirk name, what the reference does, what hardware does)
KNOWN_DIFFERENCES = [
    ("mesen-dma-count8",
     "Mesen 2: after a general DMA, the wait back to a whole CPU cycle counts "
     "each channel's bytes in 8 bits (size mod 256)",
     "every byte counts (fullsnes, bsnes)"),
]


def read_log(path):
    out = {}
    for line in open(path):
        f, w, v, *flags = line.split()
        out[int(f)] = (w, v, "dma" in flags)
    return out


def compare(mine, theirs, offset):
    """Compare two read_log() results: our frame f against the reference's
    f + offset, from reference frame 2 on. Returns (first, differ, compared,
    missing): first/differ keyed "wram" and "frame" (the first differing
    frame or None, and every differing frame), the number of frames
    compared, and our frames the reference log lacks."""
    first = {"wram": None, "frame": None}
    differ = {"wram": [], "frame": []}
    compared, missing = 0, []
    for f in sorted(mine):
        g = f + offset
        if g < 2:
            continue
        if g not in theirs:
            missing.append(f)
            continue
        compared += 1
        for k, name in ((0, "wram"), (1, "frame")):
            if name == "wram" and mine[f][2]:
                continue   # the frame edge fell inside a general DMA (see above)
            if mine[f][k] != theirs[g][k]:
                differ[name].append(f)
                if first[name] is None:
                    first[name] = f
    return first, differ, compared, missing


def runs(frames, limit=8):
    """Frame runs "a-b" of a sorted list, the first `limit` of them."""
    out = []
    for f in frames:
        if out and f == out[-1][1] + 1:
            out[-1][1] = f
        else:
            out.append([f, f])
    text = ", ".join(f"{lo}-{hi}" if hi > lo else f"{lo}" for lo, hi in out[:limit])
    return text + (f", ... ({len(out)} runs)" if len(out) > limit else "")


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
    ap.add_argument("--no-known-differences", action="store_true")
    ap.add_argument("--inject-wram")
    a = ap.parse_args()
    inject, poke = None, []
    if a.inject_wram:
        fr, _, addr = a.inject_wram.partition(":")
        inject, addr = int(fr), int(addr or "1FFFF", 16)
        if not 2 <= inject <= a.frames or addr > 0x1FFFF:
            print("ref_compare: --inject-wram needs 2 <= F <= --frames and ADDR <= 1FFFF")
            return 2
        poke = ["--poke-wram", f"{inject}:{addr:X}^FF"]
    quirks = [] if a.no_known_differences else \
        [x for name, _, _ in KNOWN_DIFFERENCES for x in ("--ref-quirk", name)]
    env = dict(os.environ, CT_ROM=a.rom)
    script = ["--script", a.script] if a.script else []
    settings = [x for s in a.setting for x in ("--setting", s)]
    with tempfile.TemporaryDirectory() as tmp:
        ours_log, ref_log = os.path.join(tmp, "ours.log"), os.path.join(tmp, "ref.log")
        ref_frames = a.frames + max(a.offset, 0)
        ours = subprocess.Popen([a.probe, "--frames", str(a.frames), "--ref-log", ours_log,
                                 "--overlay-strict",
                                 *quirks, *script, *poke], env=env, stdout=subprocess.PIPE,
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
        first, differ, compared, missing = compare(mine, theirs, a.offset)
        if missing:
            print(f"ref_compare: reference log lacks {len(missing)} probe frames: {runs(missing)}")
            return 2
        print(f"ref_compare: {compared} frames compared (ours f vs reference f{a.offset:+d})")
        skipped = sum(1 for f in mine if mine[f][2])
        if skipped:
            print(f"ref_compare: {skipped} frames end inside a general DMA: WRAM not compared there")
        for name, ref, hw in ([] if a.no_known_differences else KNOWN_DIFFERENCES):
            print(f"ref_compare: known difference reproduced ({name}): {ref}; hardware: {hw}")
        for name in ("wram", "frame"):
            f = first[name]
            print(f"ref_compare: first {name} divergence: " +
                  (f"frame {f}" if f is not None else "none"))
            if f is not None:
                print(f"ref_compare: {name} differs on {len(differ[name])} frames: "
                      f"{runs(differ[name])}")
        f = first["wram"]
        if f is not None:
            ours_wram, ref_wram = os.path.join(tmp, "ours.wram"), os.path.join(tmp, "ref.wram")
            subprocess.run([a.probe, "--frames", str(f), "--wram", ours_wram, "--overlay-strict",
                            *quirks, *script, *poke], env=env, capture_output=True)
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
        if inject is not None:
            if mine.get(inject, (0, 0, False))[2]:
                print(f"ref_compare: self-test: frame {inject} ends inside a general DMA "
                      f"(WRAM not compared there); pick another frame")
                return 2
            ok = first["wram"] == inject
            print(f"ref_compare: self-test {'passed' if ok else 'FAILED'}: byte injected at "
                  f"frame {inject}, first WRAM divergence reported at "
                  f"{first['wram'] if first['wram'] is not None else 'none'}")
            return 0 if ok else 1
        return 0 if first["wram"] is None and first["frame"] is None else 1


if __name__ == "__main__":
    sys.exit(main())
