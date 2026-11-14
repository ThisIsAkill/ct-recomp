#!/usr/bin/env python3
"""Convert an SNES input movie to an input script (runtime/replay.h).

usage: tas_convert.py MOVIE OUT [--offset D] [--rom ROM] [--sram-out FILE]

MOVIE is a BizHawk .bk2, an lsnes .lsmv, or a Mesen 2 .mmo (all zip
archives). Pad 1's buttons per frame become "F1-F2:buttons" spans; the
movie's frame k becomes script frame k + D (default 0). Only movies that
start from power on are accepted: one that starts from a savestate can't
be replayed from reset. A reset (the Reset button, or Power, which
is replayed as a reset: WRAM is kept either way) on movie frame k becomes
"reset k+D" (runtime/replay.h). Sub-frame resets (BizHawk's bsnes cores
log how far into the frame, "Reset Instruction") are replayed at the
frame's start, and the script notes where they were.

A .mmo is a replay chapter (#109): it may start from a save, the one the
previous chapter ended with. Its embedded start save (Battery.srm,
zero-padded to 8 KB; zeros when the movie carries none) is written to
--sram-out, and its SHA-1 goes in the script header ("# sram sha1 ...")
either way, so a chapter chain can be checked without the movie at hand.
A .mmo that would not replay from power-on is refused: it starts from a
savestate (SaveState.mss, which a random power-on RAM state also embeds),
was recorded with a ROM patch (PatchData.dat) or cheats, presses reset
or power mid-movie, uses pad 2 (or anything but a standard controller on
port 1), was recorded on another console than the SNES or by Mesen 1.x /
an old movie format, has a non-reference power-on RAM state (settings
other than snes.RamPowerOnState=AllZeros: game/ct/test/replay/
RECORDING.md), or --rom is given and its SHA-1 differs from the movie's.
--sram-out with a .bk2/.lsmv is refused: only a .mmo carries a start save
(the others are refused for starting with SRAM contents at all).

.bk2: "Input Log.txt" lines "|..|UDLRsSYBXAlr|..." per frame; columns are
matched to buttons through the LogKey line ("#Reset|Power|#P1 Up|...").
Axis columns come first in their group, as comma-terminated numbers
("|    0,..F|"; LogKey "#Reset Instruction|Reset|Power|Subframe|").
.lsmv: "input" lines, one per controller poll; a line starting with 'F'
begins a frame (its first line is used); the first '|' field is the system
flags (reset), then pad 1's 12 buttons in lsnes order B Y select start up
down left right A X L R ('.' released).
.mmo: "Input.txt" rows, one per input poll (Mesen polls once per frame,
checked against its frame count: row k is frame k+1), each a '|'-join of
per-device text states: the system buttons ("RP": reset, power) first,
then pad 1's 12 buttons in Mesen's order A B X Y L R select start up down
left right (chars "ABXYLRSTUDLR", '.' released), then pad 2's.
"GameSettings.txt" holds the movie header ("MesenVersion",
"MovieFormatVersion", "SHA1", ...) followed by the serialized emulator
settings (both "name value" per line) and any "Cheat" lines.

Exit 0 on success, 1 on an unusable movie, 2 on bad usage.
"""
import argparse
import hashlib
import sys
import zipfile

NAMES = {"b": 0x8000, "y": 0x4000, "select": 0x2000, "start": 0x1000, "up": 0x0800,
         "down": 0x0400, "left": 0x0200, "right": 0x0100, "a": 0x0080, "x": 0x0040,
         "l": 0x0020, "r": 0x0010}
LSNES_ORDER = ["b", "y", "select", "start", "up", "down", "left", "right", "a", "x", "l", "r"]
MMO_ORDER = ["a", "b", "x", "y", "l", "r", "select", "start", "up", "down", "left", "right"]
MMO_CHARS = "ABXYLRSTUDLR"   # Mesen's key names, in MMO_ORDER (S/T: select/start)
SRAM_SIZE = 0x2000


class Unusable(Exception):
    pass


