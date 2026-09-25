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

## Planned: `third_party/snes/`

Not vendored yet (tracked as a Title Screen milestone issue). When it lands:

- **snesrev/zelda3**, `snes/` subtree (MIT, snesrev + elzo_d/LakeSnes) — PPU,
  DMA/HDMA, APU, SPC700, and DSP emulation, adapted via a thin runtime
  interface. License headers are kept intact in every vendored file, and any
  local patches are listed here with a short rationale.

## Auditing

`git ls-files` plus a size scan of this repo are run periodically to confirm
no ROM data or ROM-derived binary assets are tracked. See the setup report
for the most recent audit.
