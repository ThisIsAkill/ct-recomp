/* Bus routing into the vendored SNES core (third_party/snes/), via the
 * adapter (runtime/snes_adapter.c): $21xx PPU, $43xx DMA, $420B MDMAEN. */
#include <string.h>

#include "harness.h"
#include "ppu.h"
#include "dma.h"
#include "snes_adapter.h"

static void test_ppu_registers(void)
{
    write8(0x2100, 0x0F);   /* INIDISP: brightness 15, no forced blank */
    CHECK(snes_hw_ppu()->brightness == 15 && !snes_hw_ppu()->forcedBlank, "INIDISP");

    write8(0x2105, 0x00);   /* BGMODE: mode 0 -- asserted away upstream, see #9 */
    CHECK(snes_hw_ppu()->mode == 0, "BGMODE mode 0 reaches the PPU");
    write8(0x2105, 0x09);   /* mode 1 with BG3 priority */
    CHECK(snes_hw_ppu()->mode == 1 && snes_hw_ppu()->bg3Priority, "BGMODE mode 1 + prio");
    write8(0x2105, 0x01);   /* mode 1 without BG3 priority */
    CHECK(snes_hw_ppu()->mode == 1 && !snes_hw_ppu()->bg3Priority, "BGMODE mode 1, no prio");

    write8(0x2101, 0x2A);   /* OBSEL: size 1, name select 1, base 2 -> $4000 */
    CHECK(snes_hw_ppu()->objSize == 1 && snes_hw_ppu()->objTileAdr1 == 0x4000 &&
          snes_hw_ppu()->objTileAdr2 == 0x6000, "OBSEL decode");

    /* Mode-7 multiply (the only PPU value ppu_read exposes): $211B/$211C
     * write low byte then high byte; $2134-$2136 read the 24-bit product. */
    write8(0x211B, 0xE8);
    write8(0x211B, 0x03);   /* M7A = 1000 */
    write8(0x211C, 0x00);
    write8(0x211C, 0x07);   /* M7B = $0700; only the high byte (7) multiplies */
    int product = read8(0x2134) | read8(0x2135) << 8 | read8(0x2136) << 16;
    CHECK(product == 1000 * 7, "mode-7 multiply via $2134-$2136: got %d", product);
}

static void test_dma_to_vram(void)
{
    uint8_t *w = bus_wram();
    memcpy(w, "\x11\x22\x33\x44", 4);

    write8(0x2115, 0x00);            /* VMAIN: increment 1, on low byte write */
    write8(0x2116, 0x00);            /* VMADDL/H: VRAM pointer = 0 */
    write8(0x2117, 0x00);

    write8(0x4300, 0x00);            /* DMAP0: A->B, 1 byte/unit, increment */
    write8(0x4301, 0x18);            /* BBAD0: $2118 (VMDATAL) */
    write8(0x4302, 0x00);            /* A1T0L/H: source $7E0000 */
    write8(0x4303, 0x00);
    write8(0x4304, 0x7E);            /* A1B0: source bank */
    write8(0x4305, 0x04);            /* DAS0L/H: 4 bytes */
    write8(0x4306, 0x00);

    CHECK(!snes_hw_dma()->dmaBusy, "dma idle before MDMAEN");
    write8(0x420B, 0x01);            /* MDMAEN: fire channel 0 */
    CHECK(!snes_hw_dma()->dmaBusy, "dma drains synchronously (no main loop yet, see #11)");
    CHECK(snes_hw_dma()->channel[0].size == 0, "channel 0 fully drained");

    Ppu *ppu = snes_hw_ppu();
    CHECK((ppu->vram[0] & 0xFF) == 0x11 && (ppu->vram[1] & 0xFF) == 0x22 &&
          (ppu->vram[2] & 0xFF) == 0x33 && (ppu->vram[3] & 0xFF) == 0x44,
          "DMA'd bytes landed in VRAM in order");
    CHECK(ppu->vramPointer == 4, "VRAM pointer advanced by the transfer");
}

int main(void)
{
    bus_init(NULL);
    test_ppu_registers();
    test_dma_to_vram();
    return th_report("snes_bus");
}
