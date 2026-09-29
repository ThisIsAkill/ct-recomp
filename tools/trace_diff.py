#!/usr/bin/env python3
"""Instruction-trace diff of the probe against Mesen 2 over a window of frames.

usage: trace_diff.py --probe PATH --mesen PATH --rom ROM --from F --to G
                     [--script FILE] [--context N]

Both run the same input (runtime/replay.h script, resets included) from
power-on, untraced up to the window, then log every instruction that starts
in frames F..G: "ADDR clock X Y" (the probe: ct_boot --trace; Mesen: an exec
callback armed at the start of frame F and dropped after G). Reports the
first instruction where address, clock, X or Y differ, with N lines of
context (default 8), or that the window matches.

Frames are counted as both count them at a frame edge (line 0): the probe's
frame F starts where Mesen's frameCount becomes F at its start-of-frame
event. Mesen's Lua reports the master clock modulo 2^32 (it wraps near frame
12018), so clocks are compared modulo 2^32.

Running to the window untraced takes about a minute for 12,000 frames;
tracing costs only for the window, so keep it to a few frames.

Exit 0 if the window matches, 1 if it differs, 2 on bad usage or a run
that fails.
"""
import argparse
import os
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import mesen_ref  # noqa: E402

LUA = r"""
local changes = { %(changes)s }
local resets = { %(resets)s }
local first, last = %(first)d, %(last)d
local names = { {"b",0x8000},{"y",0x4000},{"select",0x2000},{"start",0x1000},
  {"up",0x0800},{"down",0x0400},{"left",0x0200},{"right",0x0100},
  {"a",0x0080},{"x",0x0040},{"l",0x0020},{"r",0x0010} }
local ci = 0
local function buttons(f)
  while ci < #changes and changes[ci + 1][1] <= f do ci = ci + 1 end
  return ci > 0 and changes[ci][2] or 0
end
emu.addEventCallback(function()
  local m = buttons(emu.getState()["frameCount"])
  local t = {}
  for _, n in ipairs(names) do t[n[1]] = (m & n[2]) ~= 0 end
  emu.setInput(t, 0, 0)
end, emu.eventType.inputPolled)

local out = io.open("%(out)s", "w")
local ref = nil
local function exec(a, v)
  local s = emu.getState()
  out:write(string.format("%%06X %%d %%04X %%04X\n", a, s["masterClock"], s["cpu.x"], s["cpu.y"]))
end
emu.addEventCallback(function()
  local f = emu.getState()["frameCount"]
  if f > last then
    out:close()
    emu.stop(0)
    return
  end
  if f >= first and not ref then
    ref = emu.addMemoryCallback(exec, emu.callbackType.exec, 0x000000, 0xFFFFFF)
  end
  if resets[f + 1] then emu.reset() end
end, emu.eventType.startFrame)
"""


def mesen_trace(a, spans, resets, out_path) -> int:
    home = tempfile.mkdtemp(prefix="trace_diff_")
    try:
        os.makedirs(os.path.join(home, "Mesen2"))
        with open(os.path.join(home, "Mesen2", "settings.json"), "w") as f:
            f.write("{}\n")
        lua = os.path.join(home, "trace.lua")
        with open(lua, "w") as f:
            f.write(LUA % {
                "changes": ", ".join(f"{{{fr}, {m}}}" for fr, m in mesen_ref.changes(spans)),
                "resets": ", ".join(f"[{fr}] = true" for fr in resets),
                "first": a.first, "last": a.to,
                "out": mesen_ref.lua_str(os.path.abspath(out_path)),
            })
        env = dict(os.environ, XDG_CONFIG_HOME=home)
        env.pop("WAYLAND_DISPLAY", None)
        if os.path.exists("/usr/lib/libstdc++.so.6"):
            env["LD_PRELOAD"] = "/usr/lib/libstdc++.so.6"
        cmd = [shutil.which("xvfb-run"), "-a", a.mesen, "--testrunner", "--timeout=3600",
               "--Debug.ScriptWindow.AllowIoOsAccess=true", "--Snes.RamPowerOnState=AllZeros",
               "--Snes.SpcClockSpeedAdjustment=40", "--Snes.DisableFrameSkipping=true",
               "--Snes.Port1.Type=SnesController", os.path.abspath(a.rom), lua]
        return subprocess.run(cmd, env=env, capture_output=True, text=True).returncode
    finally:
        shutil.rmtree(home, ignore_errors=True)


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--probe", required=True)
    ap.add_argument("--mesen", required=True)
    ap.add_argument("--rom", required=True)
    ap.add_argument("--from", dest="first", type=int, required=True)
    ap.add_argument("--to", type=int, required=True)
    ap.add_argument("--script")
    ap.add_argument("--context", type=int, default=8)
    a = ap.parse_args()
    if a.first < 1 or a.to < a.first:
        print("trace_diff: need 1 <= --from <= --to")
        return 2
    spans = mesen_ref.load_script(a.script) if a.script else []
    resets = mesen_ref.load_resets(a.script) if a.script else []
    with tempfile.TemporaryDirectory() as tmp:
        ours_path, ref_path = os.path.join(tmp, "ours.txt"), os.path.join(tmp, "ref.txt")
        env = dict(os.environ, CT_ROM=a.rom)
        script = ["--script", a.script] if a.script else []
        ours = subprocess.Popen([a.probe, "--frames", str(a.to + 1), "--overlay-strict", *script,
                                 *[x for q in mesen_ref_quirks() for x in ("--ref-quirk", q)],
                                 "--trace", f"0:{2**63}@{a.first}", ours_path], env=env,
                                stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        code = mesen_trace(a, spans, resets, ref_path)
        ours.wait()
        if code or not os.path.exists(ref_path):
            print(f"trace_diff: Mesen run failed (exit {code})")
            return 2
        o = [ln.split() for ln in open(ours_path)]
        o = [[x[0], str(int(x[1]) % 2**32), x[2], x[3]] for x in o]
        r = [ln.split()[:4] for ln in open(ref_path)]
    n = min(len(o), len(r))
    for i in range(n):
        if o[i] != r[i]:
            print(f"trace_diff: frames {a.first}-{a.to}: first difference at instruction {i}:")
            for j in range(max(0, i - a.context), min(n, i + 4)):
                mark = "->" if j == i else "  "
                print(f"  {mark} ours {' '.join(o[j])}   mesen {' '.join(r[j])}")
            return 1
    print(f"trace_diff: frames {a.first}-{a.to}: {n} instructions identical "
          f"(probe logged {len(o)}, Mesen {len(r)})")
    return 0


def mesen_ref_quirks():
    """The --ref-quirk names ref_compare.py reproduces (its KNOWN_DIFFERENCES)."""
    import ref_compare
    return [name for name, _, _ in ref_compare.KNOWN_DIFFERENCES]


if __name__ == "__main__":
    sys.exit(main())
