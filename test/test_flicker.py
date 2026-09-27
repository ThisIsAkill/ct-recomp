#!/usr/bin/env python3
"""Frame-dump flicker check.

usage: test_flicker.py PROBE FROM TO [probe args...]

Runs the probe to frame TO with --dump and compares the frames FROM..TO
pixel by pixel. Frame f flickers when it differs from frame f-1 in more
than 1% of pixels but from frame f-2 in under a quarter as many: the
image jumps away and back, like sprites dropping out on every other
frame. Legitimate effects do it briefly (a character shaking when hit);
more than MAX_FLICKER such frames fails. Exit 0 if within, 1 otherwise.
"""
import os
import struct
import subprocess
import sys
import tempfile
import zlib

MAX_FLICKER = 20


def load_png(path):
    """RGB rows of a PNG written by runtime/png.c (8-bit RGB, filter 0)."""
    data = open(path, "rb").read()
    pos, idat, width, height = 8, b"", 0, 0
    while pos < len(data):
        n = struct.unpack(">I", data[pos:pos + 4])[0]
        kind = data[pos + 4:pos + 8]
        body = data[pos + 8:pos + 8 + n]
        if kind == b"IHDR":
            width, height = struct.unpack(">II", body[:8])
        elif kind == b"IDAT":
            idat += body
        pos += 12 + n
    raw = zlib.decompress(idat)
    stride = width * 3 + 1
    return [raw[y * stride + 1:(y + 1) * stride] for y in range(height)]


def differing(a, b):
    n = 0
    for ra, rb in zip(a, b):
        if ra != rb:
            n += sum(ra[k:k + 3] != rb[k:k + 3] for k in range(0, len(ra), 3))
    return n


def main() -> int:
    if len(sys.argv) < 4:
        print("usage: test_flicker.py PROBE FROM TO [probe args...]", file=sys.stderr)
        return 2
    probe, lo, hi, args = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), sys.argv[4:]
    with tempfile.TemporaryDirectory() as tmp:
        run = subprocess.run([probe, "--frames", str(hi), "--dump", tmp, *args],
                             capture_output=True, text=True)
        if run.returncode:
            print(f"test_flicker: probe exit {run.returncode}: {run.stdout.strip()[-300:]}")
            return 1
        dumped = sorted(int(n[6:11]) for n in os.listdir(tmp) if n.startswith("frame_"))
        # The probe writes a frame only when it changed: unwritten = previous.
        frames, last = {}, None
        prior = [d for d in dumped if d < lo - 2]
        if prior:
            last = load_png(os.path.join(tmp, f"frame_{prior[-1]:05d}.png"))
        for f in range(lo - 2, hi + 1):
            if f in dumped:
                last = load_png(os.path.join(tmp, f"frame_{f:05d}.png"))
            frames[f] = last
    total = 256 * 224
    flicker = []
    for f in range(lo, hi + 1):
        if frames[f] is None or frames[f - 2] is None:
            continue
        d1 = differing(frames[f], frames[f - 1])
        if d1 > total // 100 and differing(frames[f], frames[f - 2]) * 4 < d1:
            flicker.append(f)
    print(f"test_flicker: frames {lo}-{hi}: {len(flicker)} flicker frames (max {MAX_FLICKER})"
          + (f", first {flicker[:6]}" if flicker else ""))
    return 1 if len(flicker) > MAX_FLICKER else 0


if __name__ == "__main__":
    sys.exit(main())
