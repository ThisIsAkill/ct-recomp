/* Frame scheduler (runtime/sched.h) end to end on a WRAM program: VBlank
 * NMI through the ROM vector into a RAM handler, WAI between frames, and a
 * per-line HDMA gradient rendered by the vendored PPU. */
#include "harness.h"
#include "sched.h"
#include "snes_adapter.h"
#include "spc700_host.h"

static void put(uint16_t at, const uint8_t *p, unsigned n)
{
    memcpy(bus_wram() + at, p, n);
}

/* Master clock at the start of each instruction, via the trace hook. */
static uint64_t clk_at[64];
static uint32_t pc_at[64];
static int n_at;

static void clock_trace(const CPU *c, uint32_t at)
{
    (void)c;
    if (n_at < 64) {
        pc_at[n_at] = at;
        clk_at[n_at++] = sched_clock();
    }
}

/* General DMA pauses the CPU (Mesen 2's timing, #33): it starts after the
   CPU's next cycle (the NOP's opcode fetch, 8 clocks from WRAM), syncs to
   8 clocks, then 8 overhead, 8 per channel plus 8 per byte, and re-aligns
   1-6 clocks to the NOP's internal cycle counting every byte; plus each
   line's DRAM refresh the pause runs past. With SNES_QUIRK_MESEN_DMA_COUNT8
   that last count keeps only the low 8 bits of the byte count. */
static void test_dma_timing(unsigned quirks)
{
    static const uint8_t prog[] = {
        0xA9, 0x80, 0x8D, 0x00, 0x43,   /* LDA #$80 / STA $4300: B->A, 1 register */
        0xA9, 0x34, 0x8D, 0x01, 0x43,   /* LDA #$34 / STA $4301: from $2134 */
        0x9C, 0x02, 0x43,               /* STZ $4302 */
        0xA9, 0x30, 0x8D, 0x03, 0x43,   /* LDA #$30 / STA $4303: to $xx3000 */
        0xA9, 0x7E, 0x8D, 0x04, 0x43,   /* LDA #$7E / STA $4304: bank $7E */
        0xA9, 0x40, 0x8D, 0x05, 0x43,   /* LDA #$40 / STA $4305 */
        0xA9, 0x01, 0x8D, 0x06, 0x43,   /* LDA #$01 / STA $4306: $0140 bytes */
        0xA9, 0x01, 0x8D, 0x0B, 0x42,   /* LDA #$01 / STA $420B: go */
        0xEA,                           /* NOP */
        0xCB, 0x80, 0xFD,               /* WAI / BRA */
    };
    static CPU c;
    bus_reset();
    snes_ref_quirks = quirks;
    put(0x2400, prog, sizeof prog);
    interp_reset(&c);
    c.e = 0;
    c.PB = 0x7E;
    c.PC = 0x2400;
    c.DB = 0x00;
    c.m = c.x = 1;
    c.S = 0x01FF;
    sched_init(&c);
    n_at = 0;
    ct_trace_hook = clock_trace;
    sched_run_frame();
    ct_trace_hook = NULL;
    int k;
    for (k = 0; k + 1 < n_at && pc_at[k] != 0x7E2423; k++)
        ;
    CHECK(k + 1 < n_at, "STA $420B traced");
    /* STA abs: 4 cycles (opcode+2 operand fetches from WRAM, 8 each; the
       $420B write, 6). */
    uint64_t d = clk_at[k + 1] - clk_at[k], sta = 3 * 8 + 6;
    uint64_t t = clk_at[k] + sta + 8;   /* the DMA starts after the NOP's fetch */
    unsigned align = 8 - (unsigned)(t & 7);
    unsigned n = align + 8 + 8 + 8 * 0x140, count = align + 8 + 8 + 8 * 0x40;
    if (!quirks)
        count = n;
    uint64_t want = sta + n + 6 - count % 6;
    /* ~2600 clocks: up to two lines' refresh (40 each) fall inside. */
    CHECK(d == want || d == want + 40 || d == want + 80,
          "STA $420B + $140-byte DMA (quirks %u): %llu clocks, want %llu (+40 per refresh)",
          quirks, (unsigned long long)d, (unsigned long long)want);
    snes_ref_quirks = 0;
}


/* Frame edges with the CPU in WAI (NMI off): line 240 of every other
   frame is 4 clocks short (no interlace), and STAT78 bit 7 is that frame's
   field. */
