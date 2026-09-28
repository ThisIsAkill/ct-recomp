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
static uint64_t clk_at[256];
static uint32_t pc_at[256];
static int n_at;

static void clock_trace(const CPU *c, uint32_t at)
{
    (void)c;
    if (n_at < 256) {
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
    CHECK(k + 2 < n_at, "NOP after STA $420B traced");
    /* The pause lands inside the NOP (fetch 8, then the DMA, then its
       internal cycle 6): measure from the STA to the instruction after. */
    uint64_t d = clk_at[k + 2] - clk_at[k], sta = 3 * 8 + 6 + 8 + 6;
    uint64_t t = clk_at[k] + 3 * 8 + 6 + 8;   /* the DMA starts after the NOP's fetch */
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
        edge_clk[f] = sched_frame_clock();
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
static uint64_t spc_at[256];
static void spc_trace(const CPU *c, uint32_t at)
{
    if (n_at < 256)
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


/* HDMA pauses the CPU once per visible line (Mesen 2's timing, #33): one
   channel, mode 3 (4 bytes), direct: sync to 8 clocks (2-8), 8 overhead,
   32 for the bytes, 8 for the next line counter, then 1-N clocks back to
   the CPU cycle (N its clocks). In a loop of NOPs from WRAM (14 clocks:
   fetch 8, internal 6) exactly one NOP per line is stretched by that,
   and at line 0 one more by the frame's HDMA init. */
static void test_hdma_time(void)
{
    uint8_t *w = bus_wram();
    for (int r = 0; r < 40; r++) {   /* line counter 1, 4 data bytes */
        uint8_t *e = w + 0x3400 + r * 5;
        e[0] = 1;
        e[1] = e[2] = e[3] = e[4] = 0;
    }
    static uint8_t prog[120];
    static const uint8_t setup[] = {
        0xA9, 0x03, 0x8D, 0x70, 0x43,   /* DMAP7 = 3 */
        0xA9, 0x21, 0x8D, 0x71, 0x43,   /* BBAD7 = $21 ($2121/$2122) */
        0xA9, 0x00, 0x8D, 0x72, 0x43,   /* A1T7 = $7E3400 */
        0xA9, 0x34, 0x8D, 0x73, 0x43,
        0xA9, 0x7E, 0x8D, 0x74, 0x43,
        0xA9, 0x80, 0x8D, 0x0C, 0x42,   /* HDMAEN = ch 7 */
    };
    memcpy(prog, setup, sizeof setup);
    memset(prog + sizeof setup, 0xEA, sizeof prog - sizeof setup - 2);   /* NOPs */
    prog[sizeof prog - 2] = 0x80;   /* BRA back to the NOPs */
    prog[sizeof prog - 1] = (uint8_t)(sizeof setup - sizeof prog);
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
    sched_run_frame();   /* the HDMA init at line 0 of the next frame */
    n_at = 0;
    ct_trace_hook = clock_trace;
    sched_run_frame();   /* traced: its first 256 instructions, past line 0's HDMA */
    ct_trace_hook = NULL;
    int stretched = 0, inits = 0;
    for (int k = 0; k + 1 < n_at; k++) {
        uint64_t d = clk_at[k + 1] - clk_at[k];
        if (pc_at[k] < 0x7E2400 + sizeof setup || pc_at[k] == 0x7E2400 + sizeof prog - 2 ||
            d == 14 || d == 14 + 40)
            continue;   /* setup, the BRA, a plain NOP, or one with the refresh */
        unsigned s = (unsigned)(d - 14);
        if (s <= 8 + 8 + 8 + 6) {   /* the frame's HDMA init: 8 + 8 for the counter */
            CHECK(s >= 2 + 8 + 8 + 1, "NOP at $%06X stretched by %u (init 19-30)", pc_at[k], s);
            inits++;
            continue;
        }
        CHECK(s >= 2 + 8 + 32 + 8 + 1 && s <= 8 + 8 + 32 + 8 + 6,
              "NOP at $%06X stretched by %u (HDMA 51-62)", pc_at[k], s);
        stretched++;
    }
    /* The trace spans the first lines of the frame: one HDMA per line whose
       dot 276 it passes. */
    int lines = 0;
    uint64_t f0 = 262ull * 1364;   /* frame 1 starts here (frame 0 is full length) */
    for (int l = 0; l < 8; l++)
        if (f0 + (uint64_t)l * 1364 + 1104 < clk_at[n_at - 1])
            lines++;
    CHECK(inits == 1 && stretched == lines, "HDMA init %d, line HDMA %d, lines %d", inits,
          stretched, lines);
}


/* Runs a WRAM program from reset state (native mode, M=X=1). */
static CPU tc;
static void start_prog(const uint8_t *prog, unsigned n)
{
    bus_reset();
    put(0x2400, prog, n);
    interp_reset(&tc);
    tc.e = 0;
    tc.PB = 0x7E;
    tc.PC = 0x2400;
    tc.DB = 0x00;
    tc.m = tc.x = 1;
    tc.S = 0x01FF;
    sched_init(&tc);
}

/* The frame hook sees WRAM as of the frame edge: writes an instruction
   makes after the edge (inside it) are not in it yet. */
static unsigned wr_before, wr_total;
static uint64_t edge_at;
static int hooked_value;
static void count_write(uint32_t off)
{
    if (off != 0x10)
        return;
    wr_total++;
    if (edge_at && snes_access_clock(0) < edge_at)
        wr_before++;
}
static void edge_value(long f)
{
    if (f == 2 && hooked_value < 0) {
        hooked_value = bus_wram()[0x10];
        (void)f;
    }
}

static void test_frame_edge_wram(void)
{
    static const uint8_t prog[] = {0xE6, 0x10, 0x80, 0xFC};   /* INC $10 / BRA */
    start_prog(prog, sizeof prog);
    sched_run_frame();                    /* frame 1 */
    edge_at = sched_frame_clock() + 262ull * 1364 - 4;   /* the next edge (frame 1 is odd: short line) */
    wr_before = wr_total = 0;
    hooked_value = -1;
    uint8_t v0 = bus_wram()[0x10];
    ct_wram_write_hook = count_write;
    sched_set_frame_hook(edge_value);
    sched_run_frame();
    sched_set_frame_hook(NULL);
    ct_wram_write_hook = NULL;
    CHECK(sched_frame_clock() == edge_at, "edge at %llu, want %llu",
          (unsigned long long)sched_frame_clock(), (unsigned long long)edge_at);
    CHECK(hooked_value == (uint8_t)(v0 + wr_before), "hook sees $%02X, want $%02X (%u of %u writes before the edge)",
          hooked_value, (uint8_t)(v0 + wr_before), wr_before, wr_total);
}

/* HVBJOY H-blank and RDNMI are read at the read's own clock (a read
   samples 4 clocks before the end of its cycle). */
static uint64_t nmi_prev, nmi_last, nmi_after;
static void nmi_trace(const CPU *c, uint32_t at)
{
    (void)c;
    if (at == 0x7E2407) {
        nmi_prev = nmi_last;
        nmi_last = sched_clock();
    } else if (at == 0x7E240C && !nmi_after) {
        nmi_after = sched_clock();
    }
}
static void test_hblank_rdnmi_clock(void)
{
    /* L: LDA $4212 / AND #$40 / BEQ L / LDA $4210 / BPL -5 / LDA $4210 / STA $12 / BRA . */
    static const uint8_t prog[] = {
        0xAD, 0x12, 0x42, 0x29, 0x40, 0xF0, 0xF9,
        0xAD, 0x10, 0x42, 0x10, 0xFB,
        0xAD, 0x10, 0x42, 0x85, 0x12,
        0x80, 0xFE,
    };
    start_prog(prog, sizeof prog);
    n_at = 0;
    ct_trace_hook = clock_trace;
    sched_run_frame();
    ct_trace_hook = NULL;
    /* LDA abs from WRAM: 3 fetches (8), the I/O read (6): sampled at +26. */
    int last = -1, prev = -1;
    for (int k = 0; k < n_at && pc_at[k] != 0x7E2407; k++)
        if (pc_at[k] == 0x7E2400) {
            prev = last;
            last = k;
        }
    CHECK(last > 0 && prev >= 0, "H-blank loop traced");
    unsigned h1 = (unsigned)(clk_at[last] + 26), h0 = (unsigned)(clk_at[prev] + 26);
    CHECK(h1 > SCHED_HBLANK_CLOCK && h0 <= SCHED_HBLANK_CLOCK,
          "H-blank seen from the read at clock %u (the one before: %u)", h1, h0);

    /* The NMI flag rises at clock 2 of line 225; the loop leaves on the
       first read at or after it. That read clears it unless it came in the
       4 clocks after it rose; the next read then still sees it. */
    start_prog(prog, sizeof prog);
    nmi_prev = nmi_last = nmi_after = 0;
    ct_trace_hook = nmi_trace;
    sched_run_frame();
    ct_trace_hook = NULL;
    uint64_t rise = 225ull * 1364 + 2;   /* frame 0 */
    uint64_t s_last = nmi_last + 26, s_prev = nmi_prev + 26;
    CHECK(nmi_after && s_last >= rise && s_prev < rise,
          "NMI flag seen by the read at line clock %lld (the one before: %lld)",
          (long long)(s_last - rise + 2), (long long)(s_prev - rise + 2));
    int held = s_last < rise + 4;
    CHECK(((bus_wram()[0x12] >> 7) & 1) == held, "read at clock %lld: flag %s, next read $%02X",
          (long long)(s_last - rise + 2), held ? "held" : "cleared", bus_wram()[0x12]);
}

/* Auto-joypad busy (Mesen 2): step 0 at the first multiple of 256 master
   clocks after clock 130 of line 225, minus 128; busy from step 1 to step
   34 (128 clocks each). */
static uint64_t aj_prev[2], aj_last[2];
static int aj_done[2];

static void aj_trace(const CPU *c, uint32_t at)
{
    (void)c;
    int k = at == 0x7E2405 ? 0 : at == 0x7E240C ? 1 : -1;
    if (at == 0x7E240C)
        aj_done[0] = 1;
    if (at == 0x7E2413)
        aj_done[1] = 1;
    if (k >= 0 && !aj_done[k]) {
        aj_prev[k] = aj_last[k];
        aj_last[k] = sched_clock();
    }
}

static void test_autojoy_clock(void)
{
    /* LDA #1 / STA $4200 / L1: LDA $4212 / AND #1 / BEQ L1 /
       L2: LDA $4212 / AND #1 / BNE L2 / BRA . */
    static const uint8_t prog[] = {
        0xA9, 0x01, 0x8D, 0x00, 0x42,
        0xAD, 0x12, 0x42, 0x29, 0x01, 0xF0, 0xF9,
        0xAD, 0x12, 0x42, 0x29, 0x01, 0xD0, 0xF9,
        0x80, 0xFE,
    };
    start_prog(prog, sizeof prog);
    memset(aj_done, 0, sizeof aj_done);
    memset(aj_last, 0, sizeof aj_last);
    ct_trace_hook = aj_trace;
    sched_run_frame();
    sched_run_frame();
    ct_trace_hook = NULL;
    uint64_t step0 = ((225ull * 1364 + 130 + 255) & ~255ull) - 128;
    uint64_t set = step0 + 128, clear = step0 + 34 * 128;
    /* LDA abs from WRAM: the I/O read is sampled at +26. */
    CHECK(aj_done[0] && aj_last[0] + 26 >= set && aj_prev[0] + 26 < set,
          "busy seen from the read at %lld (set at %lld)", (long long)(aj_last[0] + 26),
          (long long)set);
    CHECK(aj_done[1] && aj_last[1] + 26 >= clear && aj_prev[1] + 26 < clear,
          "busy gone at the read at %lld (cleared at %lld)", (long long)(aj_last[1] + 26),
          (long long)clear);
}

/* A general DMA longer than a frame: the frame edge inside it is flagged
   (its bytes all landed at its start), the next one isn't. */
static int in_dma[2], n_edges;

static void dma_edge_hook(long frame)
{
    (void)frame;
    if (n_edges < 2)
        in_dma[n_edges++] = sched_frame_in_dma();
}

static void test_frame_in_dma(void)
{
    /* DMA ch0: WRAM $7E:0000 -> $2118, 65536 bytes; then BRA . */
    static const uint8_t prog[] = {
        0x9C, 0x00, 0x43, 0xA9, 0x18, 0x8D, 0x01, 0x43, 0x9C, 0x02, 0x43, 0x9C, 0x03, 0x43,
        0xA9, 0x7E, 0x8D, 0x04, 0x43, 0x9C, 0x05, 0x43, 0x9C, 0x06, 0x43,
        0xA9, 0x01, 0x8D, 0x0B, 0x42, 0x80, 0xFE,
    };
    start_prog(prog, sizeof prog);
    n_edges = 0;
    sched_set_frame_hook(dma_edge_hook);
    sched_run_frame();
    sched_run_frame();
    sched_set_frame_hook(NULL);
    CHECK(n_edges == 2 && in_dma[0] && !in_dma[1], "edges in the DMA: %d %d", in_dma[0],
          in_dma[1]);
}

/* The reset button: asked at VBlank with the frame about to begin; the
   frame ends there, the CPU takes the reset vector in emulation mode with
   A and the low bytes of X/Y/S kept, WRAM is kept, and the clock restarts
   from 0 at line 0 (then the reset sequence's 186 clocks). */
static long reset_asked[4];
static int n_asked;

static int reset_at_3(long frame)
{
    if (n_asked < 4)
        reset_asked[n_asked++] = frame;
    return frame == 3;
}

static void test_soft_reset(void)
{
    /* REP #$30 / LDA #$1234 / LDX #$5678 / STA $10 / BRA . */
    static const uint8_t prog[] = {0xC2, 0x30, 0xA9, 0x34, 0x12, 0xA2, 0x78, 0x56,
                                   0x85, 0x10, 0x80, 0xFE};
    start_prog(prog, sizeof prog);
    n_asked = 0;
    sched_set_reset_hook(reset_at_3);
    long a = sched_run_frame(), b = sched_run_frame();
    long c = sched_run_frame();
    sched_set_reset_hook(NULL);
    CHECK(a == 1 && b == 1 && c == 1 && sched_frame_count() == 3, "frames %ld %ld %ld, count %ld",
          a, b, c, sched_frame_count());
    CHECK(n_asked == 3 && reset_asked[0] == 1 && reset_asked[2] == 3, "asked for frames 1, 2, 3");
    uint16_t vec = (uint16_t)(read8(0xFFFC) | read8(0xFFFD) << 8);
    CHECK(tc.e && tc.m && tc.x && tc.i && tc.PB == 0 && tc.PC == vec, "at the reset vector $%04X",
          tc.PC);
    CHECK(tc.A == 0x1234 && tc.X == 0x0078 && tc.S == 0x01FF, "A %04X X %04X S %04X kept",
          tc.A, tc.X, tc.S);
    CHECK(bus_wram()[0x10] == 0x34 && bus_wram()[0x11] == 0x12, "WRAM kept");
    CHECK(sched_clock() == 0 && sched_line() == 0, "clock restarts: %llu, line %d",
          (unsigned long long)sched_clock(), sched_line());
}

int main(void)
{
    th_bus_init();
    test_dma_timing(0);
    test_dma_timing(SNES_QUIRK_MESEN_DMA_COUNT8);
    test_short_line();
    test_refresh_access_clock();
    test_hdma_time();
    test_frame_edge_wram();
    test_hblank_rdnmi_clock();
    test_autojoy_clock();
    test_frame_in_dma();
    test_soft_reset();

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
    for (int f = 0; f < 2; f++)
        sched_run_frame();
    uint64_t f2 = sched_frame_clock();
    n_at = 0;
    ct_trace_hook = clock_trace;
    sched_run_frame();
    ct_trace_hook = NULL;

    CHECK(sched_frame_count() == 3, "frames %ld", sched_frame_count());
    CHECK(sched_nmi_count() == 3, "one NMI per frame: %ld", sched_nmi_count());
    /* The NMI is raised at clock 6 of line 225; in WAI the first idle cycle
       starting at or after that sees it, one more idle cycle follows, then
       the entry (62 clocks to the ROM stub's first instruction). */
    {
        uint64_t ls = f2 + 225ull * 1364;
        int k;
        for (k = 0; k < n_at && pc_at[k] >> 16 != 0x00; k++)
            ;
        CHECK(k < n_at, "NMI entry traced");
        uint64_t entry = clk_at[k] - 62;
        CHECK(entry >= ls + 6 + 12 && entry < ls + 6 + 18, "NMI entry at line 225 clock %llu, want 18-23",
              (unsigned long long)(entry - ls));
    }
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
        for (int f = 0; f < 2; f++)
            sched_run_frame();
        n_at = 0;
        ct_trace_hook = clock_trace;   /* frame 3: the IRQ handler's instructions */
        sched_run_frame();
        ct_trace_hook = NULL;
        const uint8_t *z = bus_wram();
        /* High reads: bit 8, bits 1-7 are PPU2 open bus (the low byte
           just read from the same counter). */
        unsigned h = (unsigned)(z[0x23] | (z[0x24] & 1) << 8), v = (unsigned)(z[0x21] | (z[0x22] & 1) << 8);
        CHECK((z[0x22] & 0xFE) == (z[0x21] & 0xFE), "OPVCT high: PPU2 open bus bits $%02X",
              z[0x22]);
        CHECK(z[0x20] == 3, "mode %d: one IRQ per frame, got %u", mode, z[0x20]);
        CHECK(v == 100, "mode %d: latched V = VTIME: %u", mode, v);
        /* The IRQ line rises at clock 14 of line VTIME (V only), or at
           18 + 4 * HTIME (H and V) (Mesen 2). The CPU waits in WAI in
           6-clock idle cycles: the first cycle starting at or after that
           sees the line, one more idle cycle follows, then the entry (fetch
           8, idle 6, 4 pushes and 2 vector reads, 8 each: 62 clocks). */
        uint64_t ls = sched_frame_clock() - 262ull * 1364 + 100ull * 1364;   /* frame 2 (even: full length) */
        unsigned rise = mode ? 18 + 4 * 100 : 14;
        int k;
        for (k = 0; k < n_at && pc_at[k] >> 16 != 0x00; k++)
            ;   /* the vector's ROM stub: the first instruction after the entry */
        CHECK(k < n_at, "mode %d: IRQ entry traced", mode);
        uint64_t entry = clk_at[k] - 62;
        CHECK(entry >= ls + rise + 12 && entry < ls + rise + 18,
              "mode %d: IRQ entry at line clock %llu, want %u-%u", mode,
              (unsigned long long)(entry - ls), rise + 12, rise + 17);
        /* The latch samples H at the LDA $2137 read (3 WRAM fetches, then
           the I/O read, sampled 4 clocks before its end), after the line's
           DRAM refresh if that came first. */
        for (k = 0; k < n_at && pc_at[k] != 0x000509; k++)
            ;
        CHECK(k < n_at, "mode %d: LDA $2137 traced", mode);
        uint64_t sample = clk_at[k] + 3 * 8 + 6 - 4;
        uint64_t refresh = ls + 538 - (ls & 7);
        if (clk_at[k] < refresh && refresh <= sample)
            sample += 40;
        unsigned want = (unsigned)(sample - ls) / 4;
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
