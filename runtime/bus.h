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

/* Read handler for write-only registers: returns the last value on the
   data bus, as hardware does (for LDA abs that is the address high byte).
   Used for write-only and unused registers (see snes_adapter.c and
   bus.c for the map). The interpreter's opcode and operand fetches go
   through read8; generated code has none, so it sets bus_mdr to each
   instruction's last byte instead (ct_insn in ops.h). */
extern uint8_t bus_mdr;
uint8_t bus_open_bus(uint16_t reg);

/* Write handler for read-only registers ($4210-$421F): ignored, as on
   hardware; noted once per register (hwlog.h). */
void bus_readonly_write(uint16_t reg, uint8_t v);

/* Write handler for unused addresses ($2184-$21FF, $420E-$420F,
   $4220-$42FF, $43xC-$43xE, $4380-$43FF): ignored, noted once. */
void bus_unused_write(uint16_t reg, uint8_t v);

/* $420D MEMSEL bit 0: FastROM enabled for banks $80-$FF. */
int bus_fastrom(void);

uint8_t *bus_wram(void);
uint8_t *bus_sram(void);
const uint8_t *bus_rom(void);

#endif
