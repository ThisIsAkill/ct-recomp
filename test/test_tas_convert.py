#!/usr/bin/env python3
"""tools/tas_convert.py on synthetic movies: a BizHawk .bk2 and an lsnes
.lsmv with the same input must convert to the same per-frame buttons;
savestate movies are refused; resets are reported."""
import os
import subprocess
import sys
import tempfile
import zipfile

TOOL = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "tools", "tas_convert.py")
NAMES = {"b": 0x8000, "y": 0x4000, "select": 0x2000, "start": 0x1000, "up": 0x0800,
         "down": 0x0400, "left": 0x0200, "right": 0x0100, "a": 0x0080, "x": 0x0040,
         "l": 0x0020, "r": 0x0010}
# Frame k (1-based) -> buttons: start on 3-4, a on 6, up+right 8-10, nothing else.
WANT = {3: {"start"}, 4: {"start"}, 6: {"a"}, 8: {"up", "right"}, 9: {"up", "right"},
        10: {"up", "right"}}
N = 12

fails = 0


def check(cond, msg):
    global fails
    if not cond:
        fails += 1
        print("FAIL", msg)


def run(movie, out, *extra):
    return subprocess.run([sys.executable, TOOL, movie, out, *extra], capture_output=True,
                          text=True)


def per_frame(path):
    got = {}
    for line in open(path):
        tok = line.split("#")[0].strip()
        if not tok:
            continue
        rng, b = tok.split(":")
        lo, hi = map(int, rng.split("-"))
        for f in range(lo, hi + 1):
            got.setdefault(f, set()).update(b.split("+"))
    return got


def bk2(path, savestate=False, reset_at=None):
    order = ["Up", "Down", "Left", "Right", "Select", "Start", "Y", "B", "X", "A", "L", "R"]
    key = "LogKey:#Reset|Power|#" + "|".join("P1 " + b for b in order) + "|"
    lines = ["[Input]", key]
    for f in range(1, N + 1):
        pressed = {b.lower() for b in WANT.get(f, set())}
        pad = "".join("UDLRsSYBXAlr"[i] if o.lower() in pressed else "." for i, o in
                      enumerate(order))
        lines.append("|" + ("r" if f == reset_at else ".") + ".|" + pad + "|")
    lines.append("[/Input]")
    with zipfile.ZipFile(path, "w") as z:
        z.writestr("Header.txt", "MovieVersion BizHawk v2.0\nPlatform SNES\n" +
                   ("StartsFromSavestate True\n" if savestate else ""))
        z.writestr("Input Log.txt", "\n".join(lines) + "\n")


def lsmv(path, savestate=False):
    order = ["b", "y", "select", "start", "up", "down", "left", "right", "a", "x", "l", "r"]
    lines = []
    for f in range(1, N + 1):
        pressed = WANT.get(f, set())
        pad = "".join("BYsSudlrAXLR"[i] if o in pressed else "." for i, o in enumerate(order))
        lines.append("F.|" + pad)
        lines.append("..|" + "." * 12)   # a second poll in the same frame
    with zipfile.ZipFile(path, "w") as z:
        z.writestr("input", "\n".join(lines) + "\n")
        z.writestr("port1", "gamepad\n")
        if savestate:
            z.writestr("savestate", b"\0")


with tempfile.TemporaryDirectory() as tmp:
    want = {f: set(b) for f, b in WANT.items()}
    for kind, make in (("bk2", bk2), ("lsmv", lsmv)):
        movie, out = os.path.join(tmp, "m." + kind), os.path.join(tmp, kind + ".txt")
        make(movie)
        r = run(movie, out)
        check(r.returncode == 0, f"{kind} converts: {r.stdout}")
        check(per_frame(out) == want, f"{kind} buttons: {per_frame(out)}")
        r = run(movie, out, "--offset", "5")
        check({f - 5: b for f, b in per_frame(out).items()} == want, f"{kind} offset")
        make(movie, savestate=True)
        r = run(movie, out)
        check(r.returncode == 1 and "savestate" in r.stdout, f"{kind} savestate refused: {r.stdout}")
    movie = os.path.join(tmp, "r.bk2")
    bk2(movie, reset_at=5)
    r = run(movie, os.path.join(tmp, "r.txt"))
    check(r.returncode == 0 and "1 resets dropped" in r.stdout, f"reset reported: {r.stdout}")

print(f"tas_convert: {'ok' if not fails else str(fails) + ' failed'}")
sys.exit(1 if fails else 0)
