# ct-recomp

[![ci](https://github.com/ThisIsAkill/ct-recomp/actions/workflows/ci.yml/badge.svg)](https://github.com/ThisIsAkill/ct-recomp/actions/workflows/ci.yml)

Static recompiler for Chrono Trigger (SNES, US 1.0) 65816 code to C, plus a C runtime.
The recompiler and runtime are game-agnostic; everything specific to Chrono
Trigger lives in `game/ct/`.

## Build

Engine only (no ROM needed; what CI runs):

```
cmake -S . -B build && cmake --build build -j && ctest --test-dir build -LE rom
```

With the game (builds `game/ct`, `ct_boot`, `ct_sdl`, and the ROM tests):

```
export CT_ROM=/path/to/chrono_trigger.sfc   # headerless, 4 MB
tools/install_hooks.sh
cmake -S . -B build && cmake --build build -j && ctest --test-dir build
build/ct_sdl
```

The ROM is never committed. Generated C goes to `build/game/ct/out/`.

Symbol names partly derived from dscotton/ct_disassembly (public domain) and ChronoRET.

## License

This code is MIT-licensed (see `LICENSE`). The Chrono Trigger ROM and its
assets are **not included** and remain the property of their copyright
holders — you must supply your own legally obtained ROM via `$CT_ROM`.
Symbol names are credited to dscotton/ct_disassembly (public domain) and
ChronoRET. See `THIRD_PARTY.md` for vendored/adapted third-party code.
