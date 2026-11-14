#!/usr/bin/env python3
"""SRAM plumbing of the reference tools (#109), without Mesen or a ROM:
mesen_ref's save hash (FNV-1a 64, known vectors), its .srm validation
(exactly 8 KB or refused) and where the save goes in Mesen's private home;
ref_compare refusing a bad --sram-in before running anything (exit 2)."""
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.join(HERE, "..", "tools")
sys.path.insert(0, TOOLS)
import mesen_ref  # noqa: E402


def main() -> int:
    fails = 0

    def check(what, got, want):
        nonlocal fails
        if got != want:
            print(f"FAIL {what}: got {got!r}, want {want!r}")
            fails += 1

    check("fnv empty", mesen_ref.fnv1a64(b""), 0xCBF29CE484222325)
    check("fnv 'a'", mesen_ref.fnv1a64(b"a"), 0xAF63DC4C8601EC8C)
    check("fnv 'foobar'", mesen_ref.fnv1a64(b"foobar"), 0x85944171F73967E8)

    with tempfile.TemporaryDirectory() as tmp:
        p = lambda n: os.path.join(tmp, n)
        good = bytes((k * 31 + 7) & 0xFF for k in range(0x2000))
        with open(p("good.srm"), "wb") as f:
            f.write(good)
        check("read_sram accepts 8 KB", mesen_ref.read_sram(p("good.srm")), good)
        for name, data in (("short.srm", bytes(0x1FFF)), ("long.srm", bytes(0x2001))):
            with open(p(name), "wb") as f:
                f.write(data)
            try:
                mesen_ref.read_sram(p(name))
                check(f"read_sram refuses {name}", "accepted", "SystemExit")
            except SystemExit:
                pass
        try:
            mesen_ref.read_sram(p("nope.srm"))
            check("read_sram refuses missing", "accepted", "SystemExit")
        except SystemExit:
            pass
        check("srm path in the private home",
              mesen_ref.srm_path(tmp, p("Some Game (USA).sfc")),
              os.path.join(tmp, "Mesen2", "Saves", "Some Game (USA).srm"))

        ref_compare = os.path.join(TOOLS, "ref_compare.py")
        for bad in (p("short.srm"), p("nope.srm")):
            r = subprocess.run([sys.executable, ref_compare, "--probe", "x",
                                "--mesen", "y", "--rom", "z", "--frames", "1",
                                "--sram-in", bad], capture_output=True, text=True)
            check(f"ref_compare refuses {os.path.basename(bad)}", r.returncode, 2)
    print("test_sram_tools: " + ("ok" if not fails else f"{fails} failed"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
