#include "snes_adapter.h"

#include "bus.h"
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

static uint8_t ppu_reg_read(uint16_t reg)
{
    return ppu_read(g_ppu, (uint8_t)(reg - 0x2100u));
}

static void ppu_reg_write(uint16_t reg, uint8_t v)
{
    ppu_write(g_ppu, (uint8_t)(reg - 0x2100u), v);
}

/* ---- $2140-$2143 APU communication ports ----
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

static uint8_t dma_reg_read(uint16_t reg)
{
    return dma_read(g_dma, (uint16_t)(reg - 0x4300u));
}

static void dma_reg_write(uint16_t reg, uint8_t v)
{
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
    for (uint16_t r = 0x2140; r <= 0x2143; r++)
        bus_hook(r, apu_reg_read, apu_reg_write);
    for (uint16_t r = 0x4300; r <= 0x437F; r++)
        bus_hook(r, dma_reg_read, dma_reg_write);
    bus_hook(0x420B, NULL, mdmaen_write);
    bus_hook(0x420C, NULL, hdmaen_write);
}

void snes_hw_reset(void)
{
    if (!g_ppu)
        return;
    ppu_reset(g_ppu);
    dma_reset(g_dma);
    apu_reset(g_apu);
}

Ppu *snes_hw_ppu(void) { return g_ppu; }
Dma *snes_hw_dma(void) { return g_dma; }
Apu *snes_hw_apu(void) { return g_apu; }
