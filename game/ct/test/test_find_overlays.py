#!/usr/bin/env python3
"""find_overlays.py's entry attribution and batching on synthetic log
lines (no game ROM, no ct_boot): an entry is charged to the latest load
covering its address before it, a load whose blob fails to decompress
(size 0) covers nothing, entries outside every load are reported
uncovered, and the WRAM-entry list is split into batches ct_boot can
log (MAX_LOG is 64, one slot being the decompressor itself)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', 'tools'))
import find_overlays  # noqa: E402

DECOMP = find_overlays.DECOMPRESS
SRC_A, SRC_B, SRC_BAD = 0xC31313, 0xC40000, 0xC50000
DST = 0x7E3000
SIZES = {SRC_A: 0x100, SRC_B: 0x80, SRC_BAD: 0}


def line(at: int, m: int = 1, x: int = 0, dp: bytes = bytes(16)) -> str:
    dps = ''.join(f' {b:02X}' for b in dp)
    return f'entry {at:06X} frame 1 line 2 clock 12345 run m {m} x {x} dp{dps}'


def load(src: int, dst: int) -> str:
    dp = bytes([src & 0xFF, (src >> 8) & 0xFF, (src >> 16) & 0xFF,
                dst & 0xFF, (dst >> 8) & 0xFF,
                1 if (dst >> 16) == 0x7F else 0]) + bytes(10)
    return line(DECOMP, dp=dp)


def main() -> int:
    lines = [
        'not an entry line',
        line(0x7E3010),                       # before any load: uncovered
        load(SRC_A, DST),                     # covers $7E3000-$7E30FF
        line(0x7E30FF),                       # last covered byte of A
        line(0x7E3100),                       # one past A: uncovered
        load(SRC_B, DST),                     # covers $7E3000-$7E307F, later
        line(0x7E307F),                       # charged to B (latest covering)
        line(0x7E3090, m=0),                  # only A still covers: charged to A
        line(0x7E3090),                       # same entry, second state
        load(SRC_BAD, 0x7E4000),              # size 0: covers nothing
        line(0x7E4000),                       # uncovered
    ]
    loads, entries, n_calls = find_overlays.attribute_run(lines, SIZES.__getitem__)
    batches = [len(b) for b in find_overlays.batched(list(range(130)),
                                                     find_overlays.MAX_LOG_ENTRIES)]
    checks = [
        ('decompressor calls', n_calls, 3),
        ('loads in call order', loads, [(SRC_A, DST), (SRC_B, DST), (SRC_BAD, 0x7E4000)]),
        ('entry before any load uncovered', entries.get((0, 0, 0x7E3010)), {'m1x0'}),
        ('last byte of A charged to A', entries.get((SRC_A, DST, 0x7E30FF)), {'m1x0'}),
        ('one past A uncovered', entries.get((0, 0, 0x7E3100)), {'m1x0'}),
        ('latest covering load wins', entries.get((SRC_B, DST, 0x7E307F)), {'m1x0'}),
        ('older load still covers past B', entries.get((SRC_A, DST, 0x7E3090)),
         {'m0x0', 'm1x0'}),
        ('failed decompress covers nothing', entries.get((0, 0, 0x7E4000)), {'m1x0'}),
        ('no other entries', len(entries), 6),
        ('one batch holds 63 entries', find_overlays.MAX_LOG_ENTRIES, 63),
        ('130 entries split 63/63/4', batches, [63, 63, 4]),
        ('no entries, no batches', list(find_overlays.batched([], 63)), []),
    ]
    fails = 0
    for what, got, want in checks:
        if got != want:
            print(f'FAIL {what}: got {got}, want {want}')
            fails += 1
    print('test_find_overlays: ' + ('ok' if not fails else f'{fails} failed'))
    return 1 if fails else 0


if __name__ == '__main__':
    sys.exit(main())
