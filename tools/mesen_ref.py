#!/usr/bin/env python3
"""Reference run in Mesen 2 (headless, as a tool: nothing of it is copied).

usage: mesen_ref.py --mesen PATH --rom ROM --frames N --out LOG
                    [--script FILE] [--setting Name.Path=value ...]
                    [--wram-at F --wram-out FILE]

Runs ROM from power-on in Mesen's --testrunner mode with a generated Lua
script that drives pad 1 from an input script (runtime/replay.h format)
and, at the start of every frame (scanline 0, the same point as the frame
scheduler's frame edge), appends "frame wram_hash frame_hash" to LOG (the
picture complete at the end of the frame before, as the probe's), the
same hashes as `ct_boot --ref-log` (tools/ref_compare.py compares them):

  wram_hash   FNV-1a 64 over the 128 KB of WRAM
  frame_hash  FNV-1a 64 over the 224 visible rows of the last frame, each
              pixel as a little-endian 15-bit BGR value (8-bit channels >> 3);
              Mesen's screen buffer is 239 lines, the visible 224 at lines
              7-230 (no overscan)

"frame" is Mesen's frame count at that point: frames completed.
--wram-at F also writes WRAM at that point of frame F to --wram-out (raw
128 KB, as `ct_boot --frames F --wram FILE` does).

Mesen runs on a virtual display (xvfb-run; required: --testrunner still
opens its window on Linux) with a private home (XDG_CONFIG_HOME in a
temporary folder, seeded with an empty settings.json so no first-run
window waits for input), so its settings and save files never touch the
user's own, and with WRAM
powered on as zeros and the DSP at 32040 Hz (SpcClockSpeedAdjustment=40,
Mesen's default, set explicitly), as the frame scheduler has them, and with
frame skipping off (at unlimited speed Mesen otherwise skips rendering
frames, depending on wall-clock time).
--setting passes more Mesen settings. The Linux build carries a static
libstdc++ that crashes in std::regex next to the system one; the system
libstdc++ is preloaded to avoid that.
"""
import argparse
import os
import shutil
import signal
import subprocess
import sys
import tempfile

NAMES = {"b": 0x8000, "y": 0x4000, "select": 0x2000, "start": 0x1000, "up": 0x0800,
         "down": 0x0400, "left": 0x0200, "right": 0x0100, "a": 0x0080, "x": 0x0040,
         "l": 0x0020, "r": 0x0010}


def load_script(path):
    """Spans (from, to, mask) of an input script, as replay_load reads it."""
    spans = []
    for n, line in enumerate(open(path), 1):
        tok = line.split("#")[0].strip()
        if not tok:
            continue
        rng, buttons = tok.split(":")
        lo, hi = (int(v) for v in rng.split("-"))
        if buttons.startswith("0x"):
            mask = int(buttons, 16)
        else:
            mask = 0
            for b in buttons.split("+"):
                if b not in NAMES:
                    raise SystemExit(f"mesen_ref: {path}:{n}: bad button {b!r}")
                mask |= NAMES[b]
        spans.append((lo, hi, mask))
    return spans


def lua_str(path):
    return path.replace("\\", "\\\\").replace('"', '\\"')


