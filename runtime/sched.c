#include "sched.h"

#include <string.h>
#include <stdio.h>

#include <stdlib.h>

#include "apu.h"
#include "bus.h"
#include "cycles.h"
#include "dma.h"
#include "func_table.h"
#include "interp.h"
#include "ppu.h"
#include "snes_adapter.h"

static CPU *cpu;
static uint8_t fb[SCHED_WIDTH * SCHED_HEIGHT * 4];        /* PPU draws here */
static uint8_t present[SCHED_WIDTH * SCHED_HEIGHT * 4];   /* copied at VBlank */
static int line;
static unsigned hclock;         /* master clocks into the current line */
static uint64_t line_start;     /* master clocks since sched_init at line start */
static long frames, nmis;
/* Pending transfers and interrupt lines (see the cycle-level timing
   section): DMA/HDMA/init start one CPU cycle after their event; NMI and
   IRQ are sampled at each CPU cycle start, as Mesen 2 does. */
static struct {
    int delay, hdma, init, dma;
    unsigned hdma_cost, init_cost;
    int dma_after;                /* set the DMA pending after this bus access */
    uint32_t dma_sizes[8];
    int nmi_count;                /* cycle starts until the NMI is seen */
    int need_nmi;                 /* NMI seen: taken at the instruction's end */
    int irq_line;                 /* the H/V IRQ line (until $4211 is read) */
    int prev_irq;                 /* the IRQ line (and I clear) at the last cycle start */
    int nmi_after;                /* arm the NMI (2 cycle starts) after this bus access */
} pend;
static int nmi_done;             /* line 225: the NMI point passed */
static int insn_i;               /* the I flag before the instruction being charged */
static int take_irq;             /* the last instruction ended with an IRQ to take */
static int wai_over;             /* WAI: an interrupt ended it; one more idle cycle */
static int skip_check;           /* just entered an interrupt: run one instruction first */
static int walked;               /* advance is committing a walk (events handled) */
static int edge_undo_from = -1;  /* WRAM writes from this bus access on came after the edge */

static uint8_t nmitimen;        /* $4200 */
static uint8_t rdnmi;           /* $4210 bit 7: NMI flag, cleared by read */
static int rdnmi_set;            /* this frame's flag was set */
static int rdnmi_cleared;        /* line 0: the flag was cleared */
static int in_vblank;
static int autojoy_busy;
static uint8_t wrio;            /* $4201; bit 7 high->low latches H/V */
static uint16_t htime, vtime;   /* $4207-$420A, 9 bits each */
static uint8_t timeup;          /* $4211 bit 7: IRQ flag, the IRQ line */
static uint16_t lat_h, lat_v;   /* latched H/V counters */
static int lat_flag;            /* $213F bit 6 */
static int ophct_hi, opvct_hi;  /* $213C/$213D read flip-flops */
static uint16_t pad[4];         /* current buttons per port (sched_set_joypad) */
static uint16_t joy[4];         /* $4218-$421F: last auto-read result */

static void apu_sync(unsigned early);
static uint64_t access_clock(unsigned early);
static void dma_start(const uint32_t sizes[8]);
static void charge(unsigned clocks);
static unsigned line_clocks(void);
static void walk_reset(void);
static void native_build(void);
static void tick(CPU *c, uint32_t at, uint8_t op);

static int irq_at;               /* this line's H/V timer clock, or -1 */
static int irq_done, hblank_done;
/* DRAM refresh: once per line the CPU (and any DMA) is paused for 40
   master clocks (fullsnes CPU Clock Notes), starting at clock
   530 + 8 - (line start & 7) (bsnes CPU revision 2, scanline()). */
#define REFRESH_CLOCKS 40
static unsigned refresh_at;
static int refresh_done;
static int refresh_paid;         /* refreshes a cycle walk already charged */
static int init_done;            /* line 0: the HDMA init point passed */
static int need_start;           /* line 0 not started yet (sched_init) */
static unsigned start_delay;     /* clocks before the first instruction */
static int frame_done;
static uint64_t frame_clock;       /* line 0's start */
static void (*frame_hook)(long frame);
/* Interrupts entered minus RTIs executed, native or interpreted. */
static long int_depth;
/* Native coverage profile state (see sched_profile_report). */
#define PROF_SLOTS 8192   /* power of two */
#define PROF_DEPTH 256
static uint64_t prof_native, prof_interp;
static struct {
    uint32_t key;   /* PB:PC | M << 24 | X << 25 | E << 26 | 1 << 31 (used) */
    uint64_t count;
} prof[PROF_SLOTS];
static uint32_t shadow[PROF_DEPTH];
static int shadow_n;

/* ---- registers ---- */

static void nmitimen_write(uint16_t reg, uint8_t v)
{
    (void)reg;
    /* Enabling NMI while the VBlank NMI flag is still set fires it, two
       CPU cycle starts after the write (Mesen 2). */
    if (!(nmitimen & 0x80) && (v & 0x80) && (rdnmi & 0x80)) {
        if (cyc_in_progress())
            pend.nmi_after = (int)ct_bus_n - 1;
        else
            pend.nmi_count = 2;
    }
    if (!(v & 0x30))
        timeup = pend.irq_line = 0;   /* disabling H/V IRQs drops a pending one */
    nmitimen = v;
}

/* ---- H/V counters ---- */

/* Line and clock in it of the CPU access being made (a read's sample
   point, a write's end), which can fall past the line the instruction
   started in. */
static void access_hv(int *ln, unsigned *h)
{
    uint64_t at = access_clock(4);
    unsigned len = line_clocks();
    *ln = line;
    *h = at >= line_start ? (unsigned)(at - line_start) : 0;
    if (*h >= len) {
        *h -= len;
        *ln = line + 1 == SCHED_LINES ? 0 : line + 1;
    }
}

