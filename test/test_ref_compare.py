#!/usr/bin/env python3
"""tools/ref_compare.py's comparison on synthetic hash logs: identical logs
match; a single-byte WRAM change at one frame (hashed the same way both
sides hash WRAM) is reported at exactly that frame, including when it is
undone the next frame; a frame missing from the reference log is reported,
not skipped; frames ending inside a general DMA skip only WRAM."""
import os
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, "..", "tools"))
import ref_compare  # noqa: E402


def fnv(b):
    h = 0xCBF29CE484222325
    for x in b:
        h = ((h ^ x) * 0x100000001B3) & 0xFFFFFFFFFFFFFFFF
    return h


def log(path, frames, poke=None, drop=(), dma=()):
    """A --ref-log over a WRAM that changes one byte per frame; `poke`
    = (frame, addr) flips one more byte at that frame only."""
    wram = bytearray(0x20000)
    with open(path, "w") as out:
        for f in range(1, frames + 1):
            wram[f % 64] = f & 0xFF
            w = bytearray(wram)
            if poke and poke[0] == f:
                w[poke[1]] ^= 0xFF
            if f not in drop:
                out.write(f"{f} {fnv(w):016x} {f:016x}{' dma' if f in dma else ''}\n")
    return ref_compare.read_log(path)


def main() -> int:
    fails = 0

    def check(what, got, want):
        nonlocal fails
        if got != want:
            print(f"FAIL {what}: got {got}, want {want}")
            fails += 1

    with tempfile.TemporaryDirectory() as tmp:
        p = lambda n: os.path.join(tmp, n)
        ref = log(p("ref"), 40)
        first, differ, compared, missing = ref_compare.compare(log(p("same"), 40), ref, 0)
        check("identical", (first, compared, missing), ({"wram": None, "frame": None}, 39, []))
        for frame, addr in ((2, 0), (17, 0x1FFFF), (40, 0x12345)):
            first, differ, _, _ = ref_compare.compare(log(p("poke"), 40, (frame, addr)), ref, 0)
            check(f"poke {frame}:{addr:X}", (first["wram"], differ["wram"], first["frame"]),
                  (frame, [frame], None))
        _, _, compared, missing = ref_compare.compare(log(p("same"), 40), log(p("short"), 40,
                                                      drop={9, 30}), 0)
        check("missing reference frames", (compared, missing), (37, [9, 30]))
        first, _, _, _ = ref_compare.compare(log(p("dma"), 40, (12, 5), dma={12}), ref, 0)
        check("dma frame skips WRAM", first["wram"], None)
    print("test_ref_compare: " + ("ok" if not fails else f"{fails} failed"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
