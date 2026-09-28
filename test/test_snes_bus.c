/* Bus routing into the vendored SNES core (third_party/snes/), via the
 * adapter (runtime/snes_adapter.c): $21xx PPU, $43xx DMA, $420B MDMAEN. */
#include <string.h>

#include "harness.h"
#include "hwlog.h"
#include "ppu.h"
#include "apu.h"
#include "dma.h"
#include "sched.h"
#include "snes_adapter.h"
#include "spc700_host.h"

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

/* $2180-$2183 WRAM data port. The port address is bus state that survives
 * a WRAM memcpy, so bus_reset() must clear it: test_diff relies on that to
 * start generated code and the interpreter from the same state. */
static void test_wram_port(void)
{
    uint8_t *w = bus_wram();

    write8(0x2181, 0x34);            /* WMADDL/M/H: $1_1234 */
    write8(0x2182, 0x12);
    write8(0x2183, 0x01);
    write8(0x2180, 0xAB);
    write8(0x922180, 0xCD);          /* bank $92 mirrors $12: same port */
    CHECK(w[0x11234] == 0xAB && w[0x11235] == 0xCD, "WMDATA writes auto-increment");
    CHECK(read8(0x2180) == w[0x11236], "WMDATA read continues from the port address");

    bus_reset();
    memset(w, 0x49, 2);
    write8(0x2180, 0x00);
    CHECK(w[0] == 0x00 && w[1] == 0x49, "bus_reset clears the WRAM port address");
}

/* $2140-$2143: CPU writes reach the SPC700's input ports, CPU reads see
 * its output ports. Then the real IPL boot ROM: after it runs, the CPU
 * sees its $AA/$BB ready signature, the first step of every upload. */
static void test_apu_ports(void)
{
    bus_reset();
    Apu *apu = snes_hw_apu();
    write8(0x2140, 0x12);
    write8(0x002143, 0x34);
    CHECK(apu->inPorts[0] == 0x12 && apu->inPorts[3] == 0x34, "CPU writes -> SPC input ports");
    CHECK(apu->ram[0] == 0 && apu->ram[3] == 0, "not SPC RAM $0000-$0003");
    apu->outPorts[1] = 0x5A;
    CHECK(read8(0x2141) == 0x5A, "CPU reads <- SPC output ports");
    write8(0x217F, 0x56);   /* $2144-$217F mirror the four ports (addr & 3) */
    CHECK(apu->inPorts[3] == 0x56 && read8(0x217D) == 0x5A, "APU port mirrors");

    bus_reset();
    CHECK(read8(0x2140) == 0 && read8(0x2141) == 0, "no signature before the IPL runs");
    snes_apu_run(100000);
    CHECK(read8(0x2140) == 0xAA && read8(0x2141) == 0xBB, "IPL ready signature: $%02X $%02X",
          read8(0x2140), read8(0x2141));
}

/* $4210-$421F are read-only: writes are ignored (as on hardware) and
 * noted once per register; reads are unaffected. Menu_InitPpu ($C2940D)
 * zeroes $4216-$4219 this way. */
static void test_readonly_writes(void)
{
    bus_reset();
    write8(0x4202, 12);
    write8(0x4203, 10);                  /* WRMPYB: product 120 in $4216 */
    unsigned before = hw_note_count();
    write8(0x4216, 0x00);
    write8(0x004216, 0x00);
    write8(0x4217, 0x00);
    CHECK(read8(0x4216) == 120 && read8(0x4217) == 0, "product survives writes: %u",
          read8(0x4216));
    CHECK(hw_note_count() == before + 2, "one note per register: %u new",
          hw_note_count() - before);
    CHECK(strstr(hw_note_text(before), "$4216") != NULL, "note names the register: %s",
          hw_note_text(before));
}

/* A CPU write reaches the SPC700's port latch at once when the SPC700 is
   within one half-cycle of its input clock of the CPU's time, otherwise
   after the SPC700's next cycle (Mesen 2's timing, #33). Master clock of
   the access as the adapter sees it, set by the test. */
static uint64_t fake_clock;
static uint64_t test_clock(unsigned early) { (void)early; return fake_clock; }

static void test_port_write_timing(void)
{
    bus_reset();
    CHECK(spc_host_cycle() == 2, "SPC700 reset reads its vector in 2 cycles: %llu",
          (unsigned long long)spc_host_cycle());
    snes_access_clock = test_clock;
    Apu *apu = snes_hw_apu();
    snes_apu_run(100);
    double half = 21477270.0 / (32040.0 * 64);   /* master clocks per half-cycle */
    double h = 2.0 * (double)spc_host_cycle();   /* SPC700 time, half-cycles */
    fake_clock = (uint64_t)((h + 1.5) * half);   /* 1.5 half-cycles ahead */
    write8(0x2140, 0x5A);
    CHECK(apu_inport_read(apu, 0) != 0x5A, "1.5 half-cycles ahead: not seen yet");
    snes_apu_run(1);
    CHECK(apu_inport_read(apu, 0) == 0x5A, "seen after the SPC700's next cycle");
    fake_clock = (uint64_t)((h + 2.5) * half);   /* half a half-cycle ahead */
    write8(0x2140, 0x77);
    CHECK(apu_inport_read(apu, 0) == 0x77, "within a half-cycle: seen at once");
    snes_access_clock = NULL;
}


/* SPC700 timers: the stage-1 clock falls every 128 cycles (16 for timer 2)
   counted from reset, first at the end of cycle 128 (bsnes/ares/Mesen 2). */
