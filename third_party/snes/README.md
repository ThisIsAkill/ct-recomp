# Vendored SNES core (zelda3)

Source: https://github.com/snesrev/zelda3
Commit: `fbbb3f967a51fafe642e6140d0753979e73b4090` (2023-08-17)
Path: `snes/` in that repo.

Vendored verbatim: `ppu.{c,h}`, `dma.{c,h}`, `apu.{c,h}`, `spc.{c,h}`,
`dsp.{c,h}`, `dsp_regs.h`, `snes_regs.h`, `saveload.h`.

Not vendored: `cpu.{c,h}`, `cart.{c,h}`, `input.{c,h}`, `snes_other.c`,
`snes.{c,h}`, `tracing.{c,h}` -- ct-recomp has its own 65816 core and its
own WRAM/ROM/SRAM bus (`runtime/bus.c`), so those would either duplicate
existing code or aren't needed yet. `runtime/snes_adapter.c` bridges the
handful of `Snes*` calls `dma.c` makes into `bus.c` directly -- see its
header comment.

`ppu.c` hard-asserts several register values to exactly what
A Link to the Past uses (BGMODE, OBSEL, M7SEL, WBGLOG/WOBJLOG, CGWSEL,
SETINI). Those asserts are relaxed for Chrono Trigger compatibility --
search this tree for `ct-recomp:` comments marking every deviation from
upstream.

## License

MIT License

Copyright (c) 2022 snesrev
Copyright (c) 2021 elzo_d

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