/* PPU dot of the CPU access being made (Mesen 2 SnesPpu::GetCycle: dots
   323 and 327 are 6 clocks), past the end of the line when it falls in the
   next one. */
static int ppu_dot(void)
{
    uint64_t at = access_clock(0);
    unsigned h = at >= line_start ? (unsigned)(at - line_start) : 0;
    if (h >= line_clocks())
        return 400;
    return (int)(h <= 1292 ? h >> 2 : h <= 1310 ? (h - 2) >> 2 : (h - 4) >> 2);
}

static void latch_counters(void)
{
    int ln;
    unsigned h;
    access_hv(&ln, &h);
    lat_h = (uint16_t)(h / 4 > 339 ? 339 : h / 4);
    lat_v = (uint16_t)ln;
    lat_flag = 1;
}

/* SLHV returns CPU open bus. OPHCT/OPVCT: low byte, then bit 8 with the
   other bits PPU2 open bus. STAT78: bit 7 the field (toggles every frame,
   set on odd frames), bit 5 PPU2 open bus, NTSC, PPU2 version 3.
   (fullsnes; bsnes readIO) */
static uint8_t slhv_read(uint16_t reg)
{
    if (wrio & 0x80)
        latch_counters();
    return bus_open_bus(reg);
}

static uint8_t counter_read(uint16_t v9, int *hi)
{
    uint8_t v = *hi ? (uint8_t)((snes_ppu2_mdr() & 0xFE) | (v9 >> 8 & 1)) : (uint8_t)v9;
    *hi ^= 1;
    snes_set_ppu2_mdr(v);
    return v;
}

static uint8_t ophct_read(uint16_t reg)
{
    (void)reg;
    return counter_read(lat_h, &ophct_hi);
}

static uint8_t opvct_read(uint16_t reg)
{
    (void)reg;
    return counter_read(lat_v, &opvct_hi);
}

static uint8_t stat78_read(uint16_t reg)
{
    (void)reg;
    uint8_t v = (uint8_t)((frames & 1) << 7 | lat_flag << 6 | (snes_ppu2_mdr() & 0x20) | 3);
    lat_flag = 0;
    ophct_hi = opvct_hi = 0;
    snes_set_ppu2_mdr(v);
    return v;
}

/* RDIO: the I/O port pins; nothing is connected, so they read back what
   WRIO drives (bsnes: io.pio). */
static uint8_t rdio_read(uint16_t reg)
{
    (void)reg;
    return wrio;
}

/* WRIO: only the bit 7 counter latch is modeled (no I/O port devices). */
static void wrio_write(uint16_t reg, uint8_t v)
{
    (void)reg;
    if ((wrio & 0x80) && !(v & 0x80))
        latch_counters();
    wrio = v;
}

static void htime_vtime_write(uint16_t reg, uint8_t v)
{
    uint16_t *t = reg < 0x4209 ? &htime : &vtime;
    if (reg & 1)
        *t = (uint16_t)((*t & 0x100) | v);          /* $4207/$4209: low */
    else
        *t = (uint16_t)((*t & 0xFF) | (v & 1) << 8); /* $4208/$420A: bit 8 */
}

/* $4218-$421F JOY1L..JOY4H: 16-bit auto-read results, low byte first
   (L: A X L R 0 0 0 0, H: B Y Select Start Up Down Left Right). */
static uint8_t joy_read(uint16_t reg)
{
    uint16_t v = joy[(reg - 0x4218) >> 1];
    return (uint8_t)(reg & 1 ? v >> 8 : v);
}

void sched_set_joypad(int port, uint16_t buttons)
{
    if (port >= 0 && port < 4)
        pad[port] = buttons;
}

/* RDNMI: the VBlank NMI flag is set at clock 2 of line 225 and cleared at
   clock 2 of line 0 (Mesen 2), at the read's own clock, which can be past
   the line its instruction started in. Reading clears it, except in the
   4 clocks after it is set. */
static uint8_t rdnmi_read(uint16_t reg)
{
    (void)reg;
    int ln;
    unsigned h;
    access_hv(&ln, &h);
    if (ln == SCHED_VBLANK_LINE && h >= 2 && !rdnmi_set) {
        rdnmi = 0x80;
        rdnmi_set = 1;
    }
    if (ln == 0 && h >= 2 && line != 0)
        rdnmi = 0;
    uint8_t v = (uint8_t)(rdnmi | 0x02 | (bus_mdr & 0x70));   /* CPU version 2; 4-6 open bus */
    if (!(ln == SCHED_VBLANK_LINE && h < 6))
        rdnmi = 0;   /* held set for its first 4 clocks */
    return v;
}

static uint8_t timeup_read(uint16_t reg)
{
    (void)reg;
    uint8_t v = timeup;
    timeup = pend.irq_line = 0;
    return v;
}

/* HVBJOY at the read's own clock (Mesen 2): V-blank from line 225 on,
   H-blank outside clocks 4-1096 of the line; bits 1-5 are open bus. */
static uint8_t hvbjoy_read(uint16_t reg)
{
    (void)reg;
    int ln;
    unsigned h;
    access_hv(&ln, &h);
    int vb = ln >= SCHED_VBLANK_LINE;
    int hb = h < 4 || h > SCHED_HBLANK_CLOCK;
    return (uint8_t)(vb << 7 | hb << 6 | (bus_mdr & 0x3E) | autojoy_busy);
}

/* DSP output at its own rate, one stereo pair per 32 SPC700 cycles, kept
   until taken. A full ring drops the oldest samples. */
#define AUDIO_RING 8192
static int16_t ring[AUDIO_RING][2];
static unsigned ring_head, ring_len;

static void dsp_out(int16_t l, int16_t r)
{
    ring[ring_head][0] = l;
    ring[ring_head][1] = r;
    ring_head = (ring_head + 1) % AUDIO_RING;
    if (ring_len < AUDIO_RING)
        ring_len++;
}

