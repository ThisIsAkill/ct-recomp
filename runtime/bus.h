/* 24-bit bus: WRAM, SRAM, ROM, hardware register hooks. */
#ifndef CT_BUS_H
#define CT_BUS_H

#include <stdint.h>

#define CT_ROM_SIZE  0x400000u
#define CT_WRAM_SIZE 0x20000u
#define CT_SRAM_SIZE 0x2000u

/* Load ROM from path (NULL: $CT_ROM), clear RAM, install built-in hooks.
   Fatal on any error. */
void bus_init(const char *rom_path);
/* Clear WRAM, SRAM, and hardware register state. ROM is kept. */
void bus_reset(void);

uint8_t  read8(uint32_t addr);
void     write8(uint32_t addr, uint8_t v);
uint16_t read16(uint32_t addr);             /* lo at addr, hi at addr+1 (24-bit) */
void     write16(uint32_t addr, uint16_t v);

/* Hardware registers $2000-$5FFF in banks $00-$3F/$80-$BF.
   Unhooked access is fatal. */
typedef uint8_t (*hw_read_fn)(uint16_t reg);
typedef void    (*hw_write_fn)(uint16_t reg, uint8_t v);
void bus_hook(uint16_t reg, hw_read_fn rd, hw_write_fn wr);

/* Test use: if set, called with the WRAM offset ($00000-$1FFFF) of every
   WRAM byte written, whatever the path (CPU store, WRAM data port, DMA). */
extern void (*ct_wram_write_hook)(uint32_t off);

uint8_t *bus_wram(void);
uint8_t *bus_sram(void);
const uint8_t *bus_rom(void);

#endif