static uint64_t edge_clk[4];
static int edge_field[4];
static void edge_hook(long f)
{
    if (f < 4) {
        edge_clk[f] = sched_clock();
        edge_field[f] = read8(0x213F) >> 7;
    }
}

static void test_short_line(void)
{
    static const uint8_t prog[] = { 0xCB, 0x80, 0xFD };   /* WAI / BRA */
    static CPU c;
    bus_reset();
    put(0x2400, prog, sizeof prog);
    interp_reset(&c);
    c.e = 0;
    c.PB = 0x7E;
    c.PC = 0x2400;
    c.S = 0x01FF;
    sched_init(&c);
    sched_set_frame_hook(edge_hook);
    for (int f = 0; f < 3; f++)
        sched_run_frame();
    sched_set_frame_hook(NULL);
    CHECK(edge_clk[1] == 262 * 1364, "frame 0: %llu clocks", (unsigned long long)edge_clk[1]);
    CHECK(edge_clk[2] - edge_clk[1] == 262 * 1364 - 4, "frame 1 (short line 240): %llu clocks",
          (unsigned long long)(edge_clk[2] - edge_clk[1]));
    CHECK(edge_field[1] == 1 && edge_field[2] == 0, "field bit: %d %d", edge_field[1],
          edge_field[2]);
}

/* A port access after a DRAM refresh inside its own instruction is timed
   after the refresh: the SPC700 is brought up to the access's clock. */
static uint64_t spc_at[64];
static void spc_trace(const CPU *c, uint32_t at)
{
    if (n_at < 64)
        spc_at[n_at] = 2 * spc_host_cycle();
    clock_trace(c, at);
}

static void test_refresh_access_clock(void)
{
    static uint8_t prog[40 * 3 + 3];
    for (int k = 0; k < 40; k++) {   /* STA $2140 x40: 30 clocks each */
        prog[3 * k] = 0x8D;
        prog[3 * k + 1] = 0x40;
        prog[3 * k + 2] = 0x21;
    }
    prog[120] = 0xCB;   /* WAI / BRA */
    prog[121] = 0x80;
    prog[122] = 0xFD;
    static CPU c;
    bus_reset();
    put(0x2400, prog, sizeof prog);
    interp_reset(&c);
    c.e = 0;
    c.PB = 0x7E;
    c.PC = 0x2400;
    c.m = c.x = 1;
    c.S = 0x01FF;
    sched_init(&c);
    n_at = 0;
    ct_trace_hook = spc_trace;
    sched_run_frame();
    ct_trace_hook = NULL;
    double r = 32040.0 * 64 / 21477270.0;   /* SPC700 half-cycles per master clock */
    int straddled = 0;
    for (int k = 0; k + 1 < n_at && pc_at[k + 1] < 0x7E2400 + 120; k++) {
        uint64_t d = clk_at[k + 1] - clk_at[k];
        CHECK(d == 30 || d == 70, "STA $2140 %d: %llu clocks", k, (unsigned long long)d);
        uint64_t m = clk_at[k + 1];   /* the write ends the instruction */
        straddled += d == 70;
        int64_t target = (int64_t)((double)m * r) - 1;
        int64_t hi = target + 1 > 4 ? target + 1 : 4;   /* it starts at 4 (2-cycle reset) */
        CHECK((int64_t)spc_at[k + 1] >= target && (int64_t)spc_at[k + 1] <= hi,
              "STA %d: SPC700 at %llu, want %lld", k, (unsigned long long)spc_at[k + 1],
              (long long)target);
    }
    CHECK(straddled == 1, "one STA runs over the refresh: %d", straddled);
}

