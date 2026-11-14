#!/usr/bin/env python3
"""tools/tas_convert.py on synthetic movies: a BizHawk .bk2 and an lsnes
.lsmv with the same input must convert to the same per-frame buttons;
savestate movies are refused; resets become "reset F" lines, also from
BizHawk's bsnes log with its "Reset Instruction" axis column. A Mesen 2
.mmo (rows "|system|pad1|pad2") with the same input converts the same;
its embedded start save comes out via --sram-out with its SHA-1 in the
script header, and movies that would not replay from power-on are
refused: savestate, patch, cheats, reset/power press, pad 2, another
ROM (--rom), non-reference power-on settings, old formats."""
import hashlib
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
        if not tok or tok.startswith("reset"):
            continue
        rng, b = tok.split(":")
        lo, hi = map(int, rng.split("-"))
        for f in range(lo, hi + 1):
            got.setdefault(f, set()).update(b.split("+"))
    return got


def bk2(path, savestate=False, reset_at=None, bsnes=False):
    order = ["Up", "Down", "Left", "Right", "Select", "Start", "Y", "B", "X", "A", "L", "R"]
    system = "Reset Instruction|Reset|Power|Subframe" if bsnes else "Reset|Power"
    key = "LogKey:#" + system + "|#" + "|".join("P1 " + b for b in order) + "|"
    lines = ["[Input]", key]
    for f in range(1, N + 1):
        pressed = {b.lower() for b in WANT.get(f, set())}
        pad = "".join("UDLRsSYBXAlr"[i] if o.lower() in pressed else "." for i, o in
                      enumerate(order))
        flags = ("r" if f == reset_at else ".") + "."
        if bsnes:   # "|  123,r.F|": the axis, then Reset Power Subframe
            flags = f"{2312 if f == reset_at else 0:5d}," + flags + "."
        lines.append("|" + flags + "|" + pad + "|")
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


MMO_ORDER = ["a", "b", "x", "y", "l", "r", "select", "start", "up", "down", "left", "right"]
MMO_CHARS = "ABXYLRSTUDLR"