int sched_audio_take(int16_t *stereo, int max)
{
    int n = (int)ring_len < max ? (int)ring_len : max;
    unsigned at = (ring_head + AUDIO_RING - ring_len) % AUDIO_RING;
    for (int k = 0; k < n; k++) {
        stereo[2 * k] = ring[at][0];
        stereo[2 * k + 1] = ring[at][1];
        at = (at + 1) % AUDIO_RING;
    }
    ring_len -= (unsigned)n;
    return n;
}

void sched_init(CPU *c)
{
    cpu = c;
    native_build();
    ct_tick_hook = tick;
    line = 0;
    hclock = 0;
    need_start = 1;
    /* From power-on (emulation mode), the reset sequence runs before the
       first instruction (bsnes CPU::main resetPending): 22 x 6 clocks,
       then the interrupt sequence: a fetch at the old PB:PC, an internal
       cycle, 3 stack cycles and the 2 vector reads at their speeds. 186
       clocks with PC=0 and SlowROM. */
    start_delay = c->e ? 132 + bus_access_clocks(0) + 6 + 3 * bus_access_clocks(0x0100) +
                             bus_access_clocks(0xFFFC) + bus_access_clocks(0xFFFD)
                       : 0;
    frame_done = 0;
    frames = nmis = 0;
    int_depth = 0;
    prof_native = prof_interp = 0;
    shadow_n = 0;
    memset(prof, 0, sizeof prof);
    nmi_done = 1;
    take_irq = wai_over = skip_check = walked = 0;
    insn_i = 1;
    line_start = frame_clock = 0;
    snes_apu_sync = apu_sync;
    snes_master_clock = sched_clock;
    snes_access_clock = access_clock;
    snes_ppu_dot = ppu_dot;
    snes_dma_start = dma_start;
    walk_reset();
    dsp_output_hook = dsp_out;
    ring_head = ring_len = 0;
    nmitimen = rdnmi = 0;
    in_vblank = autojoy_busy = 0;
    wrio = 0xFF;
    htime = vtime = 0x1FF;
    timeup = 0;
    lat_h = lat_v = 0;
    lat_flag = ophct_hi = opvct_hi = 0;
    memset(pad, 0, sizeof pad);
    memset(joy, 0, sizeof joy);
    memset(fb, 0, sizeof fb);
    memset(present, 0, sizeof present);
    bus_hook(0x4200, bus_open_bus, nmitimen_write);   /* write-only: open bus */
    bus_hook(0x4201, bus_open_bus, wrio_write);
    for (uint16_t r = 0x4207; r <= 0x420A; r++)
        bus_hook(r, bus_open_bus, htime_vtime_write);
    bus_hook(0x4210, rdnmi_read, bus_readonly_write);
    bus_hook(0x4211, timeup_read, bus_readonly_write);
    bus_hook(0x4212, hvbjoy_read, bus_readonly_write);
    for (uint16_t r = 0x4218; r <= 0x421F; r++)
        bus_hook(r, joy_read, bus_readonly_write);
    bus_hook(0x4213, rdio_read, bus_readonly_write);
    bus_hook(0x2137, slhv_read, bus_readonly_write);
    bus_hook(0x213C, ophct_read, bus_readonly_write);
    bus_hook(0x213D, opvct_read, bus_readonly_write);
    bus_hook(0x213F, stat78_read, bus_readonly_write);
}

/* ---- APU ----
   The SPC700 runs at 1.024 MHz against the 21.477272 MHz master clock. It
   is caught up to CPU time on every $2140-$2143 access (instruction-start
   time) and at the end of each line, so the upload handshake sees the
   driver respond at a plausible pace. Deterministic. */

static void apu_sync(unsigned early)
{
    snes_apu_catch_up(access_clock(early));
}

/* ---- frame ---- */

static void start_line(void)
{
    Ppu *ppu = snes_hw_ppu();
    Dma *dma = snes_hw_dma();
    if (line == 0) {
        in_vblank = 0;
        rdnmi_set = rdnmi_cleared = 0;
        PpuBeginDrawing(ppu, fb, SCHED_WIDTH * 4, 0);
        dma_initHdma(dma);
    }
    if (line == SCHED_VBLANK_LINE) {
        ppu_drawTo(ppu, 256);   /* the last line's rest */
        memcpy(present, fb, sizeof present);   /* rows 0-223 are final */
        in_vblank = 1;
        snes_oam_vblank_reload();
        autojoy_busy = nmitimen & 1;
        if (nmitimen & 1)
            memcpy(joy, pad, sizeof joy);   /* auto-read (results readable at once) */
    }
    if (line == SCHED_VBLANK_LINE + 3)
        autojoy_busy = 0;
    if (line <= SCHED_HEIGHT)
        ppu_runLine(ppu, line);   /* line L draws row L-1 */
}

/* Master clock in line `ln` where the H/V timer raises the IRQ line, or
   -1 (Mesen 2 InternalRegisters): the H/V counters tick every 4 clocks at
   2 mod 4; the V counter changes at 6 (reset to 0 at 2 on line 0), the H
   counter restarts at 6 and 10 and counts from 14; a match raises TIMEUP
   4 clocks later and the CPU's IRQ line 4 after that. So V only: 14 on
   line VTIME (10 on line 0); H: 18 + 4 * HTIME (on line VTIME for H+V).
   HTIME 337-339 would match at or after the end of the line (into the
   next one): not modeled, fails loudly. */
static int irq_clock_for(int ln)
{
    int mode = nmitimen >> 4 & 3;
    if (mode == 0)
        return -1;
    if (mode == 2)
        return ln == vtime ? (vtime ? 14 : 10) : -1;
    if (mode == 3 && ln != vtime)
        return -1;
    if (htime > 339)
        return -1;
    if (htime >= 337)
        ct_fatal("H/V IRQ at HTIME %u: matches at the end of the line, not modeled", htime);
    return 18 + 4 * htime;
}