int main(void)
{
    th_bus_init();
    test_dma_timing(0);
    test_dma_timing(SNES_QUIRK_MESEN_DMA_COUNT8);
    test_short_line();
    test_refresh_access_clock();

    /* HDMA channel 7, mode 3 (4 bytes to $2121 $2121 $2122 $2122): every
       line sets CGRAM[0] (the backdrop) to red = row & 31. */
    uint8_t *w = bus_wram();
    for (int r = 0; r < SCHED_HEIGHT; r++) {
        uint8_t *e = w + 0x3400 + r * 5;
        e[0] = 1;
        e[1] = e[2] = 0;
        e[3] = (uint8_t)(r & 31);
        e[4] = 0;
    }
    w[0x3400 + SCHED_HEIGHT * 5] = 0;

    static const uint8_t prog[] = {
        0xA9, 0x0F, 0x8D, 0x00, 0x21,   /* LDA #$0F / STA $2100  INIDISP */
        0x9C, 0x2C, 0x21,               /* STZ $212C             TM: backdrop only */
        0xA9, 0x03, 0x8D, 0x70, 0x43,   /* DMAP7 = 3 */
        0xA9, 0x21, 0x8D, 0x71, 0x43,   /* BBAD7 = $21 */
        0xA9, 0x00, 0x8D, 0x72, 0x43,   /* A1T7 = $7E3400 */
        0xA9, 0x34, 0x8D, 0x73, 0x43,
        0xA9, 0x7E, 0x8D, 0x74, 0x43,
        0xA9, 0x80, 0x8D, 0x0C, 0x42,   /* HDMAEN = ch 7 */
        0xA9, 0x80, 0x8D, 0x00, 0x42,   /* NMITIMEN: NMI on */
        0xCB, 0x80, 0xFD,               /* loop: WAI / BRA loop */
    };
    static const uint8_t nmi[] = {
        0xE6, 0x10,                     /* INC $10 */
        0xAD, 0x10, 0x42,               /* LDA $4210 (ack) */
        0x40,                           /* RTI */
    };
    put(0x2000, prog, sizeof prog);
    put(0x0500, nmi, sizeof nmi);       /* ROM NMI stub: JML $000500 */
    w[0x10] = 0;

    static CPU c;
    interp_reset(&c);   /* power-on: also clears a pending WAI */
    c.e = 0;
    c.PB = 0x7E;
    c.PC = 0x2000;
    c.DB = 0x00;
    c.m = c.x = 1;
    c.i = 1;
    c.S = 0x01FF;
    sched_init(&c);
    for (int f = 0; f < 3; f++)
        sched_run_frame();

    CHECK(sched_frame_count() == 3, "frames %ld", sched_frame_count());
    CHECK(sched_nmi_count() == 3, "one NMI per frame: %ld", sched_nmi_count());
    CHECK(w[0x10] == 3, "RAM handler ran 3 times: %u", w[0x10]);
    CHECK(c.PB == 0x7E && c.PC == 0x202C, "waiting after the WAI at $202B: $%02X%04X", c.PB,
          c.PC);

    const uint8_t *fb = sched_frame();
    int bad = 0;
    for (int r = 0; r < SCHED_HEIGHT && bad < 5; r++) {
        int v = r & 31, want = (v << 3) | (v >> 2);
        const uint8_t *px = fb + (size_t)r * SCHED_WIDTH * 4 + 100 * 4;   /* B G R x */
        if (px[2] != want || px[1] || px[0]) {
            CHECK(0, "row %d: RGB %u %u %u, want %d 0 0", r, px[2], px[1], px[0], want);
            bad++;
        }
    }
    CHECK(!bad, "HDMA gradient: row r red = r & 31");

    /* APU catch-up on port access: spin on $2140 until the IPL's $AA,
       then mark $11. Without the sync the loop never ends. */
    static const uint8_t poll[] = {
        0xAD, 0x40, 0x21,               /* $2100 loop: LDA $2140 */
        0xC9, 0xAA,                     /*            CMP #$AA */
        0xD0, 0xF9,                     /*            BNE loop */
        0xE6, 0x11,                     /*            INC $11 */
        0xCB, 0x80, 0xFD,               /* idle: WAI / BRA idle */
    };
    bus_reset();
    put(0x2100, poll, sizeof poll);
    interp_reset(&c);   /* power-on: also clears a pending WAI */
    c.e = 0;
    c.PB = 0x7E;
    c.PC = 0x2100;
    c.DB = 0x00;
    c.m = c.x = 1;
    c.i = 1;
    c.S = 0x01FF;
    sched_init(&c);
    sched_run_frame();
    CHECK(bus_wram()[0x11] == 1, "IPL signature seen through $2140 within one frame");

    /* H/V IRQ (#21) through the ROM vector ($FFEE -> JML $000504). The
       handler acks $4211, latches the counters via $2137, and stores
       OPVCT (low, high) and OPHCT (low, high). */
    static const uint8_t irq_prog[] = {
        0xA9, 0x64, 0x8D, 0x09, 0x42,   /* VTIME = 100 */
        0x9C, 0x0A, 0x42,
        0xA9, 0x64, 0x8D, 0x07, 0x42,   /* HTIME = 100 */
        0x9C, 0x08, 0x42,
        0xAD, 0x00, 0x1F,               /* LDA $1F00: NMITIMEN value */
        0x8D, 0x00, 0x42,
        0x58,                           /* CLI */
        0xCB, 0x80, 0xFD,               /* idle: WAI / BRA idle */
    };
    static const uint8_t irq_handler[] = {
        0xAD, 0x11, 0x42,               /* LDA $4211 (ack) */
        0xE6, 0x20,                     /* INC $20 */
        0xAD, 0x37, 0x21,               /* LDA $2137 (latch) */
        0xAD, 0x3F, 0x21,               /* LDA $213F (reset flip-flops) */
        0xAD, 0x3D, 0x21, 0x85, 0x21,   /* OPVCT low -> $21 */
        0xAD, 0x3D, 0x21, 0x85, 0x22,   /* OPVCT high -> $22 */
        0xAD, 0x3C, 0x21, 0x85, 0x23,   /* OPHCT low -> $23 */
        0xAD, 0x3C, 0x21, 0x85, 0x24,   /* OPHCT high -> $24 */
        0x40,                           /* RTI */
    };
    for (int mode = 0; mode < 2; mode++) {
        bus_reset();
        put(0x2200, irq_prog, sizeof irq_prog);
        put(0x0504, irq_handler, sizeof irq_handler);
        bus_wram()[0x1F00] = mode ? 0x30 : 0x20;   /* HV-IRQ / V-IRQ */
        interp_reset(&c);
        c.e = 0;
        c.PB = 0x7E;
        c.PC = 0x2200;
        c.DB = 0x00;
        c.m = c.x = 1;
        c.S = 0x01FF;
        sched_init(&c);
        for (int f = 0; f < 3; f++)
            sched_run_frame();
        const uint8_t *z = bus_wram();
        /* High reads: bit 8, bits 1-7 are PPU2 open bus (the low byte
           just read from the same counter). */
        unsigned h = (unsigned)(z[0x23] | (z[0x24] & 1) << 8), v = (unsigned)(z[0x21] | (z[0x22] & 1) << 8);
        CHECK((z[0x22] & 0xFE) == (z[0x21] & 0xFE), "OPVCT high: PPU2 open bus bits $%02X",
              z[0x22]);
        CHECK(z[0x20] == 3, "mode %d: one IRQ per frame, got %u", mode, z[0x20]);
        CHECK(v == 100, "mode %d: latched V = VTIME: %u", mode, v);
        /* Latch at the start of LDA $2137: fire clock + 62 (IRQ entry:
           fetch 8, idle 6, 4 pushes, 2 vector reads, 8 each) + 32 (ROM stub
           JML) + 30 (LDA $4211: 3 WRAM fetches, I/O read 6) + 38 (INC $20:
           2 fetches, read, idle 6, write), in dots; in mode 1 (fire at 400)
           the handler also runs past the line's DRAM refresh (531-538), 40
           clocks with the CPU paused. */
        unsigned want = ((mode ? 400u + 40 : 0u) + 62 + 32 + 30 + 38) / 4;
        CHECK(h == want, "mode %d: latched H %u, want %u", mode, h, want);
    }

    /* Auto-joypad (#22): sampled at VBlank start when NMITIMEN bit 0 is
       set; nothing before that. */
    bus_reset();
    static const uint8_t idle[] = {0xCB, 0x80, 0xFD};
    put(0x2300, idle, sizeof idle);
    interp_reset(&c);
    c.e = 0;
    c.PB = 0x7E;
    c.PC = 0x2300;
    c.DB = 0x00;
    c.S = 0x01FF;
    sched_init(&c);
    sched_set_joypad(0, 0x8010);   /* B + R */
    sched_set_joypad(1, 0x0800);   /* Up on port 2 */
    sched_run_frame();
    CHECK(read8(0x4218) == 0 && read8(0x4219) == 0, "no auto-read while disabled");
    write8(0x4200, 0x01);
    sched_run_frame();
    CHECK(read8(0x4218) == 0x10 && read8(0x4219) == 0x80, "JOY1: $%02X%02X", read8(0x4219),
          read8(0x4218));
    CHECK(read8(0x421A) == 0x00 && read8(0x421B) == 0x08, "JOY2: $%02X%02X", read8(0x421B),
          read8(0x421A));
    return th_report("sched");
}
