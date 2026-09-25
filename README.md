# ct-recomp

Static recompiler for Chrono Trigger (SNES, US 1.0) 65816 code to C, plus a C runtime.

## Build

```
export CT_ROM=/path/to/chrono_trigger.sfc   # headerless, 4 MB
tools/install_hooks.sh
cmake -S . -B build && cmake --build build -j && ctest --test-dir build
```

The ROM is never committed. Generated C goes to `out/`.

Symbol names partly derived from dscotton/ct_disassembly (public domain) and ChronoRET.

## License

This code is MIT-licensed (see `LICENSE`). The Chrono Trigger ROM and its
assets are **not included** and remain the property of their copyright
holders — you must supply your own legally obtained ROM via `$CT_ROM`.
Symbol names are credited to dscotton/ct_disassembly (public domain) and
ChronoRET. See `THIRD_PARTY.md` for vendored/adapted third-party code.
