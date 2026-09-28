/* Bridges the vendored zelda3 PPU/DMA/APU (third_party/snes/) into
 * ct-recomp's own bus. Called from bus.c only. */
#ifndef CT_SNES_ADAPTER_H
#define CT_SNES_ADAPTER_H

#include <stdint.h>

/* Allocate the PPU/DMA/APU instances and hook their registers into the
 * bus. Called once, at the end of bus_init(). */
void snes_hw_init(void);

/* Reset PPU/DMA/APU state. Called from bus_reset(). */
void snes_hw_reset(void);

/* APU: run the SPC700 for n cycles. snes_apu_sync, if set, is called
   before every CPU access to $2140-$2143 so the SPC700 can be brought up
   to the moment of the access (the frame scheduler sets it): `early`
   clocks before the end of the access cycle -- a read samples the bus 4
   clocks before the cycle ends, a write lands at its end.
   snes_apu_catch_up brings the SPC700 up to a master clock, by the
   timing rules in snes_adapter.c; every sync goes through it. */
void snes_apu_run(uint32_t spc_cycles);
void snes_apu_catch_up(uint64_t master);
extern void (*snes_apu_sync)(unsigned early);

/* Master clocks since power-on at the current instruction boundary (the
   frame scheduler sets it; general DMA aligns to it). */
extern uint64_t (*snes_master_clock)(void);
/* Master clock of the CPU access being made, `early` clocks before the
   end of its cycle, counting any DRAM refresh earlier in the instruction
   (the frame scheduler sets it). */
extern uint64_t (*snes_access_clock)(unsigned early);

/* Start of VBlank: outside forced blank, the OAM address reloads from the
   last OAMADD ($2102/$2103) write. Called by the frame scheduler. */
void snes_oam_vblank_reload(void);

/* PPU2 open bus (last value read from PPU2): the frame scheduler's
   counter registers ($213C, $213D, $213F) read and set it. */
uint8_t snes_ppu2_mdr(void);
void snes_set_ppu2_mdr(uint8_t v);

/* Test-only introspection, same idea as bus_wram()/bus_sram(). */
typedef struct Ppu Ppu;
typedef struct Dma Dma;
typedef struct Apu Apu;
Ppu *snes_hw_ppu(void);
Dma *snes_hw_dma(void);
Apu *snes_hw_apu(void);

#endif
