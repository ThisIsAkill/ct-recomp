#!/usr/bin/env python3
"""ct_boot --sram-in/--sram-out (#109) on a synthetic ROM (no game data):
the ROM copies SRAM[0..15] to WRAM $1000 and stamps SRAM[$10] = $A5, so a
seeded save must show up in --wram, the stamp in --sram-out, an unseeded
run must start from zeros (power-on clears SRAM), and a missing or
wrong-sized save is a usage error (exit 2).

usage: test_sram_boot.py PATH_TO_CT_BOOT
"""
import os
import subprocess
import sys
import tempfile

SRAM_SIZE = 0x2000


def make_rom(path):
    img = bytearray([0xEA]) * 0x400000
    code = bytes([
        0x78,                    # SEI
        0x18, 0xFB,              # CLC XCE
        0xC2, 0x10,              # REP #$10 (X/Y 16-bit, A stays 8-bit)
        0xA2, 0x00, 0x00,        # LDX #$0000
        0xBF, 0x00, 0x60, 0x30,  # loop: LDA $306000,X  (SRAM[X])
        0x9F, 0x00, 0x10, 0x7E,  # STA $7E1000,X
        0xE8,                    # INX
        0xE0, 0x10, 0x00,        # CPX #$0010
        0xD0, 0xF2,              # BNE loop
        0xA9, 0xA5,              # LDA #$A5
        0x8F, 0x10, 0x60, 0x30,  # STA $306010 (SRAM[$10])
        0x80, 0xFE,              # BRA .
    ])
    img[0x8000:0x8000 + len(code)] = code
    img[0x8030] = 0x40           # RTI stub
    for vec in (0xFFEA, 0xFFEE, 0xFFFA, 0xFFFE):
        img[vec:vec + 2] = bytes([0x30, 0x80])
    img[0xFFFC:0xFFFE] = bytes([0x00, 0x80])   # RESET -> $8000
    with open(path, "wb") as f:
        f.write(img)


def main() -> int:
    if len(sys.argv) != 2:
        print(__doc__.strip().splitlines()[-1], file=sys.stderr)
        return 2
    probe = sys.argv[1]
    fails = 0

    def check(what, got, want):
        nonlocal fails
        if got != want:
            print(f"FAIL {what}: got {got!r}, want {want!r}")
            fails += 1

    with tempfile.TemporaryDirectory() as tmp:
        p = lambda n: os.path.join(tmp, n)
        make_rom(p("rom.sfc"))
        env = dict(os.environ, CT_ROM=p("rom.sfc"))

        def run(*args):
            return subprocess.run([probe, "--frames", "5", "--interp-only", *args],
                                  env=env, capture_output=True, text=True)

        seed = bytes(range(0x10, 0x20)) + bytes(SRAM_SIZE - 16)
        with open(p("seed.srm"), "wb") as f:
            f.write(seed)
        r = run("--sram-in", p("seed.srm"), "--sram-out", p("out.srm"),
                "--wram", p("out.wram"))
        check("seeded run exits 0", r.returncode, 0)
        if r.returncode == 0:
            wram = open(p("out.wram"), "rb").read()
            check("seed copied to WRAM", wram[0x1000:0x1010], seed[:16])
            out = open(p("out.srm"), "rb").read()
            check("sram-out size", len(out), SRAM_SIZE)
            check("sram-out keeps the seed", out[:16], seed[:16])
            check("sram-out has the stamp", out[0x10], 0xA5)
            check("sram-out rest is zero", out[0x11:], bytes(SRAM_SIZE - 0x11))

        r = run("--sram-out", p("plain.srm"), "--wram", p("plain.wram"))
        check("unseeded run exits 0", r.returncode, 0)
        if r.returncode == 0:
            wram = open(p("plain.wram"), "rb").read()
            check("power-on SRAM is zero", wram[0x1000:0x1010], bytes(16))
            out = open(p("plain.srm"), "rb").read()
            check("unseeded stamp only", (out[:0x10], out[0x10], out[0x11:]),
                  (bytes(0x10), 0xA5, bytes(SRAM_SIZE - 0x11)))

        with open(p("short.srm"), "wb") as f:
            f.write(bytes(100))
        check("short sram-in exits 2", run("--sram-in", p("short.srm")).returncode, 2)
        check("missing sram-in exits 2", run("--sram-in", p("nope.srm")).returncode, 2)
    print("test_sram_boot: " + ("ok" if not fails else f"{fails} failed"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
