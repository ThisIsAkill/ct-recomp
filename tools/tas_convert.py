#!/usr/bin/env python3
"""Convert a TASVideos SNES input movie to an input script (runtime/replay.h).

usage: tas_convert.py MOVIE OUT [--offset D]

MOVIE is a BizHawk .bk2 or an lsnes .lsmv (both zip archives). Pad 1's
buttons per frame become "F1-F2:buttons" spans; the movie's frame k
becomes script frame k + D (default 0). Only movies that start from power
on are accepted: one that starts from a savestate or with SRAM contents
can't be replayed from reset. Resets inside the movie are reported with
their frames and dropped (the probe has no reset input).

.bk2: "Input Log.txt" lines "|..|UDLRsSYBXAlr|..." per frame; columns are
matched to buttons through the LogKey line ("#Reset|Power|#P1 Up|...").
.lsmv: "input" lines, one per controller poll; a line starting with 'F'
begins a frame (its first line is used); the first '|' field is the system
flags (reset), then pad 1's 12 buttons in lsnes order B Y select start up
down left right A X L R ('.' released).

Exit 0 on success, 1 on an unusable movie, 2 on bad usage.
"""
import argparse
import sys
import zipfile

NAMES = {"b": 0x8000, "y": 0x4000, "select": 0x2000, "start": 0x1000, "up": 0x0800,
         "down": 0x0400, "left": 0x0200, "right": 0x0100, "a": 0x0080, "x": 0x0040,
         "l": 0x0020, "r": 0x0010}
LSNES_ORDER = ["b", "y", "select", "start", "up", "down", "left", "right", "a", "x", "l", "r"]


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
            mask, reset = 0, False
            for group, field in zip(columns, fields):
                for name, ch in zip(group, field):
                    if ch == ".":
                        continue
                    if name.lower() in ("reset", "power"):
                        reset = True
                    elif name.startswith("P1 "):
                        b = name[3:].lower()
                        if b in NAMES:
                            mask |= NAMES[b]
            if reset:
                resets.append(len(frames) + 1)
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
            resets.append(len(frames) + 1)
        pad = fields[1] if len(fields) > 1 else ""
        mask = 0
        for name, ch in zip(LSNES_ORDER, pad):
            if ch not in ". ":
                mask |= NAMES[name]
        frames.append(mask)
    return frames, resets


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
    a = ap.parse_args()
    try:
        with zipfile.ZipFile(a.movie) as z:
            if "Input Log.txt" in z.namelist():
                frames, resets, kind = *bk2_frames(z), "bk2"
            elif "input" in z.namelist():
                frames, resets, kind = *lsmv_frames(z), "lsmv"
            else:
                raise Unusable("neither a .bk2 nor an .lsmv (no Input Log.txt or input)")
    except (Unusable, zipfile.BadZipFile, KeyError) as e:
        print(f"tas_convert: {a.movie}: {e}")
        return 1
    out = spans(frames, a.offset)
    if out and out[0][0] < 1:
        print(f"tas_convert: offset {a.offset} moves input before frame 1")
        return 1
    with open(a.out, "w") as f:
        f.write(f"# Converted from {a.movie.split('/')[-1]} ({kind}, {len(frames)} frames) "
                f"by tools/tas_convert.py, offset {a.offset}.\n")
        if resets:
            f.write(f"# Resets in the movie (dropped): frames {resets[:20]}\n")
        for lo, hi, m in out:
            f.write(f"{lo}-{hi}:{fmt(m)}\n")
        f.write(f"# frames {len(frames) + a.offset}\n")
    print(f"tas_convert: {kind}, {len(frames)} frames, {len(out)} spans"
          + (f", {len(resets)} resets dropped" if resets else ""))
    return 0


if __name__ == "__main__":
    sys.exit(main())
