#!/usr/bin/env python3
"""Find the code blobs the game decompresses into WRAM (#92) and write
game/ct/overlays.toml.

Runs BUILD/ct_boot over the boot and the replays in test/replay/, logging
every call of the decompressor (--log-entry C30557:0300: its arguments are at
$0300, the direct page it sets: source $00-$02, WRAM destination $03-$04, bank $7F if
bit 0 of $05) and profiling what runs where (--profile, interpreted only, plus
--wram-entries for code entered from ROM by a jump rather than a call). Each call's source
is decompressed with ct_decompress.py. A blob is code if a run entered WRAM
inside it while it was there: a second pass logs every execution of those
entries with its clock, and each is charged to the latest load covering its
address before it (buffers are reused: graphics and code land at the same
addresses at different times). Those entries, with the M/X they were entered
in, become the overlay's entry points (source = "profile": observed at run
time). The same source loaded at several destinations gives one overlay per
destination.

overlays.toml holds pointers, sizes and a 64-bit FNV-1a hash per blob (to
check the decompressor port against at build time), never ROM bytes.
WRAM entries no blob covers are reported: that code got there another way.

usage: find_overlays.py BUILD_DIR [--write]
"""
from __future__ import annotations

import os
import re
import subprocess
import sys
import tempfile

GAME = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))   # game/ct
sys.path.insert(0, os.path.join(GAME, 'tools'))
sys.path.insert(0, os.path.join(GAME, '..', '..', 'recomp'))

import ct_decompress  # noqa: E402
import decode  # noqa: E402

DECOMPRESS = 0xC30557
BOOT_FRAMES = 1500
ENTRY_RE = re.compile(r'^entry ([0-9A-F]{6}) frame \d+ line \d+ clock (\d+) .* m (\d) x (\d) '
                      r'dp((?: [0-9A-F-]{2}){16})$')
PROF_RE = re.compile(r'\$(7[EF][0-9A-F]{4}) m([01])x([01])e0\s')


def runs() -> list[tuple[str, list[str]]]:
    out = [('boot', ['--frames', str(BOOT_FRAMES)])]
    replay = os.path.join(GAME, 'test', 'replay')
    for name in sorted(os.listdir(replay)):
        path = os.path.join(replay, name)
        frames = None   # "# frames N", else 10 past the last span (as the lockstep tests)
        last = 0
        for line in open(path):
            m = re.match(r'#\s*frames\s+(\d+)', line)
            if m:
                frames = int(m.group(1))
            m = re.match(r'\s*\d+-(\d+):', line)
            if m:
                last = max(last, int(m.group(1)))
        frames = frames or (last + 10 if last else None)
        if frames:
            out.append((name.rsplit('.', 1)[0], ['--frames', str(frames), '--script', path]))
    return out


