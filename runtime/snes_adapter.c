#include "snes_adapter.h"

#include "bus.h"
#include "cpu.h"
#include "hwlog.h"
#include "apu.h"
#include "dma.h"
#include "ppu.h"
#include "snes.h"

static Ppu *g_ppu;
static Dma *g_dma;
static Apu *g_apu;
static Snes g_snes_stub;

/* ---- Snes* stand-in dma.c expects (see third_party/snes/snes.h) ---- */

uint8_t snes_read(Snes *snes, uint32_t adr)
{
    (void)snes;
    return read8(adr);
}

void snes_write(Snes *snes, uint32_t adr, uint8_t val)
{
    (void)snes;
    write8(adr, val);
}

uint8_t snes_readBBus(Snes *snes, uint8_t adr)
{
    (void)snes;
    return read8(0x2100u + adr);
}

void snes_writeBBus(Snes *snes, uint8_t adr, uint8_t val)
{
    (void)snes;
    write8(0x2100u + adr, val);
}

/* ---- $2100-$213F PPU ---- */

/* Read side of the PPU, per bsnes sfc/ppu/io.cpp readIO and fullsnes:
 *   $2104-$2106 $2108-$210A $2114-$2116 $2118-$211A $2124-$2126
 *   $2128-$212A    write-only: PPU1 open bus (last value read from PPU1)
 *   other $2100-$2133  write-only: CPU open bus
 *   $2134-$2136    mode-7 product (vendored ppu_read)
 *   $2138          OAM data, byte address auto-increments; $200-$3FF
 *                  mirror the 32-byte high table
 *   $2139/$213A    VRAM data from a 16-bit prefetch latch: the latch loads
 *                  after a VMADD write and BEFORE the address increments
 *                  on a read, so the first word after VMADD comes twice
 *   $213B          CGRAM data, shares the write flip-flop; the high byte's
 *                  bit 7 is PPU2 open bus
 *   $213E          STAT77: PPU1 version 1, bit 4 PPU1 open bus; the OBJ
 *                  time/range overflow flags are not modeled (noted)
 *   $2137 $213C $213D $213F  counters, the frame scheduler's
 * $2134-$213F are read-only: writes are ignored and noted. VRAM address
 * remapping (VMAIN bits 2-3) is not modeled on reads, as on writes. */

static uint8_t ppu1_mdr, ppu2_mdr;
static uint16_t vram_latch;

uint8_t snes_ppu2_mdr(void) { return ppu2_mdr; }
void snes_set_ppu2_mdr(uint8_t v) { ppu2_mdr = v; }

static uint8_t oam_byte(unsigned b)
{
    unsigned w = b < 0x200 ? b >> 1 : 0x100 + ((b & 0x1F) >> 1);
    return (uint8_t)(g_ppu->oam[w] >> ((b & 1) * 8));
}

static void vram_prefetch_increment(void)
{
    vram_latch = g_ppu->vram[g_ppu->vramPointer & 0x7FFF];
    g_ppu->vramPointer += g_ppu->vramIncrement;
}

static uint8_t ppu_reg_read(uint16_t reg)
{
    switch (reg) {
    case 0x2104: case 0x2105: case 0x2106: case 0x2108: case 0x2109: case 0x210A:
    case 0x2114: case 0x2115: case 0x2116: case 0x2118: case 0x2119: case 0x211A:
    case 0x2124: case 0x2125: case 0x2126: case 0x2128: case 0x2129: case 0x212A:
        return ppu1_mdr;
    case 0x2134: case 0x2135: case 0x2136:
        return ppu1_mdr = ppu_read(g_ppu, (uint8_t)(reg - 0x2100u));
    case 0x2138: {
        unsigned b = (unsigned)(g_ppu->oamAdr << 1 | g_ppu->oamSecondWrite) & 0x3FF;
        ppu1_mdr = oam_byte(b);
        b = (b + 1) & 0x3FF;
        g_ppu->oamAdr = (uint16_t)(b >> 1);
        g_ppu->oamSecondWrite = b & 1;
        return ppu1_mdr;
    }
    case 0x2139:
        ppu1_mdr = (uint8_t)vram_latch;
        if (!g_ppu->vramIncrementOnHigh)
            vram_prefetch_increment();
        return ppu1_mdr;
    case 0x213A:
        ppu1_mdr = (uint8_t)(vram_latch >> 8);
        if (g_ppu->vramIncrementOnHigh)
            vram_prefetch_increment();
        return ppu1_mdr;
    case 0x213B: {
        uint16_t c = g_ppu->cgram[g_ppu->cgramPointer];
        if (!g_ppu->cgramSecondWrite) {
            ppu2_mdr = (uint8_t)c;
        } else {
            ppu2_mdr = (uint8_t)((ppu2_mdr & 0x80) | ((c >> 8) & 0x7F));
            g_ppu->cgramPointer++;
        }
        g_ppu->cgramSecondWrite = !g_ppu->cgramSecondWrite;
        return ppu2_mdr;
    }
    case 0x213E:
        hw_note("$213E STAT77: OBJ time/range overflow flags not modeled (read as 0)");
        return ppu1_mdr = (uint8_t)((ppu1_mdr & 0x10) | 0x01);
    case 0x2137: case 0x213C: case 0x213D: case 0x213F:
        ct_fatal("read8 $%04X: H/V counters need the frame scheduler", reg);
    default:
        return bus_open_bus(reg);   /* other write-only PPU registers */
    }
}

/* $2104 OAMDATA (replaces the vendored write, which buffered every pair
   and stopped at word $110): byte address = OAMADD*2 + flip-flop. The low
   table ($000-$1FF) latches the even byte and writes the pair on the odd
   one; the high table ($200-$21F, mirrored through $3FF) is written a
   byte at a time. The address wraps at $3FF. */