static void test_spc_timer_phase(void)
{
    bus_reset();
    Apu *apu = snes_hw_apu();
    apu_reset(apu);
    apu_cpuWrite(apu, 0xFA, 1);      /* T0 target 1 */
    apu_cpuWrite(apu, 0xFC, 1);      /* T2 target 1 */
    apu_cpuWrite(apu, 0xF1, 0x05);   /* enable T0 and T2 */
    for (int k = 0; k < 15; k++)
        apu_tick(apu);
    CHECK(apu_cpuRead(apu, 0xFF) == 0, "T2 before the end of cycle 16");
    apu_tick(apu);
    CHECK(apu_cpuRead(apu, 0xFF) == 1, "T2 at the end of cycle 16");
    for (int k = 16; k < 127; k++)
        apu_tick(apu);
    CHECK(apu_cpuRead(apu, 0xFD) == 0, "T0 before the end of cycle 128");
    apu_tick(apu);
    CHECK(apu_cpuRead(apu, 0xFD) == 1, "T0 at the end of cycle 128");
}

/* CGRAM holds 15-bit colors: bit 7 of the high byte isn't stored. */
static void test_cgram_15bit(void)
{
    bus_reset();
    write8(0x2121, 0x10);
    write8(0x2122, 0xFF);
    write8(0x2122, 0xFF);
    CHECK(snes_hw_ppu()->cgram[0x10] == 0x7FFF, "CGRAM $10 = $%04X", snes_hw_ppu()->cgram[0x10]);
}

/* A PPU write partway through a line changes the rest of it: the line is
   drawn through pixel dot - 22 first (Mesen 2). Forced blank at dot 122
   leaves pixels 0-100 lit. */
static int fake_dot;
static int get_fake_dot(void) { return fake_dot; }

static void test_midline_write(void)
{
    bus_reset();
    Ppu *p = snes_hw_ppu();
    write8(0x2100, 0x0F);   /* brightness 15 */
    write8(0x2121, 0x00);
    write8(0x2122, 0x1F);   /* backdrop: red */
    write8(0x2122, 0x00);
    static uint8_t buf[256 * 4 * 2];
    PpuBeginDrawing(p, buf, 256 * 4, 0);
    snes_ppu_dot = get_fake_dot;
    ppu_runLine(p, 0);
    ppu_runLine(p, 1);
    fake_dot = 122;
    write8(0x2100, 0x80);
    ppu_runLine(p, 2);   /* finishes line 1 */
    snes_ppu_dot = NULL;
    CHECK(buf[100 * 4 + 2] > 0 && buf[0 * 4 + 2] > 0, "pixels 0-100 drawn before the write");
    CHECK(buf[101 * 4 + 2] == 0 && buf[255 * 4 + 2] == 0, "pixel 101 on blanked (R %u)",
          buf[101 * 4 + 2]);
}

/* Offset-per-tile (mode 2): the BG3 tilemap row at BG3's V scroll gives
   BG1 a new H offset per tile column, from the second column on. Column 1
   of the screen is pointed at BG1 column 5, the only one with a visible
   tile; column 0 never takes an offset. */
static void test_offset_per_tile(void)
{
    bus_reset();
    Ppu *p = snes_hw_ppu();
    write8(0x2100, 0x0F);   /* INIDISP: brightness 15 */
    write8(0x2105, 0x02);   /* BGMODE 2 */
    write8(0x2107, 0x00);   /* BG1 tilemap at $0000 */
    write8(0x2109, 0x04);   /* BG3 tilemap (the offset table) at $0400 */
    write8(0x210B, 0x01);   /* BG1 tiles at $1000 */
    write8(0x212C, 0x01);   /* TM: BG1 */
    write8(0x2121, 0x01);
    write8(0x2122, 0x1F);   /* color 1: red */
    write8(0x2122, 0x00);
    memset(p->vram, 0, sizeof p->vram);
    for (int r = 0; r < 32; r++)
        p->vram[r * 32 + 5] = 1;         /* BG1 column 5: tile 1 */
    for (int r = 0; r < 8; r++)
        p->vram[0x1000 + 16 + r] = 0x00FF;   /* tile 1: 4bpp, all color 1 */
    p->vram[0x0400] = 0x2000 | 4 * 8;    /* BG3 row 0, entry 0: BG1 H offset +32 */
    static uint8_t buf[256 * 4 * 2];
    PpuBeginDrawing(p, buf, 256 * 4, 0);
    ppu_runLine(p, 0);
    ppu_runLine(p, 1);   /* screen row 0 */
    ppu_drawTo(p, 256);
    CHECK(buf[8 * 4 + 2] > 0 && buf[15 * 4 + 2] > 0 && buf[8 * 4 + 1] == 0,
          "column 1 takes the offset: BG1 column 5 (R %u)", buf[8 * 4 + 2]);
    CHECK(buf[0 * 4 + 2] == 0 && buf[7 * 4 + 2] == 0, "column 0 keeps no offset");
    CHECK(buf[16 * 4 + 2] == 0, "column 2 has no entry: no offset");
}

int main(void)
{
    th_bus_init();
    test_readonly_writes();
    test_ppu_registers();
    test_dma_to_vram();
    test_wram_port();
    test_apu_ports();
    test_port_write_timing();
    test_spc_timer_phase();
    test_cgram_15bit();
    test_offset_per_tile();
    test_midline_write();
    return th_report("snes_bus");
}
