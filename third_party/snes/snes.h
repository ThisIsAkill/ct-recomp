/* ct-recomp: not from zelda3. Minimal stand-in for the Snes* dma.c
 * expects -- only the fields/calls it actually touches. See
 * runtime/snes_adapter.c for the real implementations, which forward to
 * ct-recomp's own bus (runtime/bus.c) instead of zelda3's snes.c. */
#ifndef CT_SNES_COMPAT_H
#define CT_SNES_COMPAT_H

#include <stdint.h>

#include "saveload.h"   /* SaveLoadFunc, referenced by dma.h's prototypes */

typedef struct Snes {
    uint8_t openBus;
} Snes;

uint8_t snes_read(Snes *snes, uint32_t adr);
void snes_write(Snes *snes, uint32_t adr, uint8_t val);
uint8_t snes_readBBus(Snes *snes, uint8_t adr);
void snes_writeBBus(Snes *snes, uint8_t adr, uint8_t val);

#endif