static int irq_clock(void) { return irq_clock_for(line); }

/* ---- clock and events ----
   The clock advances by whole instructions and interrupt entries. An event
   fires at the first instruction boundary at or past its clock, in time
   order: the DRAM refresh pause (40 clocks pass with the CPU stopped), an
   H/V timer IRQ before HBlank, HBlank (HDMA), an H/V timer IRQ at or after
   HBlank, then the end of the line. Line 0 starts as soon as
   line 261 ends, so a frame boundary never waits on the caller. */

static void begin_line(void)
{
    start_line();
    irq_at = irq_clock();
    irq_done = irq_at < 0;
    hblank_done = 0;
    init_done = line != 0;
    nmi_done = line != SCHED_VBLANK_LINE;
    refresh_at = 530 + 8 - (unsigned)(line_start & 7);
    refresh_done = 0;
}

/* Clocks in the current line: 1364, except that without interlace line
   240 of every other frame (STAT78 field bit set; frame 1, 3, ...) is 1360
   (fullsnes; Mesen 2). Interlace isn't modeled (SETINI bits 0-1 have no
   effect in the vendored PPU), so this is the non-interlace timing. */
static unsigned line_clocks(void)
{
    return line == 240 && (frames & 1) ? SCHED_CLOCKS_PER_LINE - 4 : SCHED_CLOCKS_PER_LINE;
}

/* The frame hook sees memory as of the frame edge: WRAM writes the
   current instruction made after it are rolled back meanwhile. */
static void edge_hook(void)
{
    uint8_t *w = bus_wram();
    unsigned n = ct_bus_n < CT_BUS_LOG ? ct_bus_n : CT_BUS_LOG;
    if (edge_undo_from >= 0)
        for (unsigned k = n; k-- > (unsigned)edge_undo_from;)
            if (ct_bus_wram_off[k])
                w[ct_bus_wram_off[k] - 1] = ct_bus_wram_old[k];
    frame_hook(frames);
    if (edge_undo_from >= 0)
        for (unsigned k = (unsigned)edge_undo_from; k < n; k++)
            if (ct_bus_wram_off[k])
                w[ct_bus_wram_off[k] - 1] = ct_bus_wram_new[k];
}

static void advance(unsigned clocks)
{
    hclock += clocks;
    for (;;) {
        if (line == SCHED_VBLANK_LINE && !rdnmi_set && hclock >= 2) {
            rdnmi = 0x80;
            rdnmi_set = 1;
        } else if (line == 0 && !rdnmi_cleared && hclock >= 2) {
            rdnmi = 0;   /* line 0, clock 2 */
            rdnmi_cleared = 1;
        } else if (!nmi_done && hclock >= 6) {
            nmi_done = 1;   /* line 225: the NMI (walked already, or charged whole) */
            if (!walked && (nmitimen & 0x80))
                pend.nmi_count = 1;
        } else if (!refresh_done && hclock >= refresh_at) {
            if (refresh_paid)
                refresh_paid--;         /* in the clocks already */
            else
                hclock += REFRESH_CLOCKS;   /* paused: time passes, no CPU work */
            refresh_done = 1;
        } else if (!irq_done && irq_at < SCHED_HDMA_CLOCK && hclock >= (unsigned)irq_at) {
            timeup = 0x80;
            pend.irq_line = 1;
            irq_done = 1;
        } else if (!hblank_done && hclock >= SCHED_HDMA_CLOCK) {
            if (line < SCHED_VBLANK_LINE) {
                ppu_drawTo(snes_hw_ppu(), 256);   /* HDMA runs after the line's last pixel */
                dma_doHdma(snes_hw_dma());
            }
            hblank_done = 1;
        } else if (!irq_done && hclock >= (unsigned)irq_at) {
            timeup = 0x80;
            pend.irq_line = 1;
            irq_done = 1;
        } else if (hclock >= line_clocks()) {
            apu_sync(0);
            unsigned len = line_clocks();
            hclock -= len;
            line_start += len;
            if (++line == SCHED_LINES) {
                line = 0;
                frames++;
                frame_clock = line_start;
                frame_done = 1;
                if (frame_hook)
                    edge_hook();
            }
            begin_line();
        } else {
            break;
        }
    }
}

/* ---- cycle-level timing ----
   Instructions are charged whole (advance) unless something happens
   inside one: the line's DRAM refresh, the HDMA point (dot 276 of lines
   0-224, channels enabled), the frame's HDMA init point (line 0, clock
   12 + (line start & 7)), the end of the line, or a pending DMA/HDMA.
   Then its cycles (cyc_cycles) are walked against those events, timed as
   Mesen 2 does, the boot reference (#33):
   - A refresh pauses the CPU 40 clocks at its exact clock, inside a cycle
     if need be (a read samples after it if it came before the sample).
   - DMA, HDMA and HDMA init go pending at their event with a one-cycle
     start delay: the next CPU cycle start takes the delay, the one after
     runs the transfer (one per cycle start: HDMA, then init, then DMA).
   - HDMA and init: sync to 8 clocks since power-on, their clocks, then a
     wait back to a whole number of the resuming cycle's clocks counted
     from the sync. Inside a general DMA they run between bytes with no
     sync, and their clocks count toward the DMA's own wait.
   - General DMA: sync, 8, per channel 8 and 8 per byte, then the wait.
   Clocks walked past the refresh are marked paid so advance doesn't add
   it again. */


static void walk_reset(void)
{
    memset(&pend, 0, sizeof pend);
    pend.dma_after = pend.nmi_after = -1;
    refresh_paid = 0;
}

