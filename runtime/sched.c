#include "sched.h"

#include <string.h>

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

static void apu_sync(unsigned early);
static uint64_t access_clock(unsigned early);
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
static int need_start;           /* line 0 not started yet (sched_init) */
static unsigned start_delay;     /* clocks before the first instruction */
static int frame_done;
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
    nmi_pending = 0;
    line_start = 0;
    snes_apu_sync = apu_sync;
    snes_master_clock = sched_clock;
    snes_access_clock = access_clock;
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

/* Master clock of the access being made, `early` clocks before its end.
   Events apply at instruction boundaries, but a DRAM refresh due before
   that point inside the instruction has already paused the CPU there. */
static uint64_t access_clock(unsigned early)
{
    int64_t h = (int64_t)hclock + cyc_elapsed() - early;
    if (h < 0)
        h = 0;
    uint64_t at = line_start + (uint64_t)h;
    if (!refresh_done && refresh_at <= (uint64_t)h)
        at += REFRESH_CLOCKS;
    return at;
}

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
        rdnmi = 0;
        PpuBeginDrawing(ppu, fb, SCHED_WIDTH * 4, 0);
        dma_initHdma(dma);
    }
    if (line == SCHED_VBLANK_LINE) {
        memcpy(present, fb, sizeof present);   /* rows 0-223 are final */
        in_vblank = 1;
        rdnmi = 0x80;
        snes_oam_vblank_reload();
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

static unsigned next_event(void)
{
    unsigned t = line_clocks();
    if (!hblank_done && SCHED_HBLANK_CLOCK < t)
        t = SCHED_HBLANK_CLOCK;
    if (!irq_done && (unsigned)irq_at < t)
        t = (unsigned)irq_at;
    return t;
}

static void advance(unsigned clocks)
{
    hclock += clocks;
    for (;;) {
        if (!refresh_done && hclock >= refresh_at) {
            hclock += REFRESH_CLOCKS;   /* paused: time passes, no CPU work */
            refresh_done = 1;
        } else if (!irq_done && irq_at < SCHED_HBLANK_CLOCK && hclock >= (unsigned)irq_at) {
            timeup = 0x80;
            irq_done = 1;
        } else if (!hblank_done && hclock >= SCHED_HBLANK_CLOCK) {
            if (line < SCHED_HEIGHT)
                dma_doHdma(snes_hw_dma());
            hblank_done = 1;
        } else if (!irq_done && hclock >= (unsigned)irq_at) {
            timeup = 0x80;
            irq_done = 1;
        } else if (hclock >= line_clocks()) {
            apu_sync(0);
            unsigned len = line_clocks();
            hclock -= len;
            line_start += len;
            if (++line == SCHED_LINES) {
                line = 0;
                frames++;
                frame_done = 1;
                if (frame_hook)
                    frame_hook(frames);
            }
            begin_line();
        } else {
            break;
        }
    }
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
    advance(cyc_finish());
    for (;;) {
        int nmi = nmi_pending;
        if (!nmi && !(timeup && (nmitimen & 0x30) && !c->i))
            break;
        if (nmi) {
            nmi_pending = 0;
            nmis++;
        }
        CPU before = *c;
        long depth = int_depth++;
        advance(interp_interrupt(c, nmi));
        shadow_push(c);
        while (int_depth > depth)
            exec_one();
        if (c->PB != before.PB || c->PC != before.PC || c->S != before.S || c->m != before.m ||
            c->x != before.x || c->e != before.e)
            ct_fatal("$%06X: %s did not return to the interrupted native code: "
                     "RTI went to $%02X%04X S=%04X m%dx%de%d, interrupted at $%02X%04X S=%04X m%dx%de%d",
                     at, nmi ? "NMI" : "IRQ", c->PB, c->PC, c->S, c->m, c->x, c->e, before.PB,
                     before.PC, before.S, before.m, before.x, before.e);
        advance(cyc_finish());
    }
    if (op == 0x40)
        int_depth--;   /* this native RTI (charged at the next boundary) */
    prof_native++;
    cyc_begin_compiled(c, at, op);
}

static void exec_one(void)
{
    if (nmi_pending) {
        nmi_pending = 0;
        nmis++;
        int_depth++;
        advance(interp_interrupt(cpu, 1));
        shadow_push(cpu);
        return;
    }
    if (timeup && (nmitimen & 0x30)) {
        if (!cpu->i) {
            int_depth++;
            advance(interp_interrupt(cpu, 0));   /* level: until $4211 is read */
            shadow_push(cpu);
            return;
        }
        interp_wake();   /* IRQ with I=1 still ends WAI */
    }
    if (interp_waiting()) {
        advance(next_event() - hclock);
        return;
    }
    const ct_func *f = native_lookup(cpu);
    if (f) {
        uint32_t entry = entry_key(cpu);
        f->fn(cpu);
        if (shadow_n > 0 && shadow_n <= PROF_DEPTH && shadow[shadow_n - 1] == entry)
            shadow_pop();   /* the interpreted JSR/JSL that called it */
        advance(cyc_finish());   /* its last instruction (RTS/RTL) */
        return;
    }
    prof_interp++;
    prof_charge();
    unsigned clocks = interp_step(cpu);
    uint8_t op = interp_last_op();
    if (op == 0x40)
        int_depth--;
    if (op == 0x20 || op == 0x22 || op == 0xFC)
        shadow_push(cpu);   /* JSR, JSL, JSR (a,X): now at the callee */
    else if (op == 0x60 || op == 0x6B || op == 0x40)
        shadow_pop();
    advance(clocks);
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
long sched_nmi_count(void) { return nmis; }
int sched_line(void) { return line; }
