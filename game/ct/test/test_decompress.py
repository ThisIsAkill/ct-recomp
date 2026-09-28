#!/usr/bin/env python3
"""tools/ct_decompress.py against the game's own decompressor: every call
the boot makes ($C30557, arguments in direct page $0300), decompressed in
Python, must equal what the ROM routine left in WRAM when it returned (the
RTL at $C308B2, snapshotted by ct_boot --snap-at; arguments read at
$0300, --log-entry C30557:0300), byte for byte, and the
size must equal the one it reports in $0306.

usage: test_decompress.py BUILD_DIR
"""
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

ENTRY_RE = re.compile(r'^entry C30557 .* dp((?: [0-9A-F-]{2}){16})$')


def main() -> int:
    probe = os.path.join(sys.argv[1], 'ct_boot')
    rom = decode.load_rom()
    fails = 0
    with tempfile.TemporaryDirectory() as snaps:
        r = subprocess.run([probe, '--frames', '1500', '--log-entry', 'C30557:0300', '--snap-at',
                            'C308B2', snaps], capture_output=True, text=True)
        calls = []
        for line in r.stdout.splitlines():
            m = ENTRY_RE.match(line)
            if m:
                dp = [int(b, 16) for b in m.group(1).split()]
                calls.append((dp[0] | dp[1] << 8 | dp[2] << 16,
                              (0x10000 if dp[5] & 1 else 0) | dp[3] | dp[4] << 8))
        files = sorted(os.listdir(snaps))
        if not calls or len(files) != len(calls):
            print(f'FAIL: {len(calls)} calls, {len(files)} snapshots: {r.stdout[-300:]}')
            return 1
        for (src, off), name in zip(calls, files):
            wram = open(os.path.join(snaps, name), 'rb').read()
            try:
                data = ct_decompress.decompress(rom, src)
            except ct_decompress.DecompressError as ex:
                print(f'FAIL ${src:06X}: {ex}')
                fails += 1
                continue
            got = wram[off:off + len(data)]
            size = wram[0x306] | wram[0x307] << 8
            if got != data or size != len(data):
                at = next((k for k in range(len(data)) if k >= len(got) or got[k] != data[k]), -1)
                print(f'FAIL ${src:06X} -> ${0x7E0000 + off:06X}: {len(data)} bytes (game: {size}),'
                      f' first difference at +{at:#x}')
                fails += 1
    print(f'decompress: {len(calls)} calls, {"ok" if not fails else f"{fails} failed"}')
    return 1 if fails else 0


if __name__ == '__main__':
    sys.exit(main())