struct walk {
    uint64_t t;                   /* master clock */
    uint64_t ls;                  /* its line's start */
    int line;
    long frame;
    int refresh_done, hdma_done, init_done, nmi_done, irq_done;
    int irq_at;                   /* its line's IRQ clock, or -1 */
    int refreshes;
    int edge;                     /* a frame edge passed */
    int undo_from;                /* first write after it (bus access number), or -1 */
};

static unsigned walk_line_clocks(const struct walk *w)
{
    return w->line == 240 && (w->frame & 1) ? SCHED_CLOCKS_PER_LINE - 4 : SCHED_CLOCKS_PER_LINE;
}

/* The line's next event at or after the walk's clock: 0 refresh, 1 HDMA,
   2 HDMA init, 3 end of line, 4 NMI (line 225, clock 6), 5 H/V IRQ. */
static int walk_next(const struct walk *w, uint64_t *at)
{
    int kind = 3;
    uint64_t e = w->ls + walk_line_clocks(w);
    if (w->line == SCHED_VBLANK_LINE && !w->nmi_done && w->ls + 6 < e) {
        e = w->ls + 6;
        kind = 4;
    }
    if (!w->irq_done && w->irq_at >= 0 && w->ls + (unsigned)w->irq_at < e) {
        e = w->ls + (unsigned)w->irq_at;
        kind = 5;
    }
    if (w->line == 0 && !w->init_done && w->ls + 12 + (w->ls & 7) < e) {
        e = w->ls + 12 + (w->ls & 7);
        kind = 2;
    }
    if (!w->refresh_done && w->ls + 538 - (w->ls & 7) < e) {
        e = w->ls + 538 - (w->ls & 7);
        kind = 0;
    }
    if (!w->hdma_done && w->line < SCHED_VBLANK_LINE && w->ls + SCHED_HDMA_CLOCK < e) {
        e = w->ls + SCHED_HDMA_CLOCK;
        kind = 1;
    }
    *at = e;
    return kind;
}

/* `clocks` of CPU or DMA time; a read samples `sample` clocks in. */
static void walk_pass(struct walk *w, unsigned clocks, unsigned sample, uint64_t *sampled)
{
    uint64_t end = w->t + clocks, smp = w->t + sample;
    for (;;) {
        uint64_t e;
        int kind = walk_next(w, &e);
        if (e > end)
            break;
        switch (kind) {
        case 0:
            w->refresh_done = 1;
            w->refreshes++;
            if (e <= smp)
                smp += REFRESH_CLOCKS;
            end += REFRESH_CLOCKS;
            break;
        case 1:
            w->hdma_done = 1;
            if (snes_hdma_enabled()) {
                pend.hdma = 1;
                pend.hdma_cost = snes_hdma_cost();
                pend.delay = 1;
            }
            break;
        case 2:
            w->init_done = 1;
            pend.delay = 1;
            if (snes_hdma_enabled()) {
                pend.init = 1;
                pend.init_cost = snes_hdma_init_cost();
            }
            break;
        case 4:
            w->nmi_done = 1;
            if (nmitimen & 0x80)
                pend.nmi_count = 1;
            break;
        case 5:
            w->irq_done = 1;
            pend.irq_line = 1;
            break;
        default:
            w->ls = e;
            if (++w->line == SCHED_LINES) {
                w->line = 0;
                w->frame++;
                w->edge = 1;
            }
            w->refresh_done = w->hdma_done = w->init_done = w->nmi_done = w->irq_done = 0;
            w->irq_at = irq_clock_for(w->line);
        }
    }
    w->t = end;
    if (sampled)
        *sampled = smp;
}

/* HDMA or init inside a general DMA: between bytes, no sync. */
static void walk_dma_pending(struct walk *w, unsigned *count)
{
    if (pend.delay) {
        pend.delay = 0;
    } else if (pend.hdma) {
        pend.hdma = 0;
        walk_pass(w, pend.hdma_cost, 0, NULL);
        *count += pend.hdma_cost;
    } else if (pend.init) {
        pend.init = 0;
        walk_pass(w, pend.init_cost, 0, NULL);
        *count += pend.init_cost;
    }
}

/* At the start of a CPU cycle of `speed` clocks. */
static int walk_pending(struct walk *w, unsigned speed)
{
    if (!(pend.delay | pend.hdma | pend.init | pend.dma))
        return 0;
    if (pend.delay) {
        pend.delay = 0;
        return 0;
    }
    unsigned sync = 8 - (unsigned)(w->t & 7), count = sync;
    walk_pass(w, sync, 0, NULL);
    if (pend.hdma || pend.init) {
        unsigned n = pend.hdma ? pend.hdma_cost : pend.init_cost;
        if (pend.hdma)
            pend.hdma = 0;
        else
            pend.init = 0;
        walk_pass(w, n, 0, NULL);
        count += n;
    } else {
        pend.dma = 0;
        walk_pass(w, 8, 0, NULL);
        count += 8;
        walk_dma_pending(w, &count);
        for (int c = 0; c < 8; c++) {
            uint32_t size = pend.dma_sizes[c];
            if (!size)
                continue;
            walk_pass(w, 8, 0, NULL);
            count += 8;
            walk_dma_pending(w, &count);
            for (uint32_t b = 0; b < size; b++) {
                walk_pass(w, 8, 0, NULL);
                walk_dma_pending(w, &count);
            }
            count += 8 * ((snes_ref_quirks & SNES_QUIRK_MESEN_DMA_COUNT8) ? (size & 0xFF) : size);
        }
    }
    walk_pass(w, speed - count % speed, 0, NULL);
    return 1;
}

/* The start of a CPU cycle of `speed` clocks: a pending transfer, then
   the NMI counter and the IRQ line are sampled (not right after a
   transfer: then both wait a cycle). `i` is the I flag in effect. */
