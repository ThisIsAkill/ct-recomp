/* Bridges the vendored zelda3 PPU/DMA/APU (third_party/snes/) into
 * ct-recomp's own bus. Called from bus.c only. */
#ifndef CT_SNES_ADAPTER_H
#define CT_SNES_ADAPTER_H

/* Allocate the PPU/DMA/APU instances and hook their registers into the
 * bus. Called once, at the end of bus_init(). */
void snes_hw_init(void);

/* Reset PPU/DMA/APU state. Called from bus_reset(). */
void snes_hw_reset(void);

/* Test-only introspection, same idea as bus_wram()/bus_sram(). */
typedef struct Ppu Ppu;
typedef struct Dma Dma;
typedef struct Apu Apu;
Ppu *snes_hw_ppu(void);
Dma *snes_hw_dma(void);
Apu *snes_hw_apu(void);

#endif
