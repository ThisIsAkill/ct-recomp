#include "sched.h"

#include <string.h>

#include "bus.h"
#include "dma.h"
#include "interp.h"
#include "ppu.h"
#include "snes_adapter.h"

static CPU *cpu;
static uint8_t fb[SCHED_WIDTH * SCHED_HEIGHT * 4];
static int line;
static unsigned hclock;         /* master clocks into the current line */
static uint64_t line_start;     /* master clocks since sched_init at line start */
static uint64_t spc_done;       /* SPC700 cycles run since sched_init */
static long frames, nmis;
static int nmi_pending;

static uint8_t nmitimen;        /* $4200 */
static uint8_t rdnmi;           /* $4210 bit 7: NMI flag, cleared by read */
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

static void apu_sync(void);

/* ---- registers ---- */

static void nmitimen_write(uint16_t reg, uint8_t v)
{
    (void)reg;
    /* Enabling NMI while the VBlank NMI flag is still set fires it. */
    if (!(nmitimen & 0x80) && (v & 0x80) && (rdnmi & 0x80))
        nmi_pending = 1;
    if (!(v & 0x30))
        timeup = 0;   /* disabling H/V IRQs drops a pending one */
    nmitimen = v;
}

/* ---- H/V counters ---- */

static void latch_counters(void)
{
    lat_h = (uint16_t)(hclock / 4 > 339 ? 339 : hclock / 4);
    lat_v = (uint16_t)line;
    lat_flag = 1;
}

static uint8_t slhv_read(uint16_t reg)
{
    (void)reg;
    if (wrio & 0x80)
        latch_counters();
    return 0;   /* open bus, not modeled */
}

static uint8_t ophct_read(uint16_t reg)
{
    (void)reg;
    uint8_t v = ophct_hi ? (uint8_t)(lat_h >> 8 & 1) : (uint8_t)lat_h;
    ophct_hi ^= 1;
    return v;
}

static uint8_t opvct_read(uint16_t reg)
{
    (void)reg;
    uint8_t v = opvct_hi ? (uint8_t)(lat_v >> 8 & 1) : (uint8_t)lat_v;
    opvct_hi ^= 1;
    return v;
}

