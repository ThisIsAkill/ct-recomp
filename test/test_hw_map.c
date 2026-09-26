/* $2100-$21FF and $4200-$43FF address map against documented hardware
 * behavior (fullsnes; bsnes sfc/ppu/io.cpp for the PPU read side):
 * mirrors, CPU/PPU1/PPU2 open bus, read-only and write-only registers,
 * unused addresses. */
#include "harness.h"
#include "hwlog.h"
#include "ppu.h"
#include "apu.h"
#include "sched.h"
#include "snes_adapter.h"

/* Put v on the CPU data bus (a WRAM write) so open-bus reads are
   distinguishable. */
static void cpu_bus(uint8_t v)
{
    write8(0x7E1F00, v);
}

static int noted(const char *needle)
{
    for (unsigned k = 0; k < hw_note_count(); k++)
        if (strstr(hw_note_text(k), needle))
            return 1;
    return 0;
}

static void test_apu_mirrors(void)
{
    bus_reset();
    Apu *apu = snes_hw_apu();
    write8(0x217F, 0x11);   /* mirror of $2143 */
    write8(0x2144, 0x22);   /* mirror of $2140 */
    CHECK(apu->inPorts[3] == 0x11 && apu->inPorts[0] == 0x22, "writes through $2144-$217F");
    apu->outPorts[1] = 0x5A;
    CHECK(read8(0x2145) == 0x5A && read8(0x217D) == 0x5A, "reads through the mirrors");
}

static void test_open_bus_classes(void)
{
    bus_reset();
    write8(0x211B, 0x10);
    write8(0x211B, 0x00);   /* M7A = $0010 */
    write8(0x211C, 0x00);
    write8(0x211C, 0x03);   /* M7B high = 3: product $30 */
    uint8_t p = read8(0x2134);
    CHECK(p == 0x30, "mode-7 product low byte $%02X", p);
    cpu_bus(0xC5);
    CHECK(read8(0x2104) == 0x30 && read8(0x2129) == 0x30, "PPU1 open bus registers");
    cpu_bus(0xC5);
    CHECK(read8(0x2100) == 0xC5, "other write-only PPU register: CPU open bus");
    cpu_bus(0xC6);
    CHECK(read8(0x2181) == 0xC6, "WMADD is write-only: CPU open bus");

    static const uint16_t unused[] = {0x2184, 0x21FF, 0x420E, 0x420F, 0x4220, 0x42FF,
                                      0x430C, 0x437E, 0x4380, 0x43FF};
    for (unsigned k = 0; k < sizeof unused / sizeof unused[0]; k++) {
        cpu_bus((uint8_t)(0x40 + k));
        CHECK(read8(unused[k]) == 0x40 + k, "$%04X reads CPU open bus", unused[k]);
        write8(unused[k], 0xEE);   /* ignored */
    }
    CHECK(noted("write $2184 ignored: unused") && noted("write $43FF ignored: unused"),
          "unused writes noted");
    write8(0x430B, 0x77);
    CHECK(read8(0x430F) == 0x77, "$43xB/$43xF are one unused R/W byte");
    cpu_bus(0x3C);
    CHECK(read8(0x4200) == 0x3C && read8(0x420A) == 0x3C, "write-only timing registers");
}

static void test_vram_read(void)
{
    bus_reset();
    Ppu *ppu = snes_hw_ppu();
    ppu->vram[0x10] = 0x1234;
    ppu->vram[0x11] = 0x5678;
    write8(0x2115, 0x80);   /* VMAIN: +1 after the high byte */
    write8(0x2116, 0x10);
    write8(0x2117, 0x00);
    uint8_t r[6];
    for (int k = 0; k < 6; k++)
        r[k] = read8(k & 1 ? 0x213A : 0x2139);
    /* Prefetch BEFORE increment: the first word comes back twice. */
    CHECK(r[0] == 0x34 && r[1] == 0x12 && r[2] == 0x34 && r[3] == 0x12 && r[4] == 0x78 &&
          r[5] == 0x56, "VRAM reads %02X %02X %02X %02X %02X %02X", r[0], r[1], r[2], r[3],
          r[4], r[5]);
}