LUA = r"""
local spans = { %(spans)s }
local frames = %(frames)d
local wram_at, wram_out = %(wram_at)d, "%(wram_out)s"
local out = io.open("%(out)s", "w")
local names = { {"b",0x8000},{"y",0x4000},{"select",0x2000},{"start",0x1000},
  {"up",0x0800},{"down",0x0400},{"left",0x0200},{"right",0x0100},
  {"a",0x0080},{"x",0x0040},{"l",0x0020},{"r",0x0010} }

local function buttons(f)
  local m = 0
  for _, s in ipairs(spans) do
    if f >= s[1] and f <= s[2] then m = m | s[3] end
  end
  return m
end

local PRIME, BASIS = 0x100000001B3, 0xCBF29CE484222325   -- hex literals wrap to 64 bits

local function wram_hash()
  local h = BASIS
  local wram = emu.memType.snesWorkRam
  for a = 0, 0x1FFFC, 4 do
    local v = emu.read32(a, wram, false)
    for k = 0, 3 do
      h = (h ~ ((v >> (8 * k)) & 0xFF)) * PRIME
    end
  end
  return h
end

-- Mesen's buffer is 239 lines: without overscan, visible row r is line r + 7.
local function frame_hash()
  local h = BASIS
  local buf = emu.getScreenBuffer()
  for i = 7 * 256 + 1, 231 * 256 do
    local p = buf[i]
    local c = ((p >> 19) & 0x1F) | (((p >> 11) & 0x1F) << 5) | (((p >> 3) & 0x1F) << 10)
    h = (h ~ (c & 0xFF)) * PRIME
    h = (h ~ (c >> 8)) * PRIME
  end
  return h
end

-- Buttons for the frame in progress (frames completed + 1), set whenever
-- the game polls the pad.
emu.addEventCallback(function()
  local m = buttons(emu.getState()["frameCount"] + 1)
  local t = {}
  for _, n in ipairs(names) do t[n[1]] = (m & n[2]) ~= 0 end
  emu.setInput(t, 0, 0)
end, emu.eventType.inputPolled)

-- Mesen swaps its screen buffers at line 0, so the picture the probe logs at
-- a frame edge (the one just shown) is the one complete at the end of the
-- frame before (VBlank): hash it there, log it with the next frame start.
local picture = 0
emu.addEventCallback(function()
  picture = frame_hash()
end, emu.eventType.endFrame)

emu.addEventCallback(function()
  local f = emu.getState()["frameCount"]
  if f == wram_at then
    local w = io.open(wram_out, "wb")
    for a = 0, 0x1FFFF do w:write(string.char(emu.read(a, emu.memType.snesWorkRam, false))) end
    w:close()
  end
  if f >= 1 then
    out:write(string.format("%%d %%016x %%016x\n", f, wram_hash(), picture))
  end
  if f >= frames then
    out:close()
    emu.stop(0)
  end
end, emu.eventType.startFrame)
"""


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--mesen", required=True)
    ap.add_argument("--rom", required=True)
    ap.add_argument("--frames", type=int, required=True)
    ap.add_argument("--out", required=True)
    ap.add_argument("--script")
    ap.add_argument("--setting", action="append", default=[])
    ap.add_argument("--timeout", type=int, default=3600)
    ap.add_argument("--wram-at", type=int, default=-1)
    ap.add_argument("--wram-out", default="")
    a = ap.parse_args()
    spans = load_script(a.script) if a.script else []
    xvfb = shutil.which("xvfb-run")
    if not xvfb:
        print("mesen_ref: xvfb-run not found (Mesen would open windows on the desktop)")
        return 2
    home = tempfile.mkdtemp(prefix="mesen_ref_")
    os.makedirs(os.path.join(home, "Mesen2"))
    with open(os.path.join(home, "Mesen2", "settings.json"), "w") as f:
        f.write("{}\n")
    try:
        lua = os.path.join(home, "ref.lua")
        with open(lua, "w") as f:
            f.write(LUA % {
                "spans": ", ".join(f"{{{lo}, {hi}, {m}}}" for lo, hi, m in spans),
                "frames": a.frames,
                "out": lua_str(os.path.abspath(a.out)),
                "wram_at": a.wram_at,
                "wram_out": lua_str(os.path.abspath(a.wram_out)) if a.wram_out else "",
            })
        settings = ["Debug.ScriptWindow.AllowIoOsAccess=true", "Snes.RamPowerOnState=AllZeros",
                    "Snes.SpcClockSpeedAdjustment=40", "Snes.DisableFrameSkipping=true", *a.setting]
        env = dict(os.environ, XDG_CONFIG_HOME=home)
        env.pop("WAYLAND_DISPLAY", None)   # X11, inside Xvfb
        stdcxx = "/usr/lib/libstdc++.so.6"
        if os.path.exists(stdcxx):
            env["LD_PRELOAD"] = stdcxx
        cmd = [xvfb, "-a", a.mesen, "--testrunner", *("--" + s for s in settings),
               os.path.abspath(a.rom), lua]
        proc = subprocess.Popen(cmd, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                text=True, start_new_session=True)
        try:
            out, _ = proc.communicate(timeout=a.timeout)
        except subprocess.TimeoutExpired:
            os.killpg(proc.pid, signal.SIGKILL)   # Xvfb and Mesen with it
            proc.communicate()
            print(f"mesen_ref: Mesen timed out after {a.timeout} s")
            return 1
        if proc.returncode:
            print(f"mesen_ref: Mesen exit {proc.returncode}: {out[-400:]}")
            return 1
        with open(a.out) as f:
            n = sum(1 for _ in f)
        if n < a.frames:
            print(f"mesen_ref: only {n} of {a.frames} frames logged")
            return 1
        print(f"mesen_ref: {n} frames")
        return 0
    finally:
        shutil.rmtree(home, ignore_errors=True)


if __name__ == "__main__":
    sys.exit(main())
