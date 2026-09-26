#!/usr/bin/env python3
"""Write the synthetic ROM the engine tests run on (no game data).

4 MB HiROM image: a byte pattern, a header title, and just the stubs the
tests need:
  $00:8000 / $C0:8000  reset stub: SEI CLC XCE JML $C08100
  $8010  NMI stub: JML $000500      $8014  IRQ stub: JML $000504
  vectors: emulation RESET $FFFC -> $8000, native NMI $FFEA -> $8010,
           native IRQ $FFEE -> $8014
Generated at build time; never committed.

usage: make_test_rom.py OUT
"""
import sys

SIZE = 0x400000
TITLE = b'CT-RECOMP TEST ROM'   # test/harness.h TH_TITLE


def image() -> bytes:
    img = bytearray(((k * 7 + (k >> 16)) & 0xFF) for k in range(SIZE))
    img[0xFFC0:0xFFC0 + len(TITLE)] = TITLE
    img[0x8000:0x8007] = bytes([0x78, 0x18, 0xFB, 0x5C, 0x00, 0x81, 0xC0])
    img[0x8010:0x8014] = bytes([0x5C, 0x00, 0x05, 0x00])
    img[0x8014:0x8018] = bytes([0x5C, 0x04, 0x05, 0x00])
    img[0xFFFC:0xFFFE] = bytes([0x00, 0x80])
    img[0xFFEA:0xFFEC] = bytes([0x10, 0x80])
    img[0xFFEE:0xFFF0] = bytes([0x14, 0x80])
    return bytes(img)


def main() -> int:
    if len(sys.argv) != 2:
        print(__doc__.strip().splitlines()[-1], file=sys.stderr)
        return 2
    with open(sys.argv[1], 'wb') as f:
        f.write(image())
    return 0


if __name__ == '__main__':
    sys.exit(main())
