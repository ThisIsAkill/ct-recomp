#!/usr/bin/env python3
"""tools/chapters.py (#109) against a fake probe: two synthetic .mmo
chapters chained by their saves convert, replay in order (the probe's
end save is its start save with byte 0 advanced by the frame count),
and leave script + start/end saves behind; a chapter that does not start
from the previous end save, a failing replay, and a movie the converter
refuses all fail the chain."""
import hashlib
import json
import os
import subprocess
import sys
import tempfile
import zipfile

HERE = os.path.dirname(os.path.abspath(__file__))
TOOL = os.path.join(HERE, "..", "tools", "chapters.py")
N = 12

FAKE_PROBE = r"""
import json, os, sys
args = sys.argv[1:]
if os.environ.get("FAKE_PROBE_LOG"):
    with open(os.environ["FAKE_PROBE_LOG"], "a") as f:
        f.write(json.dumps(args) + "\n")
if os.environ.get("FAKE_PROBE_FAIL"):
    print("fake probe: asked to fail")
    sys.exit(3)
def val(flag):
    return args[args.index(flag) + 1] if flag in args else None
frames = int(val("--frames"))
sram = bytearray(open(val("--sram-in"), "rb").read())
sram[0] = (sram[0] + frames) & 0xFF
with open(val("--sram-out"), "wb") as f:
    f.write(sram)
print(f"fake probe: {frames} frames")
"""

fails = 0


def check(cond, msg):
    global fails
    if not cond:
        fails += 1
        print("FAIL", msg)


def mmo(path, rom_sha1, battery=None, savestate=False):
    rows = ["|..|" + "." * 12 + "|" + "." * 12] * (N - 1)
    rows.append("|..|.......T....|" + "." * 12)   # start on the last frame
    settings = ["MesenVersion 2.1.0", "MovieFormatVersion 2", "GameFile game.sfc",
                "SHA1 " + rom_sha1, "emu.consoleType Snes",
                "snes.RamPowerOnState AllZeros", "snes.EnableRandomPowerOnState false"]
    with zipfile.ZipFile(path, "w") as z:
        z.writestr("GameSettings.txt", "\n".join(settings) + "\n")
        z.writestr("Input.txt", "\n".join(rows) + "\n")
        if battery is not None:
            z.writestr("Battery.srm", battery)
        if savestate:
            z.writestr("SaveState.mss", b"\0")


def end_after(start, frames=N):
    out = bytearray(start)
    out[0] = (out[0] + frames) & 0xFF
    return bytes(out)


with tempfile.TemporaryDirectory() as tmp:
    p = lambda n: os.path.join(tmp, n)
    with open(p("rom.sfc"), "wb") as f:
        f.write(b"bytes standing in for the ROM" * 100)
    rom_sha1 = hashlib.sha1(open(p("rom.sfc"), "rb").read()).hexdigest()
    probe = p("fake_probe.py")
    with open(probe, "w") as f:
        f.write(FAKE_PROBE)
    env = dict(os.environ, FAKE_PROBE_LOG=p("probe.log"))

    def run(*movies, extra_env=None):
        return subprocess.run([sys.executable, TOOL, "--probe", probe, "--rom", p("rom.sfc"),
                               *movies], capture_output=True, text=True,
                              env={**env, **(extra_env or {})})

    mmo(p("01-first.mmo"), rom_sha1)
    mmo(p("02-second.mmo"), rom_sha1, battery=end_after(bytes(0x2000)))
    r = run(p("01-first.mmo"), p("02-second.mmo"))
    check(r.returncode == 0, f"chain converts and replays: {r.stdout} {r.stderr}")
    for stem in ("01-first", "02-second"):
        for suffix in (".txt", ".start.srm", ".end.srm"):
            check(os.path.exists(p(stem + suffix)), f"{stem}{suffix} left behind")
    if r.returncode == 0:
        check(open(p("01-first.end.srm"), "rb").read()
              == open(p("02-second.start.srm"), "rb").read(),
              "chapter 2 starts where chapter 1 ended")
        calls = [json.loads(l) for l in open(p("probe.log"))]
        check(len(calls) == 2 and all(c[c.index("--frames") + 1] == str(N) for c in calls)
              and all("--sram-in" in c and "--sram-out" in c for c in calls),
              f"probe replayed each chapter from its start save: {calls}")

    bad = bytearray(end_after(bytes(0x2000)))
    bad[0] ^= 0xFF
    mmo(p("02-second.mmo"), rom_sha1, battery=bytes(bad))
    r = run(p("01-first.mmo"), p("02-second.mmo"))
    check(r.returncode == 1 and "chapter 2" in r.stdout and "does not start" in r.stdout,
          f"broken chain refused: {r.stdout}")

    r = run(p("01-first.mmo"), extra_env={"FAKE_PROBE_FAIL": "1"})
    check(r.returncode == 1, f"failing replay fails the chain: {r.stdout}")

    mmo(p("03-state.mmo"), rom_sha1, savestate=True)
    r = run(p("03-state.mmo"))
    check(r.returncode == 1 and "savestate" in r.stdout,
          f"converter refusal surfaces: {r.stdout}")

print(f"chapters: {'ok' if not fails else str(fails) + ' failed'}")
sys.exit(1 if fails else 0)