static uint8_t stat78_read(uint16_t reg)
{
    (void)reg;
    uint8_t v = (uint8_t)(lat_flag << 6 | 3);   /* NTSC, no interlace, PPU2 version 3 */
    lat_flag = 0;
    ophct_hi = opvct_hi = 0;
    return v;
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

static uint8_t rdnmi_read(uint16_t reg)
{
    (void)reg;
    uint8_t v = (uint8_t)(rdnmi | 0x02);   /* CPU version 2 */
    rdnmi = 0;
    return v;
}

static uint8_t timeup_read(uint16_t reg)
{
    (void)reg;
    uint8_t v = timeup;
    timeup = 0;
    return v;
}

static uint8_t hvbjoy_read(uint16_t reg)
{
    (void)reg;
    return (uint8_t)(in_vblank << 7 | (hclock >= SCHED_HBLANK_CLOCK) << 6 | autojoy_busy);
}

void sched_init(CPU *c)
{
    cpu = c;
    line = 0;
    hclock = 0;
    frames = nmis = 0;
    nmi_pending = 0;
    line_start = spc_done = 0;
    snes_apu_sync = apu_sync;
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
    bus_hook(0x4200, NULL, nmitimen_write);
    bus_hook(0x4201, NULL, wrio_write);
    for (uint16_t r = 0x4207; r <= 0x420A; r++)
        bus_hook(r, NULL, htime_vtime_write);
    bus_hook(0x4210, rdnmi_read, NULL);
    bus_hook(0x4211, timeup_read, NULL);
    bus_hook(0x4212, hvbjoy_read, NULL);
    for (uint16_t r = 0x4218; r <= 0x421F; r++)
        bus_hook(r, joy_read, NULL);
    bus_hook(0x2137, slhv_read, NULL);
    bus_hook(0x213C, ophct_read, NULL);
    bus_hook(0x213D, opvct_read, NULL);
    bus_hook(0x213F, stat78_read, NULL);
}

/* ---- APU ----
   The SPC700 runs at 1.024 MHz against the 21.477272 MHz master clock. It
   is caught up to CPU time on every $2140-$2143 access (instruction-start
   time) and at the end of each line, so the upload handshake sees the
   driver respond at a plausible pace. Deterministic. */

#define MASTER_HZ 21477272u
#define SPC_HZ    1024000u

static void apu_sync(void)
{
    uint64_t want = (line_start + hclock) * SPC_HZ / MASTER_HZ;
    if (want > spc_done) {
        snes_apu_run((uint32_t)(want - spc_done));
        spc_done = want;
    }
}

/* ---- CPU ---- */

static void run_to(unsigned target)
{
    while (hclock < target) {
        if (nmi_pending) {
            nmi_pending = 0;
            nmis++;
            hclock += interp_interrupt(cpu, 1);
            continue;
        }
        if (timeup && (nmitimen & 0x30)) {
            if (!cpu->i) {
                hclock += interp_interrupt(cpu, 0);   /* level: until $4211 is read */
                continue;
            }
            interp_wake();   /* IRQ with I=1 still ends WAI */
        }
        if (interp_waiting()) {
            hclock = target;   /* WAI: nothing happens until the next event */
            break;
        }
        hclock += interp_step(cpu);
    }
}

/* ---- frame ---- */

static void start_line(void)
{
    Ppu *ppu = snes_hw_ppu();
    Dma *dma = snes_hw_dma();
    if (line == 0) {
        in_vblank = 0;
        rdnmi = 0;
        PpuBeginDrawing(ppu, fb, SCHED_WIDTH * 4, 0);
        dma_initHdma(dma);
    }
    if (line == SCHED_VBLANK_LINE) {
        in_vblank = 1;
        rdnmi = 0x80;
        autojoy_busy = nmitimen & 1;
        if (nmitimen & 1)
            memcpy(joy, pad, sizeof joy);   /* auto-read (results readable at once) */
        if (nmitimen & 0x80)
            nmi_pending = 1;
    }
    if (line == SCHED_VBLANK_LINE + 3)
        autojoy_busy = 0;
    if (line <= SCHED_HEIGHT)
        ppu_runLine(ppu, line);   /* line L draws row L-1 */
}

/* Master clock in this line where the H/V timer fires, or -1. HTIME is in
   dots (4 master clocks); V-only fires at the start of line VTIME. The
   mode is sampled at the start of the line. */
static int irq_clock(void)
{
    int mode = nmitimen >> 4 & 3;
    if (mode == 0 || (htime > 339 && mode != 2))
        return -1;
    if (mode == 1)
        return htime * 4;
    if (line != vtime)
        return -1;
    return mode == 2 ? 0 : htime * 4;
}

void sched_run_frame(void)
{
    Dma *dma = snes_hw_dma();
    for (line = 0; line < SCHED_LINES; line++) {
        start_line();
        int irq = irq_clock();
        if (irq >= 0 && irq < SCHED_HBLANK_CLOCK) {
            run_to((unsigned)irq);
            timeup = 0x80;
        }
        run_to(SCHED_HBLANK_CLOCK);
        if (line < SCHED_HEIGHT)
            dma_doHdma(dma);
        if (irq >= SCHED_HBLANK_CLOCK) {
            run_to((unsigned)irq);
            timeup = 0x80;
        }
        run_to(SCHED_CLOCKS_PER_LINE);
        apu_sync();
        hclock -= SCHED_CLOCKS_PER_LINE;
        line_start += SCHED_CLOCKS_PER_LINE;
    }
    line = 0;
    frames++;
}

const uint8_t *sched_frame(void) { return fb; }
long sched_frame_count(void) { return frames; }
long sched_nmi_count(void) { return nmis; }
int sched_line(void) { return line; }