static void walk_cycle_start(struct walk *w, unsigned speed, int i)
{
    int lock = walk_pending(w, speed);
    if (pend.nmi_count && --pend.nmi_count == 0) {
        if (lock)
            pend.nmi_count = 1;
        else
            pend.need_nmi = 1;
    }
    pend.prev_irq = !lock && pend.irq_line && !i;
}

static void walk_init(struct walk *w)
{
    w->t = line_start + hclock;
    w->ls = line_start;
    w->line = line;
    w->frame = frames;
    w->refresh_done = refresh_done;
    w->hdma_done = hblank_done;
    w->init_done = init_done;
    w->nmi_done = nmi_done;
    w->irq_done = irq_done;
    w->irq_at = irq_at;
    w->refreshes = 0;
    w->edge = 0;
    w->undo_from = -1;
}

/* Walk the current instruction's cycles from its start (the boundary);
   returns the master clock at its end, and the sample clock of cycle
   `upto` (its read point, or the end of a write). */
static uint64_t walk_insn(struct walk *w, unsigned upto, uint64_t *sampled)
{
    uint8_t clk[160], kind[160];
    int8_t idx[160];
    unsigned n = cyc_cycles(clk, kind, idx, sizeof clk, upto);
    walk_init(w);
    if (!n) {
        if (sampled)
            *sampled = w->t;
        return w->t;
    }
    for (unsigned k = 0; k < n; k++) {
        walk_cycle_start(w, clk[k], insn_i);
        walk_pass(w, clk[k], kind[k] == CY_READ ? clk[k] - 4u : clk[k],
                  k + 1 == n ? sampled : NULL);
        if (w->edge && w->undo_from < 0 && idx[k] >= 0 && kind[k] == CY_WRITE)
            w->undo_from = idx[k];   /* this write lands after the frame edge */
        if (idx[k] >= 0 && idx[k] == pend.dma_after) {
            pend.dma = 1;
            pend.delay = 1;
            pend.dma_after = -1;
        }
        if (idx[k] >= 0 && idx[k] == pend.nmi_after) {
            pend.nmi_count = 2;
            pend.nmi_after = -1;
        }
    }
    return w->t;
}

/* Master clock of the CPU access being made (the last bus access so far),
   cycle by cycle from the instruction's start. */
static uint64_t access_clock(unsigned early)
{
    (void)early;
    if (!cyc_in_progress())
        return line_start + hclock;
    if (!ct_bus_n || ct_bus_n > CT_BUS_LOG)
        return line_start + hclock + cyc_elapsed();
    struct walk w;
    uint64_t sampled;
    typeof(pend) saved = pend;
    walk_insn(&w, ct_bus_n - 1, &sampled);
    pend = saved;
    return sampled;
}

static void dma_start(const uint32_t sizes[8])
{
    memcpy(pend.dma_sizes, sizes, sizeof pend.dma_sizes);
    pend.dma_after = (int)ct_bus_n - 1;   /* the $420B write */
}

static void walk_commit(const struct walk *w, uint64_t start, uint64_t t)
{
    refresh_paid += w->refreshes;
    edge_undo_from = w->undo_from;
    walked = 1;
    advance((unsigned)(t - start));
    walked = 0;
    edge_undo_from = -1;
    if (line == w->line) {
        init_done |= w->init_done;
        nmi_done |= w->nmi_done;
    }
}

/* One idle cycle of WAI (6 clocks), or a run of them up to just before the
   next event when nothing can happen in between. An NMI seen or the IRQ
   line up at a cycle's start ends the wait after that cycle and one more
   (even with I set: then no interrupt is taken). */
static void wai_cycle(void)
{
    int quiet = !(pend.delay | pend.hdma | pend.init | pend.dma) && !pend.nmi_count &&
                !pend.need_nmi && !pend.irq_line;
    if (quiet) {
        unsigned next = line_clocks();
        if (!refresh_done && refresh_at < next)
            next = refresh_at;
        if (!hblank_done && line < SCHED_VBLANK_LINE && SCHED_HDMA_CLOCK < next)
            next = SCHED_HDMA_CLOCK;
        if (line == 0 && !init_done && 12 + (unsigned)(line_start & 7) < next)
            next = 12 + (unsigned)(line_start & 7);
        if (!nmi_done && 6 < next)
            next = 6;
        if (!irq_done && irq_at >= 0 && (unsigned)irq_at < next)
            next = (unsigned)irq_at;
        if (next > hclock + 12) {
            unsigned k = (next - hclock) / 6 - 1;
            advance(6 * k);   /* k idle cycles: nothing happens in them */
        }
    }
    struct walk w;
    walk_init(&w);
    uint64_t start = w.t;
    walk_cycle_start(&w, 6, cpu->i);
    if (pend.need_nmi || pend.irq_line)
        wai_over = 1;   /* seen at this cycle's start */
    walk_pass(&w, 6, 6, NULL);
    walk_commit(&w, start, w.t);
}

/* Charge an instruction (or interrupt entry) of `clocks` clocks. */
static void charge(unsigned clocks)
{
    if (!clocks)
        return;   /* nothing begun (already charged): pending transfers wait */
    unsigned end = hclock + clocks;
    int inside = (pend.delay | pend.hdma | pend.init | pend.dma) || pend.dma_after >= 0 ||
                 pend.nmi_count || pend.nmi_after >= 0 ||
                 (!nmi_done && 6 <= end) || (!irq_done && irq_at >= 0 && (unsigned)irq_at <= end) ||
                 end >= line_clocks() || (!refresh_done && refresh_at <= end) ||
                 (!hblank_done && line < SCHED_VBLANK_LINE && SCHED_HDMA_CLOCK <= end &&
                  snes_hdma_enabled()) ||
                 (line == 0 && !init_done && 12 + (line_start & 7) <= end);
    if (!inside) {
        pend.prev_irq = pend.irq_line && !insn_i;   /* as at its last cycle start */
        take_irq = pend.prev_irq;
        cyc_done();
        advance(clocks);
        return;
    }
    struct walk w;
    uint64_t start = line_start + hclock;
    uint64_t t = walk_insn(&w, ~0u, NULL);
    cyc_done();
    pend.dma_after = pend.nmi_after = -1;
    take_irq = pend.prev_irq;
    walk_commit(&w, start, t);
}

