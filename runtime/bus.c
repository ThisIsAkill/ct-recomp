#include "bus.h"
#include "cpu.h"
#include "snes_adapter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t *rom;
static uint8_t wram[CT_WRAM_SIZE];
static uint8_t sram[CT_SRAM_SIZE];

#define HW_BASE 0x2000u
#define HW_SIZE 0x4000u
static hw_read_fn  hw_rd[HW_SIZE];
static hw_write_fn hw_wr[HW_SIZE];

/* ---- $4202-$4217 multiply/divide unit (results immediate in v0) ---- */

static struct {
    uint8_t  wrmpya, wrmpyb, wrdivb;
    uint16_t wrdiv, rddiv, rdmpy;
} alu;

static void math_write(uint16_t reg, uint8_t v)
{
    switch (reg) {
    case 0x4202: alu.wrmpya = v; break;
    case 0x4203:
        alu.wrmpyb = v;
        alu.rdmpy = (uint16_t)(alu.wrmpya * v);
        break;
    case 0x4204: alu.wrdiv = (uint16_t)((alu.wrdiv & 0xFF00) | v); break;
    case 0x4205: alu.wrdiv = (uint16_t)((alu.wrdiv & 0x00FF) | (v << 8)); break;
    case 0x4206:
        alu.wrdivb = v;
        if (v == 0) {
            alu.rddiv = 0xFFFF;
            alu.rdmpy = alu.wrdiv;
        } else {
            alu.rddiv = (uint16_t)(alu.wrdiv / v);
            alu.rdmpy = (uint16_t)(alu.wrdiv % v);
        }
        break;
    default: ct_fatal("math_write: bad register $%04X", reg);
    }
}

/* ---- $2180-$2183 WRAM data port ---- */

static uint32_t wram_port_addr;

static uint8_t wram_port_read(uint16_t reg)
{
    (void)reg;
    uint8_t v = wram[wram_port_addr & (CT_WRAM_SIZE - 1)];
    wram_port_addr = (wram_port_addr + 1) & 0x1FFFFu;
    return v;
}

static void wram_port_write(uint16_t reg, uint8_t v)
{
    switch (reg) {
    case 0x2180:
        wram[wram_port_addr & (CT_WRAM_SIZE - 1)] = v;
        wram_port_addr = (wram_port_addr + 1) & 0x1FFFFu;
        break;
    case 0x2181: wram_port_addr = (wram_port_addr & 0x1FF00u) | v; break;
    case 0x2182: wram_port_addr = (wram_port_addr & 0x100FFu) | ((uint32_t)v << 8); break;
    case 0x2183: wram_port_addr = (wram_port_addr & 0x0FFFFu) | ((uint32_t)(v & 1) << 16); break;
    default: ct_fatal("wram_port_write: bad register $%04X", reg);
    }
}

static uint8_t math_read(uint16_t reg)
{
    switch (reg) {
    case 0x4214: return (uint8_t)alu.rddiv;
    case 0x4215: return (uint8_t)(alu.rddiv >> 8);
    case 0x4216: return (uint8_t)alu.rdmpy;
    case 0x4217: return (uint8_t)(alu.rdmpy >> 8);
    default: ct_fatal("math_read: bad register $%04X", reg);
    }
}

/* ---- setup ---- */

void bus_hook(uint16_t reg, hw_read_fn rd, hw_write_fn wr)
{
    if (reg < HW_BASE || reg >= HW_BASE + HW_SIZE)
        ct_fatal("bus_hook: $%04X outside $2000-$5FFF", reg);
    hw_rd[reg - HW_BASE] = rd;
    hw_wr[reg - HW_BASE] = wr;
}

void bus_reset(void)
{
    memset(wram, 0, sizeof wram);
    memset(sram, 0, sizeof sram);
    memset(&alu, 0, sizeof alu);
    wram_port_addr = 0;
    snes_hw_reset();
}

