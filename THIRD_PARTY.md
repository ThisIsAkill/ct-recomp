# Third-party code and data

ct-recomp is MIT-licensed (`Copyright (c) 2026 Akhil Moola`, see `LICENSE`).
This file tracks third-party sources used by the project and their terms.

## Currently in use

### Symbol names and entry-state hints

- **dscotton/ct_disassembly** — public domain. Function labels, boundaries,
  and comments are usable directly. Where they conflict with ChronoRET,
  ChronoRET wins.
- **ChronoRET** (this project's own matching disassembly, `../ChronoRET`) —
  source of truth for names and verified entry states.

No files from either source are vendored verbatim; `tools/sync_symbols.py`
reads them to populate `funcs.toml` / `unresolved.toml`.

### Hints only, not copied

- **DocDamage/chronotrigger_disassembly** (MIT) — used only as a hint when
  labeling routines. Anything actually copied from it must be verified
  against our own decoder and keep its MIT notice here.

### Debugging tools, never copied from

- **bsnes**, **Mesen2** (GPL), **snes9x** (non-free) — used only as external
  debuggers/tracers during development. No code from any of them is in this
  repository.

### `third_party/snes/`

- **snesrev/zelda3**, `snes/` subtree, commit `fbbb3f967a51fafe642e6140d0753979e73b4090`
  (MIT, snesrev + elzo_d) — PPU, DMA/HDMA, APU, SPC700, and DSP emulation.
  Full license text and file list in `third_party/snes/README.md`.
  `runtime/snes_adapter.c` bridges it into `bus.c`; `dma.c`/`dma.h` are
  otherwise unmodified. Local patches to `ppu.c`/`ppu.h` (all marked
  `ct-recomp:` in place):
  - `OBSEL` ($2101): upstream asserted one fixed value and never actually
    decoded it (`objSize`/`objTileAdr1/2` were hardcoded in `ppu_reset`).
    Implemented the real decode.
  - `BGMODE` ($2105) bit 3 (BG3 priority): upstream hardcoded "always on
    for mode 1" instead of reading the bit. Added `Ppu.bg3Priority` and
    read it for real.
  - Asserts on `BGMODE`, `M7SEL`, `VMAIN`, `WBGLOG`/`WOBJLOG`, `CGWSEL`,
    `SETINI`, `OAMADDH` restricted every value to what A Link to the Past
    happens to use. Relaxed so Chrono Trigger's actual register writes
    don't abort; several of the underlying features (window AND/XOR/XNOR
    logic, VRAM address remapping, direct color mode, interlace/hi-res/
    overscan) are still genuinely unimplemented, not just untested --
    see issue #9's gap report for the full list.

## Auditing

`git ls-files` plus a size scan of this repo are run periodically to confirm
no ROM data or ROM-derived binary assets are tracked. See the setup report
for the most recent audit.