/* One step at an instruction boundary: take a due interrupt, sit out a
   WAI until the next event, or run one instruction. */
/* ---- native dispatch ----
   At an instruction boundary, a recompiled function whose entry is PB:PC
   runs natively if the CPU matches its entry state: native mode, its M/X
   variant, and the DB/DP funcs.toml says it assumes (when it says). A
   function that can reach an extern hook never runs natively: the hook
   stands in for ROM code that system mode runs for real. Native code
   charges cycles through the tick hook, per instruction, with the same
   model as the interpreter. */

static int native_on = 1;
static const ct_func **native;   /* sorted by address */
static unsigned n_native;

static int by_addr(const void *a, const void *b)
{
    uint32_t x = (*(const ct_func *const *)a)->addr, y = (*(const ct_func *const *)b)->addr;
    return x < y ? -1 : x > y;
}

static void native_build(void)
{
    free(native);
    native = malloc((ct_func_count + 1) * sizeof *native);
    n_native = 0;
    for (unsigned k = 0; k < ct_func_count; k++)
        if (ct_funcs[k].fn && !ct_funcs[k].calls_extern)
            native[n_native++] = &ct_funcs[k];
    qsort(native, n_native, sizeof *native, by_addr);
}

/* ---- native coverage profile ----
   Instructions run natively (one tick each) and interpreted. Interpreted
   ones are charged to the function they run in: the target of the last
   interpreted JSR/JSL/JSR (a,X) or interrupt entry not yet returned from,
   with the M, X and E it was entered with (a shadow stack; a native
   function's own calls and returns never reach it). */

static uint32_t entry_key(const CPU *c)
{
    return (uint32_t)c->PB << 16 | c->PC | (uint32_t)c->m << 24 | (uint32_t)c->x << 25 |
           (uint32_t)c->e << 26 | 1u << 31;
}

static void shadow_push(const CPU *c)
{
    if (shadow_n < PROF_DEPTH)
        shadow[shadow_n] = entry_key(c);
    shadow_n++;
}

static void shadow_pop(void)
{
    if (shadow_n > 0)
        shadow_n--;
}

static void prof_charge(void)
{
    uint32_t key = shadow_n > 0 && shadow_n <= PROF_DEPTH ? shadow[shadow_n - 1] : 1u << 31;
    unsigned k = (key * 2654435761u) & (PROF_SLOTS - 1);
    for (unsigned n = 0; n < PROF_SLOTS; n++, k = (k + 1) & (PROF_SLOTS - 1)) {
        if (prof[k].key == key || !prof[k].key) {
            prof[k].key = key;
            prof[k].count++;
            return;
        }
    }
}

uint64_t sched_native_insns(void) { return prof_native; }
uint64_t sched_interp_insns(void) { return prof_interp; }

/* Why the function at this key didn't run natively. */
static const char *not_native_reason(uint32_t key)
{
    uint32_t addr = key & 0xFFFFFF;
    int m = key >> 24 & 1, x = key >> 25 & 1, e = key >> 26 & 1;
    if ((key & 0xFFFFFF) == 0 && !(key & 0x7F000000))
        return "outside any call (reset code, main loop)";
    if (e)
        return "emulation mode";
    int any = 0, ext = 0, state = 0;
    for (unsigned k = 0; k < ct_func_count; k++) {
        const ct_func *f = &ct_funcs[k];
        if (f->addr != addr || !f->fn)
            continue;
        any = 1;
        if (f->calls_extern)
            ext = 1;
        else if (f->m == m && f->x == x)
            state = 1;
    }
    if (!any)
        return "not recompiled";
    if (state)
        return "recompiled, but DB/DP differ from the recompiled entry";
    if (ext)
        return "recompiled, but can reach an extern hook";
    return "recompiled only for another M/X";
}

void sched_profile_report(FILE *out, int top)
{
    uint64_t total = prof_native + prof_interp;
    fprintf(out, "profile: %llu instructions, %.2f%% native (%llu native, %llu interpreted)\n",
            (unsigned long long)total, total ? 100.0 * (double)prof_native / (double)total : 0.0,
            (unsigned long long)prof_native, (unsigned long long)prof_interp);
    for (int n = 0; n < top; n++) {
        int best = -1;
        for (int k = 0; k < PROF_SLOTS; k++)
            if (prof[k].key && prof[k].count && (best < 0 || prof[k].count > prof[best].count))
                best = k;
        if (best < 0)
            break;
        uint32_t key = prof[best].key;
        const char *name = "";
        for (unsigned k = 0; k < ct_func_count; k++)
            if (ct_funcs[k].addr == (key & 0xFFFFFF)) {
                name = ct_funcs[k].name;
                break;
            }
        fprintf(out, "profile: %6.2f%%  $%06X m%dx%de%d %-28s %s\n",
                100.0 * (double)prof[best].count / (double)(total ? total : 1), key & 0xFFFFFF,
                key >> 24 & 1, key >> 25 & 1, key >> 26 & 1, name, not_native_reason(key));
        prof[best].count = 0;   /* report consumes the table */
    }
}