def bk2_frames(z):
    names = z.namelist()
    header = z.read("Header.txt").decode("utf-8", "replace") if "Header.txt" in names else ""
    for line in header.splitlines():
        key, _, value = line.partition(" ")
        if key == "StartsFromSavestate" and value.strip().lower() == "true":
            raise Unusable("starts from a savestate")
        if key == "StartsFromSaveRam" and value.strip().lower() == "true":
            raise Unusable("starts with SRAM contents")
    if any(n.lower().startswith("saveram") for n in names):
        raise Unusable("carries SRAM contents")
    log = z.read("Input Log.txt").decode("utf-8", "replace").splitlines()
    columns = None
    frames, resets = [], []
    for line in log:
        if line.startswith("LogKey:"):
            groups = line[len("LogKey:"):].strip("#").split("#")
            columns = [[b.strip() for b in g.strip("|").split("|") if b.strip()] for g in groups]
        elif line.startswith("|") and columns is not None:
            fields = line.strip("|").split("|")
            mask, reset, axes = 0, False, {}
            for group, field in zip(columns, fields):
                names_left = list(group)
                while "," in field:   # axis values first: "  123,"
                    value, field = field.split(",", 1)
                    try:
                        axes[names_left.pop(0)] = int(value)
                    except (ValueError, IndexError):
                        raise Unusable(f"bad axis field in input line {line!r}")
                for name, ch in zip(names_left, field):
                    if ch in ". ":
                        continue
                    if name.lower() in ("reset", "power"):
                        reset = True
                    elif name.startswith("P1 "):
                        b = name[3:].lower()
                        if b in NAMES:
                            mask |= NAMES[b]
            if reset:
                resets.append((len(frames) + 1, axes.get("Reset Instruction", 0)))
            frames.append(mask)
    if columns is None:
        raise Unusable("no LogKey line in Input Log.txt")
    return frames, resets


def lsmv_frames(z):
    names = z.namelist()
    if "savestate" in names:
        raise Unusable("starts from a savestate")
    if any(n.startswith("moviesram.") for n in names):
        raise Unusable("starts with SRAM contents")
    port1 = z.read("port1").decode().strip() if "port1" in names else "gamepad"
    if port1 != "gamepad":
        raise Unusable(f"port 1 is {port1!r}, not a gamepad")
    frames, resets = [], []
    for line in z.read("input").decode("utf-8", "replace").splitlines():
        if not line.startswith("F"):
            continue   # later polls within the same frame
        fields = line.split("|")
        if "R" in fields[0][1:]:
            resets.append((len(frames) + 1, 0))
        pad = fields[1] if len(fields) > 1 else ""
        mask = 0
        for name, ch in zip(LSNES_ORDER, pad):
            if ch not in ". ":
                mask |= NAMES[name]
        frames.append(mask)
    return frames, resets


def mmo_frames(z, rom_sha1):
    """(frames, resets, sram): the start save is always the full 8 KB the
    replay starts from (see the docstring for the refusals)."""
    names = z.namelist()
    if "SaveState.mss" in names:
        raise Unusable("starts from a savestate")
    if "PatchData.dat" in names:
        raise Unusable("recorded with a ROM patch")
    settings, cheats = {}, 0
    for line in z.read("GameSettings.txt").decode("utf-8", "replace").splitlines():
        key, _, value = line.partition(" ")
        if key == "Cheat":
            cheats += 1
        elif key:
            settings[key] = value.strip()
    if cheats:
        raise Unusable("carries cheats")
    version = settings.get("MesenVersion", "")
    if len(version) < 2 or version[:2] in ("0.", "1."):
        raise Unusable(f"recorded by Mesen {version or '?'}, need Mesen 2")
    try:
        fmt = int(settings.get("MovieFormatVersion", "0"))
    except ValueError:
        fmt = 0
    if fmt < 2:
        raise Unusable(f"movie format version {settings.get('MovieFormatVersion', '?')}, need 2")
    console = settings.get("emu.consoleType")
    if console is not None and console != "Snes":
        raise Unusable(f"console is {console}, not Snes")
    ram = settings.get("snes.RamPowerOnState")
    if ram is not None and ram != "AllZeros":
        raise Unusable(f"snes.RamPowerOnState is {ram}, not AllZeros")
    if settings.get("snes.EnableRandomPowerOnState") in ("true", "1"):
        raise Unusable("random power-on state enabled, not AllZeros")
    sha1 = settings.get("SHA1", "").lower()
    if rom_sha1 and sha1 != rom_sha1:
        raise Unusable(f"recorded on another ROM (movie SHA1 {sha1 or '?'}, "
                       f"--rom SHA1 {rom_sha1})")
    for n in names:
        if n.startswith("Battery") and n != "Battery.srm":
            raise Unusable(f"carries battery file {n}, not plain SRAM")
    sram = bytes(SRAM_SIZE)
    if "Battery.srm" in names:
        data = z.read("Battery.srm")
        if len(data) > SRAM_SIZE:
            raise Unusable(f"Battery.srm is {len(data)} bytes, want at most {SRAM_SIZE}")
        sram = data + bytes(SRAM_SIZE - len(data))
    frames = []
    for line in z.read("Input.txt").decode("utf-8", "replace").splitlines():
        if not line.startswith("|"):
            continue   # Mesen itself only reads '|' rows
        f = len(frames) + 1   # row k is frame k+1: Mesen polls once a frame
        segs = line[1:].split("|")
        if len(segs) < 2:
            raise Unusable(f"frame {f}: input row {line!r} has no pad 1")
        for ch in segs[0]:
            if ch == "R":
                raise Unusable(f"reset pressed on frame {f}")
            if ch == "P":
                raise Unusable(f"power pressed on frame {f}")
            if ch != ".":
                raise Unusable(f"frame {f}: system buttons {segs[0]!r}")
        pad = segs[1]
        if len(pad) != 12 or any(c != "." and c != MMO_CHARS[i] for i, c in enumerate(pad)):
            raise Unusable(f"frame {f}: port 1 is not a SNES controller ({pad!r})")
        for extra in segs[2:]:
            if any(c not in ".:" for c in extra):
                raise Unusable(f"pad 2 input on frame {f}")
        mask = 0
        for name, ch in zip(MMO_ORDER, pad):
            if ch != ".":
                mask |= NAMES[name]
        frames.append(mask)
    return frames, [], sram