static uint16_t oamadd_reload;   /* last OAMADD, reloaded at VBlank */

static void oam_write(uint8_t v)
{
    unsigned b = (unsigned)(g_ppu->oamAdr << 1 | g_ppu->oamSecondWrite) & 0x3FF;
    if (b < 0x200) {
        if (!(b & 1))
            g_ppu->oamBuffer = v;
        else
            g_ppu->oam[b >> 1] = (uint16_t)(v << 8 | g_ppu->oamBuffer);
    } else {
        uint16_t *w = &g_ppu->oam[0x100 + ((b & 0x1F) >> 1)];
        *w = b & 1 ? (uint16_t)((*w & 0x00FF) | v << 8) : (uint16_t)((*w & 0xFF00) | v);
    }
    b = (b + 1) & 0x3FF;
    g_ppu->oamAdr = (uint16_t)(b >> 1);
    g_ppu->oamSecondWrite = b & 1;
}

void snes_oam_vblank_reload(void)
{
    if (g_ppu->forcedBlank)
        return;
    g_ppu->oamAdr = oamadd_reload;
    g_ppu->oamSecondWrite = false;
}

static void ppu_reg_write(uint16_t reg, uint8_t v)
{
    if (reg >= 0x2134) {
        bus_readonly_write(reg, v);
        return;
    }
    if (reg == 0x2104) {
        oam_write(v);
        return;
    }
    ppu_write(g_ppu, (uint8_t)(reg - 0x2100u), v);
    if (reg == 0x2102 || reg == 0x2103)
        oamadd_reload = g_ppu->oamAdr;
    if (reg == 0x2116 || reg == 0x2117)
        vram_latch = g_ppu->vram[g_ppu->vramPointer & 0x7FFF];   /* prefetch after VMADD */
}

/* ---- $2140-$2143 APU communication ports, mirrored through $217F ----
 * CPU side: writes land in the SPC700's input ports (what it reads at
 * $F4-$F7), reads return its output ports (what it wrote there).
 * apu_cpuRead/apu_cpuWrite are the SPC700's own memory map, not this. */

void (*snes_apu_sync)(void);

static uint8_t apu_reg_read(uint16_t reg)
{
    if (snes_apu_sync)
        snes_apu_sync();
    return g_apu->outPorts[reg & 3];
}

static void apu_reg_write(uint16_t reg, uint8_t v)
{
    if (snes_apu_sync)
        snes_apu_sync();
    g_apu->inPorts[reg & 3] = v;
}

void snes_apu_run(uint32_t spc_cycles)
{
    while (spc_cycles--)
        apu_cycle(g_apu);
}

/* ---- $4300-$437F DMA channel registers ---- */

/* $43x0-$43xA per channel; $43xB and $43xF are the same unused R/W byte
   (vendored); $43xC-$43xE are unused: CPU open bus, writes ignored. */
static uint8_t dma_reg_read(uint16_t reg)
{
    if ((reg & 0xF) >= 0xC && (reg & 0xF) <= 0xE)
        return bus_open_bus(reg);
    return dma_read(g_dma, (uint16_t)(reg - 0x4300u));
}

static void dma_reg_write(uint16_t reg, uint8_t v)
{
    if ((reg & 0xF) >= 0xC && (reg & 0xF) <= 0xE) {
        bus_unused_write(reg, v);
        return;
    }
    dma_write(g_dma, (uint16_t)(reg - 0x4300u), v);
}

/* ---- $420B MDMAEN / $420C HDMAEN ----
 * No cycle-driven main loop exists yet (see #11), so general DMA runs to
 * completion synchronously on the MDMAEN write instead of byte-at-a-time
 * over real cycles. HDMAEN only arms hdmaActive; dma_initHdma/dma_doHdma
 * need the scanline loop #11 will add, so HDMA registers hold correct
 * state but do nothing until then. */

static void mdmaen_write(uint16_t reg, uint8_t v)
{
    (void)reg;
    dma_startDma(g_dma, v, false);
    while (g_dma->dmaBusy)
        dma_doDma(g_dma);
}

static void hdmaen_write(uint16_t reg, uint8_t v)
{
    (void)reg;
    dma_startDma(g_dma, v, true);
}

void snes_hw_init(void)
{
    if (!g_ppu) {
        g_ppu = ppu_init();
        g_apu = apu_init();
        g_dma = dma_init(&g_snes_stub);
    }

    for (uint16_t r = 0x2100; r <= 0x213F; r++)
        bus_hook(r, ppu_reg_read, ppu_reg_write);
    for (uint16_t r = 0x2140; r <= 0x217F; r++)   /* $2144-$217F mirror $2140-$2143 */
        bus_hook(r, apu_reg_read, apu_reg_write);
    for (uint16_t r = 0x4300; r <= 0x437F; r++)
        bus_hook(r, dma_reg_read, dma_reg_write);
    bus_hook(0x420B, bus_open_bus, mdmaen_write);   /* write-only: open bus */
    bus_hook(0x420C, bus_open_bus, hdmaen_write);
}

void snes_hw_reset(void)
{
    if (!g_ppu)
        return;
    ppu_reset(g_ppu);
    ppu1_mdr = ppu2_mdr = 0;
    vram_latch = 0;
    oamadd_reload = 0;
    dma_reset(g_dma);
    apu_reset(g_apu);
}

Ppu *snes_hw_ppu(void) { return g_ppu; }
Dma *snes_hw_dma(void) { return g_dma; }
Apu *snes_hw_apu(void) { return g_apu; }