static void test_oam(void)
{
    bus_reset();
    Ppu *ppu = snes_hw_ppu();
    write8(0x2102, 0x00);
    write8(0x2103, 0x00);
    static const uint8_t bytes[] = {0x11, 0x22, 0x33, 0x44};
    for (int k = 0; k < 4; k++)
        write8(0x2104, bytes[k]);
    CHECK(ppu->oam[0] == 0x2211 && ppu->oam[1] == 0x4433, "low table pairs");
    write8(0x2102, 0x00);
    int ok = 1;
    for (int k = 0; k < 4; k++)
        ok &= read8(0x2138) == bytes[k];
    CHECK(ok, "OAM read-back");

    write8(0x2102, 0x00);
    write8(0x2103, 0x01);   /* word $100: byte $200, the high table */
    write8(0x2104, 0xAB);   /* one byte, written directly */
    CHECK((ppu->oam[0x100] & 0xFF) == 0xAB, "high table byte write: $%04X", ppu->oam[0x100]);
    write8(0x2102, 0x10);
    write8(0x2103, 0x01);   /* word $110: byte $220, mirrors $200 */
    CHECK(read8(0x2138) == 0xAB, "$220 mirrors the high table");

    /* VBlank reload (outside forced blank) returns to the last OAMADD. */
    write8(0x2100, 0x0F);
    write8(0x2102, 0x04);
    write8(0x2103, 0x00);
    write8(0x2104, 0x01);
    write8(0x2104, 0x02);
    snes_oam_vblank_reload();
    CHECK(ppu->oamAdr == 4 && !ppu->oamSecondWrite, "OAM address reloaded: %u", ppu->oamAdr);
    write8(0x2100, 0x80);   /* forced blank: no reload */
    write8(0x2104, 0x03);
    write8(0x2104, 0x04);
    snes_oam_vblank_reload();
    CHECK(ppu->oamAdr == 5, "no reload during forced blank: %u", ppu->oamAdr);
}

static void test_cgram_stat77_readonly(void)
{
    bus_reset();
    Ppu *ppu = snes_hw_ppu();
    write8(0x2121, 5);
    write8(0x2122, 0xB4);
    write8(0x2122, 0x7F);
    CHECK(ppu->cgram[5] == 0x7FB4, "CGRAM write $%04X", ppu->cgram[5]);
    write8(0x2121, 5);
    uint8_t lo = read8(0x213B), hi = read8(0x213B);
    /* High byte bit 7 is PPU2 open bus: the low byte's bit 7 here. */
    CHECK(lo == 0xB4 && hi == 0xFF, "CGRAM read %02X %02X", lo, hi);

    read8(0x2134);   /* PPU1 bus: product low byte = 0 after reset */
    CHECK(read8(0x213E) == 0x01, "STAT77: PPU1 version 1");
    CHECK(noted("$213E STAT77"), "overflow flags noted as not modeled");

    write8(0x2135, 0x99);
    write8(0x213E, 0x99);
    CHECK(noted("write $2135 ignored: read-only") && noted("write $213E ignored: read-only"),
          "writes to $2134-$213F are ignored and noted");
}

static void test_scheduler_registers(void)
{
    static CPU c;
    bus_reset();
    cpu_init(&c);
    sched_init(&c);
    write8(0x4201, 0x7F);
    CHECK(read8(0x4213) == 0x7F, "RDIO reads back WRIO");
    cpu_bus(0xA5);
    CHECK(read8(0x2137) == 0xA5, "SLHV returns CPU open bus");
    write8(0x213C, 1);
    write8(0x213F, 1);
    CHECK(noted("write $213C ignored") && noted("write $213F ignored"),
          "counter registers are read-only");
}

int main(void)
{
    th_bus_init();
    test_apu_mirrors();
    test_open_bus_classes();
    test_vram_read();
    test_oam();
    test_cgram_stat77_readonly();
    test_scheduler_registers();
    return th_report("hw_map");
}