static const ct_func *native_lookup(const CPU *c)
{
    if (!native_on || c->e || !n_native)
        return NULL;
    uint32_t at = (uint32_t)c->PB << 16 | c->PC;
    unsigned lo = 0, hi = n_native;
    while (lo < hi) {
        unsigned mid = (lo + hi) / 2;
        if (native[mid]->addr < at)
            lo = mid + 1;
        else
            hi = mid;
    }
    for (; lo < n_native && native[lo]->addr == at; lo++) {
        const ct_func *f = native[lo];
        if (f->m == c->m && f->x == c->x && (f->db < 0 || f->db == c->DB) &&
            (f->dp < 0 || f->dp == c->DP))
            return f;
    }
    return NULL;
}

void sched_set_native(int on) { native_on = on; }

static void exec_one(void);

/* Start of each native instruction (ct_insn): charge the previous one,
   run events, take a due interrupt at this boundary, begin this one. An
   interrupt enters with PB:PC at this instruction and runs (natively or
   interpreted, nested interrupts included) until its own RTI; a handler
   may switch to a stack of its own in between. After that RTI the CPU
   must be exactly in the interrupted context (PB:PC, S, M, X, E); anything
   else is fatal, and native code never resumes in a changed one. */
static void tick(CPU *c, uint32_t at, uint8_t op)
{
    c->PB = (uint8_t)(at >> 16);   /* as the interpreter has it at a boundary */
    c->PC = (uint16_t)at;
    charge(cyc_finish());
    for (;;) {
        int nmi = pend.need_nmi;
        if (!nmi && !take_irq)
            break;
        if (nmi) {
            pend.need_nmi = 0;
            nmis++;
        }
        take_irq = 0;
        CPU before = *c;
        long depth = int_depth++;
        insn_i = 1;
        charge(interp_interrupt(c, nmi));
        skip_check = 1;
        shadow_push(c);
        while (int_depth > depth)
            exec_one();
        if (c->PB != before.PB || c->PC != before.PC || c->S != before.S || c->m != before.m ||
            c->x != before.x || c->e != before.e)
            ct_fatal("$%06X: %s did not return to the interrupted native code: "
                     "RTI went to $%02X%04X S=%04X m%dx%de%d, interrupted at $%02X%04X S=%04X m%dx%de%d",
                     at, nmi ? "NMI" : "IRQ", c->PB, c->PC, c->S, c->m, c->x, c->e, before.PB,
                     before.PC, before.S, before.m, before.x, before.e);
        charge(cyc_finish());
    }
    if (op == 0x40)
        int_depth--;   /* this native RTI (charged at the next boundary) */
    static uint32_t last_at;
    if (!((op == 0x54 || op == 0x44) && at == last_at))
        prof_native++;   /* an MVN/MVP byte after the first isn't a new instruction */
    last_at = at;
    insn_i = c->i;
    cyc_begin_compiled(c, at, op);
}

/* Interrupts are taken at the end of an instruction (Mesen 2
   CheckForInterrupts): an NMI once seen at a cycle start, else an IRQ if
   the line was up (and I clear) at its last cycle start. An interrupt
   entry is followed by at least one instruction of the handler. */
static void exec_one(void)
{
    if (skip_check) {
        skip_check = 0;
    } else if (!interp_waiting() && (pend.need_nmi || take_irq)) {   /* WAI: see below */
        int nmi = pend.need_nmi;
        if (nmi) {
            pend.need_nmi = 0;
            nmis++;
        }
        take_irq = 0;
        int_depth++;
        insn_i = 1;
        charge(interp_interrupt(cpu, nmi));   /* IRQ is level: until $4211 is read */
        skip_check = 1;
        shadow_push(cpu);
        return;
    }
    if (interp_waiting()) {
        /* WAI (Mesen 2): idle cycles until an NMI is seen or the IRQ line
           is up, then one more; then the interrupt, if any, is taken. */
        int over = wai_over;
        wai_cycle();
        if (over) {
            wai_over = 0;
            interp_wake();
            take_irq = pend.prev_irq;
        }
        return;
    }
    const ct_func *f = native_lookup(cpu);
    if (f) {
        uint32_t entry = entry_key(cpu);
        f->fn(cpu);
        if (shadow_n > 0 && shadow_n <= PROF_DEPTH && shadow[shadow_n - 1] == entry)
            shadow_pop();   /* the interpreted JSR/JSL that called it */
        charge(cyc_finish());   /* its last instruction (RTS/RTL) */
        return;
    }
    uint32_t at = (uint32_t)cpu->PB << 16 | cpu->PC;
    static uint32_t mv_at = ~0u;
    if (at != mv_at) {   /* an MVN/MVP byte after the first isn't a new instruction */
        prof_interp++;
        prof_charge();
    }
    insn_i = cpu->i;
    unsigned clocks = interp_step(cpu);
    uint8_t op = interp_last_op();
    if (interp_waiting())
        wai_over = 0;   /* WAI just ran */
    mv_at = (op == 0x54 || op == 0x44) && ((uint32_t)cpu->PB << 16 | cpu->PC) == at ? at : ~0u;
    if (op == 0x40)
        int_depth--;
    if (op == 0x20 || op == 0x22 || op == 0xFC)
        shadow_push(cpu);   /* JSR, JSL, JSR (a,X): now at the callee */
    else if (op == 0x60 || op == 0x6B || op == 0x40)
        shadow_pop();
    charge(clocks);
}

long sched_run_frame(void)
{
    if (need_start) {
        need_start = 0;
        begin_line();
        advance(start_delay);
    }
    long f0 = frames;
    frame_done = 0;
    while (!frame_done)
        exec_one();
    return frames - f0;
}


const uint8_t *sched_frame(void) { return present; }
long sched_frame_count(void) { return frames; }
void sched_set_frame_hook(void (*fn)(long frame)) { frame_hook = fn; }
uint64_t sched_clock(void) { return line_start + hclock; }
uint64_t sched_frame_clock(void) { return frame_clock; }
long sched_nmi_count(void) { return nmis; }
int sched_line(void) { return line; }
