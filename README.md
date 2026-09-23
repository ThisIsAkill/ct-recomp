# ct-recomp

Static recompiler for Chrono Trigger (SNES, US 1.0) 65816 code to C, plus a C runtime.

## Build

```
export CT_ROM=/path/to/chrono_trigger.sfc   # headerless, 4 MB
tools/install_hooks.sh
cmake -S . -B build && cmake --build build -j && ctest --test-dir build
```

The ROM is never committed. Generated C goes to `out/`.