def mmo(path, rom_sha1="", battery=None, savestate=False, patch=False, cheat=False,
        ram="AllZeros", console="Snes", version="2.1.0", fmt=2, system_at=None,
        pad2_at=None, pad1_at=None, extra_battery=False):
    lines = []
    for f in range(1, N + 1):
        pressed = WANT.get(f, set())
        pad = "".join(MMO_CHARS[i] if o in pressed else "." for i, o in enumerate(MMO_ORDER))
        if pad1_at and f == pad1_at[0]:
            pad = pad1_at[1]
        system = system_at[1] if system_at and f == system_at[0] else ".."
        pad2 = "A..........." if pad2_at and f == pad2_at else "." * 12
        lines.append(f"|{system}|{pad}|{pad2}")
    settings = ["MesenVersion " + version, "MovieFormatVersion " + str(fmt),
                "GameFile game.sfc", "SHA1 " + rom_sha1,
                "emu.consoleType " + console,
                # GameSettings.txt goes on with the serialized settings
                "snes.RamPowerOnState " + ram,
                "snes.EnableRandomPowerOnState false"]
    if cheat:
        settings.append("Cheat GameGenie ABCDEF")
    with zipfile.ZipFile(path, "w") as z:
        z.writestr("GameSettings.txt", "\n".join(settings) + "\n")
        z.writestr("Input.txt", "\n".join(lines) + "\n")
        if battery is not None:
            z.writestr("Battery.srm", battery)
        if extra_battery:
            z.writestr("Battery.rtc", b"\0")
        if savestate:
            z.writestr("SaveState.mss", b"\0")
        if patch:
            z.writestr("PatchData.dat", b"\0")


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
    for bsnes in (False, True):
        movie, out = os.path.join(tmp, "r.bk2"), os.path.join(tmp, "r.txt")
        bk2(movie, reset_at=5, bsnes=bsnes)
        r = run(movie, out, "--offset", "2")
        text = open(out).read() if r.returncode == 0 else ""
        resets = [l.split("#")[0].split() for l in text.splitlines() if l.startswith("reset")]
        check(r.returncode == 0 and "1 resets" in r.stdout and resets == [["reset", "7"]],
              f"reset kept (bsnes={bsnes}): {r.stdout} {resets}")
        check({f - 2: b for f, b in per_frame(out).items()} == want if text else False,
              f"buttons with a reset (bsnes={bsnes})")
        check(("2312 instructions in" in text) == bsnes, f"sub-frame noted (bsnes={bsnes})")

    movie, out, srm = (os.path.join(tmp, "c.mmo"), os.path.join(tmp, "mmo.txt"),
                       os.path.join(tmp, "out.srm"))
    rom = os.path.join(tmp, "rom.sfc")
    with open(rom, "wb") as f:
        f.write(b"not the game, just bytes to hash" * 100)
    rom_sha1 = hashlib.sha1(open(rom, "rb").read()).hexdigest()

    mmo(movie, rom_sha1=rom_sha1)
    r = run(movie, out)
    check(r.returncode == 0, f"mmo converts: {r.stdout}")
    check(per_frame(out) == want, f"mmo buttons: {per_frame(out)}")
    check("# sram sha1 " + hashlib.sha1(bytes(0x2000)).hexdigest() in open(out).read(),
          "mmo header carries the zeroed save's sha1")

    seed = bytes(range(0x40, 0x48))   # a short battery file: zero-padded to 8 KB
    mmo(movie, rom_sha1=rom_sha1, battery=seed)
    r = run(movie, out, "--sram-out", srm, "--rom", rom)
    got = open(srm, "rb").read() if r.returncode == 0 and os.path.exists(srm) else b""
    check(r.returncode == 0 and got == seed + bytes(0x2000 - len(seed)),
          f"mmo start save extracted: {r.stdout} {len(got)} bytes")
    check("# sram sha1 " + hashlib.sha1(got).hexdigest() in open(out).read(),
          "mmo header carries the start save's sha1")

    mmo(movie, rom_sha1="0" * 40)
    r = run(movie, out, "--rom", rom)
    check(r.returncode == 1 and "another ROM" in r.stdout, f"wrong ROM refused: {r.stdout}")
    r = run(movie, out)
    check(r.returncode == 0, "without --rom the sha1 is only recorded")

    refusals = [
        (dict(savestate=True), "savestate"),
        (dict(patch=True), "patch"),
        (dict(cheat=True), "cheat"),
        (dict(system_at=(5, "R.")), "reset"),
        (dict(system_at=(5, ".P")), "power"),
        (dict(pad2_at=6), "pad 2"),
        (dict(ram="Random"), "AllZeros"),
        (dict(ram="AllOnes"), "AllZeros"),
        (dict(console="Nes"), "console"),
        (dict(version="0.9.9"), "Mesen 2"),
        (dict(fmt=1), "movie format"),
        (dict(battery=bytes(0x2001)), "Battery.srm"),
        (dict(extra_battery=True), "Battery.rtc"),
        (dict(pad1_at=(3, "120 40 ............")), "port 1"),
        (dict(pad1_at=(3, "....:.......")), "port 1"),
    ]
    for kw, word in refusals:
        mmo(movie, rom_sha1=rom_sha1, **kw)
        r = run(movie, out)
        check(r.returncode == 1 and word in r.stdout, f"refused {kw}: {r.stdout.strip()}")

    bk2(movie)
    r = run(movie, out, "--sram-out", srm)
    check(r.returncode == 1 and "start save" in r.stdout,
          f"--sram-out on a .bk2 refused: {r.stdout.strip()}")

print(f"tas_convert: {'ok' if not fails else str(fails) + ' failed'}")
sys.exit(1 if fails else 0)