def main(argv: list[str]) -> int:
    if not argv or argv[0].startswith('-'):
        print(__doc__.strip().splitlines()[-1], file=sys.stderr)
        return 2
    probe = os.path.join(argv[0], 'ct_boot')
    rom = decode.load_rom()
    # pass 1: which WRAM entries run at all: call and interrupt targets (the
    # profile) and places execution reaches from ROM by a jump
    # (--wram-entries); interpreted only, so compiled overlays don't hide theirs
    wram: set[int] = set()
    for name, args in runs():
        jumps = os.path.join(tempfile.gettempdir(), f'find_overlays_{os.getpid()}.txt')
        r = subprocess.run([probe, *args, '--interp-only', '--profile', '1000000',
                            '--wram-entries', jumps], capture_output=True, text=True)
        wram |= {int(m.group(1), 16) for m in map(PROF_RE.search, r.stdout.splitlines()) if m}
        if os.path.exists(jumps):
            wram |= {int(line.split()[0], 16) for line in open(jumps) if line.strip()}
            os.remove(jumps)
    if len(wram) > 63:
        print(f'find_overlays: {len(wram)} WRAM entries, more than ct_boot logs', file=sys.stderr)
        return 1
    # pass 2: loads and executions in time order
    sizes: dict[int, int] = {}
    loads: dict[tuple[int, int], set[str]] = {}          # (src, dst) -> runs
    entries: dict[tuple[int, int, int], set[str]] = {}   # (src, dst, entry) -> states
    for name, args in runs():
        log = [f'{DECOMPRESS:06X}:0300'] + [f'{a:06X}' for a in sorted(wram)]
        r = subprocess.run([probe, *args, *(x for a in log for x in ('--log-entry', a))],
                           capture_output=True, text=True)
        resident: list[tuple[int, int, int]] = []   # (dst, end, src), latest last
        n_calls = 0
        for line in r.stdout.splitlines():
            m = ENTRY_RE.match(line)
            if not m:
                continue
            at = int(m.group(1), 16)
            if at == DECOMPRESS:
                dp = [int(b, 16) if b != '--' else 0 for b in m.group(5).split()]
                src = dp[0] | dp[1] << 8 | dp[2] << 16
                dst = (0x7F if dp[5] & 1 else 0x7E) << 16 | dp[3] | dp[4] << 8
                if src not in sizes:
                    try:
                        sizes[src] = len(ct_decompress.decompress(rom, src))
                    except ct_decompress.DecompressError as ex:
                        print(f'# skip ${src:06X}: {ex}', file=sys.stderr)
                        sizes[src] = 0
                loads.setdefault((src, dst), set()).add(name)
                resident.append((dst, dst + sizes[src], src))
                n_calls += 1
                continue
            owner = next((r_ for r_ in reversed(resident) if r_[0] <= at < r_[1]), None)
            state = f'm{m.group(3)}x{m.group(4)}'
            if owner is None:
                entries.setdefault((0, 0, at), set()).add(state)
            else:
                entries.setdefault((owner[2], owner[0], at), set()).add(state)
        print(f'# {name}: {n_calls} decompressor calls', file=sys.stderr)

    overlays, covered = [], set()
    for (src, dst), seen in sorted(loads.items()):
        inside = {a: st for (s_, d_, a), st in entries.items() if (s_, d_) == (src, dst)}
        if not inside:
            continue   # graphics or other data
        covered |= set(inside)
        overlays.append((src, dst, ct_decompress.decompress(rom, src), inside, seen))
    for (s_, d_, a), st in sorted(entries.items()):
        if s_ == 0:
            print(f'# WRAM entry ${a:06X} ({",".join(sorted(st))}) ran outside any decompressed '
                  'blob', file=sys.stderr)
    text = ['# Code the game decompresses into WRAM, recompiled as overlays (#92).',
            '# Written by game/ct/tools/find_overlays.py: pointers, sizes and hashes only.', '']
    for src, dst, data, inside, seen in overlays:
        text += ['[[overlay]]', f'name = "ovl_{src:06X}_{dst:06X}"', f'src = 0x{src:06X}',
                 f'dst = 0x{dst:06X}', f'size = 0x{len(data):04X}',
                 f'fnv64 = "{ct_decompress.fnv64(data):016x}"',
                 f'seen = [{", ".join(chr(34) + s + chr(34) for s in sorted(seen))}]',
                 'source = "profile"']
        for a in sorted(inside):
            states = ', '.join(f'"{s}"' for s in sorted(inside[a]))
            text += ['[[overlay.entry]]', f'addr = 0x{a:06X}', f'states = [{states}]']
        text.append('')
    body = '\n'.join(text)
    if '--write' in argv:
        with open(os.path.join(GAME, 'overlays.toml'), 'w') as f:
            f.write(body)
    else:
        print(body)
    print(f'# {len(overlays)} code blobs, {len(covered)} of {len(wram)} WRAM entries covered',
          file=sys.stderr)
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