void bus_init(const char *path)
{
    if (!path)
        path = getenv("CT_ROM");
    if (!path || !*path)
        ct_fatal("CT_ROM not set");
    FILE *f = fopen(path, "rb");
    if (!f)
        ct_fatal("cannot open ROM: %s", path);
    if (!rom)
        rom = malloc(CT_ROM_SIZE);
    if (!rom)
        ct_fatal("out of memory");
    size_t n = fread(rom, 1, CT_ROM_SIZE, f);
    int extra = fgetc(f);
    fclose(f);
    if (n != CT_ROM_SIZE || extra != EOF)
        ct_fatal("ROM %s: expected %u bytes", path, CT_ROM_SIZE);

    memset(hw_rd, 0, sizeof hw_rd);
    memset(hw_wr, 0, sizeof hw_wr);
    for (uint16_t r = 0x4202; r <= 0x4206; r++)
        bus_hook(r, NULL, math_write);
    for (uint16_t r = 0x4214; r <= 0x4217; r++)
        bus_hook(r, math_read, NULL);
    bus_hook(0x2180, wram_port_read, wram_port_write);
    for (uint16_t r = 0x2181; r <= 0x2183; r++)
        bus_hook(r, NULL, wram_port_write);
    snes_hw_init();
    bus_reset();
}

uint8_t *bus_wram(void) { return wram; }
uint8_t *bus_sram(void) { return sram; }
const uint8_t *bus_rom(void) { return rom; }

/* ---- HiROM map ---- */

enum region { R_WRAM, R_SRAM, R_ROM, R_HW, R_OPEN };

static enum region decode_addr(uint32_t a, uint32_t *off)
{
    uint8_t bank = (uint8_t)(a >> 16);
    uint16_t lo = (uint16_t)a;

    if (bank == 0x7E || bank == 0x7F) {
        *off = a - 0x7E0000u;
        return R_WRAM;
    }
    if (bank >= 0xC0) {
        *off = a - 0xC00000u;
        return R_ROM;
    }
    if (bank >= 0x40 && bank <= 0x7D) {
        *off = a - 0x400000u;
        return R_ROM;
    }
    /* $00-$3F, $80-$BF */
    if (lo < 0x2000) {
        *off = lo;
        return R_WRAM;
    }
    if (lo < 0x6000) {
        *off = lo;
        return R_HW;
    }
    if (lo < 0x8000) {
        if ((bank & 0x7F) >= 0x20) {
            *off = lo & (CT_SRAM_SIZE - 1);
            return R_SRAM;
        }
        return R_OPEN;
    }
    *off = ((uint32_t)(bank & 0x3F) << 16) | lo;
    return R_ROM;
}

uint8_t read8(uint32_t a)
{
    uint32_t off;
    a &= 0xFFFFFF;
    switch (decode_addr(a, &off)) {
    case R_WRAM: return wram[off];
    case R_SRAM: return sram[off];
    case R_ROM:  return rom[off];
    case R_HW:
        if (hw_rd[off - HW_BASE])
            return hw_rd[off - HW_BASE]((uint16_t)off);
        ct_fatal("read8 $%06X: unhooked hardware register", a);
    default:
        ct_fatal("read8 $%06X: open bus", a);
    }
}

void write8(uint32_t a, uint8_t v)
{
    uint32_t off;
    a &= 0xFFFFFF;
    switch (decode_addr(a, &off)) {
    case R_WRAM: wram[off] = v; return;
    case R_SRAM: sram[off] = v; return;
    case R_ROM:
        ct_fatal("write8 $%06X: write to ROM", a);
    case R_HW:
        if (hw_wr[off - HW_BASE]) {
            hw_wr[off - HW_BASE]((uint16_t)off, v);
            return;
        }
        ct_fatal("write8 $%06X: unhooked hardware register", a);
    default:
        ct_fatal("write8 $%06X: open bus", a);
    }
}

uint16_t read16(uint32_t a)
{
    uint8_t lo = read8(a);
    return (uint16_t)(lo | (read8((a + 1) & 0xFFFFFF) << 8));
}

void write16(uint32_t a, uint16_t v)
{
    write8(a, (uint8_t)v);
    write8((a + 1) & 0xFFFFFF, (uint8_t)(v >> 8));
}
