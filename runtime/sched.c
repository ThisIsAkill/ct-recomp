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
static uint8_t wrio;            /* $4201, stored only */
static uint8_t htime_vtime[4];  /* $4207-$420A, stored only; used by #21 */

static void apu_sync(void);

/* ---- registers ---- */

static void nmitimen_write(uint16_t reg, uint8_t v)
{
    (void)reg;
    if (v & 0x30)
        ct_fatal("NMITIMEN $%02X: H/V IRQ not implemented (#21)", v);
    /* Enabling NMI while the VBlank NMI flag is still set fires it. */
    if (!(nmitimen & 0x80) && (v & 0x80) && (rdnmi & 0x80))
        nmi_pending = 1;
    nmitimen = v;
}

/* Stored, no effect yet: WRIO's H/V counter latch and the IRQ timer
   targets belong to #21, and IRQs cannot be enabled until then. */
static void wrio_write(uint16_t reg, uint8_t v)
{
    (void)reg;
    wrio = v;
}

static void htime_vtime_write(uint16_t reg, uint8_t v)
{
    htime_vtime[reg - 0x4207] = v;
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
    return 0;   /* no IRQ source until #21 */
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
    memset(htime_vtime, 0, sizeof htime_vtime);
    memset(fb, 0, sizeof fb);
    bus_hook(0x4200, NULL, nmitimen_write);
    bus_hook(0x4201, NULL, wrio_write);
    for (uint16_t r = 0x4207; r <= 0x420A; r++)
        bus_hook(r, NULL, htime_vtime_write);
    bus_hook(0x4210, rdnmi_read, NULL);
    bus_hook(0x4211, timeup_read, NULL);
    bus_hook(0x4212, hvbjoy_read, NULL);
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
        if (nmitimen & 0x80)
            nmi_pending = 1;
    }
    if (line == SCHED_VBLANK_LINE + 3)
        autojoy_busy = 0;
    if (line <= SCHED_HEIGHT)
        ppu_runLine(ppu, line);   /* line L draws row L-1 */
}

void sched_run_frame(void)
{
    Dma *dma = snes_hw_dma();
    for (line = 0; line < SCHED_LINES; line++) {
        start_line();
        run_to(SCHED_HBLANK_CLOCK);
        if (line < SCHED_HEIGHT)
            dma_doHdma(dma);
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