def spans(frames, offset):
    out, start = [], None
    for k in range(len(frames) + 1):
        cur = frames[k] if k < len(frames) else 0
        prev = frames[k - 1] if k > 0 else 0
        if k > 0 and cur != prev and prev:
            out.append((start + offset, k + offset, prev))   # index k-1 is frame k
        if cur != prev:
            start = k + 1
    return out


def fmt(mask):
    return "+".join(n for n in ["b", "y", "select", "start", "up", "down", "left", "right",
                                "a", "x", "l", "r"] if mask & NAMES[n])


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("movie")
    ap.add_argument("out")
    ap.add_argument("--offset", type=int, default=0)
    ap.add_argument("--rom", help="a .mmo must have been recorded on this ROM (SHA-1)")
    ap.add_argument("--sram-out", help="write a .mmo's start save (8 KB) here")
    a = ap.parse_args()
    rom_sha1 = ""
    if a.rom:
        try:
            with open(a.rom, "rb") as f:
                rom_sha1 = hashlib.sha1(f.read()).hexdigest()
        except OSError as e:
            print(f"tas_convert: cannot read ROM {a.rom}: {e.strerror or e}")
            return 2
    sram = None
    try:
        with zipfile.ZipFile(a.movie) as z:
            if "Input Log.txt" in z.namelist():
                frames, resets, kind = *bk2_frames(z), "bk2"
            elif "GameSettings.txt" in z.namelist() and "Input.txt" in z.namelist():
                frames, resets, sram = mmo_frames(z, rom_sha1)
                kind = "mmo"
            elif "input" in z.namelist():
                frames, resets, kind = *lsmv_frames(z), "lsmv"
            else:
                raise Unusable("neither a .bk2, .lsmv nor .mmo "
                               "(no Input Log.txt, input or GameSettings.txt+Input.txt)")
    except (Unusable, zipfile.BadZipFile, KeyError) as e:
        print(f"tas_convert: {a.movie}: {e}")
        return 1
    if a.sram_out and sram is None:
        print(f"tas_convert: {a.movie}: --sram-out: only a .mmo carries a start save")
        return 1
    out = spans(frames, a.offset)
    if out and out[0][0] < 1:
        print(f"tas_convert: offset {a.offset} moves input before frame 1")
        return 1
    with open(a.out, "w") as f:
        f.write(f"# Converted from {a.movie.split('/')[-1]} ({kind}, {len(frames)} frames) "
                f"by tools/tas_convert.py, offset {a.offset}.\n")
        if sram is not None:
            f.write(f"# sram sha1 {hashlib.sha1(sram).hexdigest()}\n")
        lines = [(lo, f"{lo}-{hi}:{fmt(m)}") for lo, hi, m in out]
        for k, at in resets:
            note = f"   # sub-frame in the movie: {at} instructions in" if at else ""
            lines.append((k + a.offset, f"reset {k + a.offset}{note}"))
        for _, text in sorted(lines, key=lambda t: t[0]):
            f.write(text + "\n")
        f.write(f"# frames {len(frames) + a.offset}\n")
    if a.sram_out:
        with open(a.sram_out, "wb") as f:
            f.write(sram)
    if resets and resets[0][0] + a.offset < 1:
        print(f"tas_convert: offset {a.offset} moves a reset before frame 1")
        return 1
    print(f"tas_convert: {kind}, {len(frames)} frames, {len(out)} spans"
          + (f", {len(resets)} resets" if resets else "")
          + (f", start save sha1 {hashlib.sha1(sram).hexdigest()}" if sram is not None else ""))
    return 0


if __name__ == "__main__":
    sys.exit(main())
