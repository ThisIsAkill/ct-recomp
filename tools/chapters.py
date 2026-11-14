#!/usr/bin/env python3
"""Replay chapters: convert, chain by save, and replay Mesen movies in order.

usage: chapters.py --probe PATH --rom ROM [--out-dir DIR] MOVIE...

Each MOVIE is a Mesen 2 .mmo recorded from power-on (see
game/ct/test/replay/RECORDING.md), given in chapter order. For each one:

  1. tools/tas_convert.py converts it to <stem>.txt (an input script,
     runtime/replay.h) and extracts the save it starts from to
     <stem>.start.srm, refusing anything that would not replay from
     power-on and any movie not recorded on --rom;
  2. the chapter must start from the save the previous chapter ended
     with: SHA-1 of <stem>.start.srm (cross-checked against the script
     header's "# sram sha1") against the previous <stem>.end.srm;
  3. the probe replays it headless from that save
     (--frames/--script/--sram-in/--sram-out, strict overlays, $CT_ROM
     set to --rom), leaving <stem>.end.srm.

Outputs land next to each movie (or in --out-dir). The scripts and saves
are what the replay tests consume (a lockstep test passes the same
--script/--sram-in pair to tools/lockstep.py), so a chain verified here
stays verified frame-by-frame in the test suite.

Exit 0 when every chapter converts, chains, and replays cleanly; 1 on
the first that does not; 2 on bad usage.
"""
import argparse
import hashlib
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
SRAM_SIZE = 0x2000


def probe_cmd(probe):
    """A .py probe runs under this interpreter (no shebang execution on
    Windows; the test's fake probe relies on it, as tools/lockstep.py)."""
    return [sys.executable, probe] if probe.endswith(".py") else [probe]


def sha1_file(path):
    with open(path, "rb") as f:
        return hashlib.sha1(f.read()).hexdigest()


def script_facts(path):
    """(frames, sram sha1) from a converted script's header/trailer."""
    frames = sram_sha1 = None
    with open(path) as f:
        for line in f:
            if line.startswith("# sram sha1 "):
                sram_sha1 = line.split()[-1]
            elif line.startswith("# frames "):
                frames = int(line.split()[-1])
    return frames, sram_sha1


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--probe", required=True)
    ap.add_argument("--rom", required=True)
    ap.add_argument("--out-dir")
    ap.add_argument("movies", nargs="+")
    a = ap.parse_args()
    env = dict(os.environ, CT_ROM=a.rom)
    prev_end = None   # (index, stem, sha1)
    for i, movie in enumerate(a.movies, 1):
        stem = os.path.splitext(os.path.basename(movie))[0]
        out_dir = a.out_dir or os.path.dirname(os.path.abspath(movie))
        txt = os.path.join(out_dir, stem + ".txt")
        start = os.path.join(out_dir, stem + ".start.srm")
        end = os.path.join(out_dir, stem + ".end.srm")
        r = subprocess.run([sys.executable, os.path.join(HERE, "tas_convert.py"), movie, txt,
                            "--rom", a.rom, "--sram-out", start],
                           capture_output=True, text=True)
        if r.returncode != 0:
            print(r.stdout, end="")
            print(f"chapters: chapter {i} ({stem}): conversion failed")
            return 1
        frames, header_sha1 = script_facts(txt)
        start_sha1 = sha1_file(start)
        if frames is None or header_sha1 != start_sha1:
            print(f"chapters: chapter {i} ({stem}): script header does not match "
                  f"its start save")
            return 1
        if prev_end is not None and start_sha1 != prev_end[2]:
            print(f"chapters: chapter {i} ({stem}) does not start from chapter "
                  f"{prev_end[0]}'s ({prev_end[1]}) end save: starts from sha1 "
                  f"{start_sha1}, which ended with {prev_end[2]}")
            return 1
        r = subprocess.run([*probe_cmd(a.probe), "--frames", str(frames), "--script", txt,
                            "--sram-in", start, "--sram-out", end, "--overlay-strict"],
                           env=env, capture_output=True, text=True)
        if r.returncode != 0:
            print(r.stdout, end="")
            print(r.stderr, end="")
            print(f"chapters: chapter {i} ({stem}): replay failed (probe exit "
                  f"{r.returncode})")
            return 1
        if not (os.path.exists(end) and os.path.getsize(end) == SRAM_SIZE):
            print(f"chapters: chapter {i} ({stem}): probe left no end save")
            return 1
        end_sha1 = sha1_file(end)
        print(f"chapters: {i}/{len(a.movies)} {stem}: {frames} frames, "
              f"start {start_sha1[:12]}..., end {end_sha1[:12]}...")
        prev_end = (i, stem, end_sha1)
    print(f"chapters: {len(a.movies)} chapter{'s' if len(a.movies) != 1 else ''} chained")
    return 0


if __name__ == "__main__":
    sys.exit(main())
