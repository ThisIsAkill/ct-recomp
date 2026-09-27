# Vendored SPC700 core (ares)

Source: https://github.com/ares-emulator/ares
Commit: `4cb8d92b441557cb6bcaf133c4cbc7f6819b1122` (2026-09-23)
Path: `ares/component/processor/spc700/` in that repo.
License: ISC (`LICENSE`, the ares repository's license file, copied whole).

Vendored verbatim: `spc700.hpp`, `spc700.cpp`, `memory.cpp`,
`algorithms.cpp`, `instruction.cpp`, `instructions.cpp`,
`serialization.cpp`, `disassembler.cpp`. No file here is modified.

The core is cycle-accurate: every instruction is its sequence of
`read`/`write`/`idle` calls, one SPC700 cycle each, supplied by the
host. ct-recomp's host is `runtime/spc700_host.cpp`: it provides the few
`nall` types the core uses (`runtime/nall_shim.hpp`), compiles
`spc700.hpp` and the instruction files the way `spc700.cpp` does but
without the disassembler and serializer (which need the rest of nall),
steps the vendored APU's DSP and timers (`third_party/snes`) every cycle,
and runs the core as a coroutine that stops at the CPU's time, mid-
instruction if need be.
