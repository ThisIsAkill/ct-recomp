/* Boot probe: run the game in $CT_ROM from reset in the system-mode
 * interpreter under the frame scheduler, headless.
 *
 * usage: ct_boot [--frames N] [--dump DIR] [--needed-hw FILE] [options]
 *
 * --dump DIR       write DIR/frame_NNNNN.png for every frame the PPU drew
 *                  anything into (any nonblack pixel) that differs from the
 *                  last one written
 * --needed-hw FILE write FILE: the fatal error that stopped the run, if any
 *                  (where the boot stops next), and the hardware stubbed
 *                  on purpose along the way
 * --input F1-F2:B  hold buttons B on pad 1 for frames F1..F2 (1-based,
 *                  inclusive; repeatable). B: names joined by '+' (b y
 *                  select start up down left right a x l r) or a hex mask
 * --script FILE    --input specs from FILE, one per line ('#' comments)
 * --hash-log FILE  write per-frame state hashes to FILE, taken at the frame
 *                  edge (native and interpreted runs must agree): "frame
 *                  cpu=H wram=H sram=H vram=H cgram=H oam=H frame=H apu=H",
 *                  FNV-1a 64 each; apu covers the SPC700 registers, ports,
 *                  timers, its RAM and the DSP registers
 * --expect-pc ADDR exit 1 unless the instruction at ADDR (hex) ran
 *                  (repeatable)
 * --expect-wram F:ADDR=V / F:ADDR!=V  exit 1 unless the WRAM byte at ADDR
 *                  (hex offset, 0-1FFFF) is (is not) V (hex) at the edge of
 *                  frame F (repeatable): a replay's outcome, e.g. an enemy's
 *                  HP at 0
 * --ref-log FILE  write "frame wram_hash frame_hash" per frame to FILE, the
 *                  hashes tools/mesen_ref.py logs from a reference emulator:
 *                  FNV-1a 64 of WRAM, and of the frame as 15-bit pixels
 *                  (" dma" appended when the frame edge fell inside a general
 *                  DMA: its bytes all landed at its start)
 * --wram FILE      write the 128 KB of WRAM to FILE at the last frame edge
 * --vram FILE      likewise the 64 KB of VRAM, then CGRAM (512 bytes) and
 *                  OAM (544 bytes)
 * --aram FILE      likewise the SPC700's 64 KB of RAM
 * --profile N      at the end, print the native share of instructions and
 *                  the N functions most interpreted instructions ran in,
 *                  with why each didn't run natively
 * --min-native P   exit 1 unless at least P percent of instructions ran
 *                  natively
 * --watch A[:N]    print every change to WRAM bytes A..A+N-1 (A a WRAM
 *                  offset in hex, 0-1FFFF; N default 2): frame, line, the
 *                  instruction that made it, old and new bytes (repeatable)
 * --at ADDR        print frame, line and master clock each time the
 *                  instruction at ADDR (hex) runs, the first 20 times
 *                  (repeatable)
 * --log-entry ADDR[:MEM] print one line every time the instruction at ADDR
 *                  (hex) runs: "entry ADDR frame F line L clock C A X Y S DP
 *                  DB PB m x" and the 16 bytes at MEM (hex, bank 0 WRAM;
 *                  default DP), for tools that read a routine's arguments
 *                  (repeatable)
 * --snap-at ADDR DIR  write the 128 KB of WRAM to DIR/snap_NNNN.bin each time
 *                  the instruction at ADDR (hex) runs (first 256 times), for
 *                  checking what a routine left in memory
 * --wram-entries F  write "ADDR mMxX" to F for every WRAM address ($7E/$7F)
 *                  execution arrives at from ROM other than by a return
 *                  (jumps, calls, interrupts into WRAM code), once per
 *                  address and state, for finding entries into code the
 *                  game puts in WRAM
 * --overlay-strict / --overlay-fallback  when native overlay code (#92) finds
 *                  its own bytes changed while it runs: fail loudly (debug
 *                  builds' default; lockstep and reference runs use it), or
 *                  carry on in the interpreter (release builds' default)
 * --first-exec F   write "ADDR clock" to F the first time each instruction
 *                  address runs (for timing comparisons against a reference)
 * --trace LO:HI[@N] F  write "ADDR clock X Y" to F for every instruction
 *                  starting at master clock LO-HI (from frame N on: a reset
 *                  restarts the clock), for trace diffs against a reference
 * --interp-only    run everything in the interpreter (no native dispatch)
 * --ref-quirk NAME reproduce a reference emulator's quirk, for comparisons
 *                  only (tools/ref_compare.py): mesen-dma-count8
 *
 * --min-nmis K     exit 1 unless at least K NMIs were taken
 * --require-render exit 1 if the last frame is all black
 * --wav FILE       write the audio (the DSP output, 32 kHz stereo) to FILE
 * --require-audio  exit 1 if every audio sample of the run is zero
 *
 * On a fatal error the last 64 instruction addresses are printed.
 *
 * Exit 0 after N frames, 1 on a fatal error, 2 on bad usage. */
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bus.h"
#include "hwlog.h"
#include "interp.h"
#include "png.h"
#include "replay.h"
#include "sched.h"
#include "overlay.h"
#include "snes_adapter.h"
#include "spc700_host.h"
#include "ppu.h"
#include "apu.h"
#include "dsp.h"
#include "wav.h"

static jmp_buf fatal_jmp;
static char fatal_msg[256];

static void on_fatal(const char *msg)
{
    snprintf(fatal_msg, sizeof fatal_msg, "%s", msg);
    longjmp(fatal_jmp, 1);
}

/* Last executed instruction addresses (with the CPU state at each). */
#define TAIL 64
static struct {
    uint32_t at;
    CPU c;
} tail[TAIL];
static unsigned tail_n;

#define MAX_EXPECT 8
static uint32_t expect_pc[MAX_EXPECT];
static int expect_hit[MAX_EXPECT], n_expect;
static struct {
    long frame;
    uint32_t addr;
    unsigned val;
    int neq, seen, ok;
} expect_wram[MAX_EXPECT];
static int n_expect_wram;

#define MAX_WATCH 8
static struct {
    uint32_t at, len;
    uint8_t last[16];
} watch[MAX_WATCH];
static int n_watch;
static uint32_t prev_at;   /* the instruction before this one */

static void check_watch(uint32_t at)
{
    const uint8_t *w = bus_wram();
    for (int k = 0; k < n_watch; k++) {
        if (!memcmp(watch[k].last, w + watch[k].at, watch[k].len))
            continue;
        printf("watch $%05X: frame %ld line %d after $%06X:", watch[k].at, sched_frame_count(),
               sched_line(), prev_at);
        for (uint32_t b = 0; b < watch[k].len; b++)
            printf(" %02X", watch[k].last[b]);
        printf(" ->");
        for (uint32_t b = 0; b < watch[k].len; b++)
            printf(" %02X", w[watch[k].at + b]);
        printf("  (now at $%06X)\n", at);
        memcpy(watch[k].last, w + watch[k].at, watch[k].len);
    }
    prev_at = at;
}

#define MAX_AT 8
static uint32_t at_pc[MAX_AT];
static int at_seen[MAX_AT], n_at;

#define MAX_LOG 64
static uint32_t log_pc[MAX_LOG];
static int log_mem[MAX_LOG];   /* bank 0 address to dump, -1: DP */
static int n_log;

static uint32_t snap_pc = ~0u;
static const char *snap_dir;
static unsigned n_snaps;

static FILE *wram_entries;
static uint8_t *wram_entry_seen;   /* per WRAM address: bit 1 << (m << 1 | x) */
static uint32_t last_at = ~0u;

static FILE *first_exec;
static FILE *trace_out;
static unsigned long long trace_lo, trace_hi;
static long trace_from;
static uint8_t *seen_pc;   /* one bit per 24-bit address */

static void trace(const CPU *c, uint32_t at)
{
    if (first_exec && !(seen_pc[at >> 3] & 1 << (at & 7))) {
        seen_pc[at >> 3] |= (uint8_t)(1 << (at & 7));
        fprintf(first_exec, "%06X %llu\n", at, (unsigned long long)sched_clock());
    }
    if (trace_out) {
        unsigned long long t = sched_clock();
        if (t >= trace_lo && t <= trace_hi && sched_frame_count() >= trace_from)
            fprintf(trace_out, "%06X %llu %04X %04X\n", at, t, c->X, c->Y);
    }
    if (wram_entries && (at >> 16 == 0x7E || at >> 16 == 0x7F) && last_at != ~0u &&
        last_at >> 16 != 0x7E && last_at >> 16 != 0x7F) {
        uint8_t op = bus_peek(last_at);
        uint8_t bit = (uint8_t)(1u << (c->m << 1 | c->x));
        if (op != 0x60 && op != 0x6B && op != 0x40 && !(wram_entry_seen[at & 0x1FFFF] & bit)) {
            wram_entry_seen[at & 0x1FFFF] |= bit;
            fprintf(wram_entries, "%06X m%dx%d\n", at, c->m, c->x);
        }
    }
    last_at = at;
    if (at == snap_pc && n_snaps < 256) {
        char path[4096];
        snprintf(path, sizeof path, "%s/snap_%04u.bin", snap_dir, n_snaps++);
        FILE *f = fopen(path, "wb");
        if (!f || fwrite(bus_wram(), 1, 0x20000, f) != 0x20000)
            fprintf(stderr, "ct_boot: cannot write %s\n", path);
        if (f)
            fclose(f);
    }
    for (int k = 0; k < n_log; k++)
        if (at == log_pc[k]) {
            printf("entry %06X frame %ld line %d clock %llu A %04X X %04X Y %04X S %04X DP %04X "
                   "DB %02X PB %02X m %d x %d dp", at, sched_frame_count(), sched_line(),
                   (unsigned long long)sched_clock(), c->A, c->X, c->Y, c->S, c->DP, c->DB, c->PB,
                   c->m, c->x);
            for (unsigned b = 0; b < 16; b++) {
                unsigned a = (uint16_t)((log_mem[k] < 0 ? c->DP : (unsigned)log_mem[k]) + b);
                if (a < 0x2000)
                    printf(" %02X", bus_wram()[a]);
                else
                    printf(" --");
            }
            printf("\n");
        }
    for (int k = 0; k < n_at; k++)
        if (at == at_pc[k] && at_seen[k]++ < 20)
            printf("at $%06X: frame %ld line %d clock %llu\n", at, sched_frame_count(), sched_line(),
                   (unsigned long long)sched_clock());
    if (n_watch)
        check_watch(at);
    for (int k = 0; k < n_expect; k++)
        if (at == expect_pc[k])
            expect_hit[k] = 1;
    tail[tail_n % TAIL].at = at;
    tail[tail_n % TAIL].c = *c;
    tail_n++;
}

static replay input;

static uint64_t fnv(uint64_t h, const void *p, size_t n)
{
    const uint8_t *b = p;
    for (size_t k = 0; k < n; k++)
        h = (h ^ b[k]) * 0x100000001B3ull;
    return h;
}

static CPU cpu;

#define HASH0 0xCBF29CE484222325ull

/* One --hash-log line: frame, then an FNV-1a 64 hash per component, so a
   divergence names what differs. */
static void write_state_hashes(FILE *f, long frame)
{
    const CPU *c = &cpu;
    uint16_t r[] = {c->A, c->X, c->Y, c->S, c->DP, c->DB, c->PB, c->PC,
                    (uint16_t)(c->m | c->x << 1 | c->e << 2 | c->i << 3 | c->d << 4 | c->c << 5 |
                               c->z << 6 | c->v << 7 | c->n << 8)};
    Ppu *ppu = snes_hw_ppu();
    Apu *apu = snes_hw_apu();
    uint8_t spc[8];
    spc_host_regs(spc);
    uint8_t apu_regs[] = {spc[0], spc[1], spc[2], spc[3], spc[4], spc[5], spc[6],
                          spc[7], apu->dspAdr, apu->romReadable,
                          apu->inPorts[0], apu->inPorts[1], apu->inPorts[2], apu->inPorts[3],
                          apu->outPorts[0], apu->outPorts[1], apu->outPorts[2], apu->outPorts[3]};
    uint64_t h_apu = fnv(fnv(fnv(HASH0, apu_regs, sizeof apu_regs), apu->ram, sizeof apu->ram),
                         apu->dsp->ram, sizeof apu->dsp->ram);
    for (int t = 0; t < 3; t++) {
        const Timer *tm = &apu->timer[t];
        uint8_t v[] = {tm->cycles, tm->divider, tm->target, tm->counter, tm->enabled};
        h_apu = fnv(h_apu, v, sizeof v);
    }
    fprintf(f,
            "%ld cpu=%016llx wram=%016llx sram=%016llx vram=%016llx cgram=%016llx "
            "oam=%016llx frame=%016llx apu=%016llx\n",
            frame, (unsigned long long)fnv(HASH0, r, sizeof r),
            (unsigned long long)fnv(HASH0, bus_wram(), 0x20000),
            (unsigned long long)fnv(HASH0, bus_sram(), CT_SRAM_SIZE),
            (unsigned long long)fnv(HASH0, ppu->vram, sizeof ppu->vram),
            (unsigned long long)fnv(HASH0, ppu->cgram, sizeof ppu->cgram),
            (unsigned long long)fnv(HASH0, ppu->oam, sizeof ppu->oam),
            (unsigned long long)fnv(HASH0, sched_frame(), (size_t)SCHED_WIDTH * SCHED_HEIGHT * 4),
            (unsigned long long)h_apu);
}

/* The frame as tools/mesen_ref.py hashes it: 256x224, each pixel a
   little-endian 15-bit value, 5 bits per channel (B G R x bytes in). */
static uint64_t frame_hash(void)
{
    const uint8_t *p = sched_frame();
    uint64_t h = 0xCBF29CE484222325ull;
    for (int k = 0; k < SCHED_WIDTH * SCHED_HEIGHT; k++, p += 4) {
        uint16_t c = (uint16_t)(p[2] >> 3 | (p[1] >> 3) << 5 | (p[0] >> 3) << 10);
        uint8_t b[2] = {(uint8_t)c, (uint8_t)(c >> 8)};
        h = fnv(h, b, 2);
    }
    return h;
}

static int nonblack(const uint8_t *p, size_t n)
{
    for (size_t k = 0; k < n; k += 4)
        if (p[k] | p[k + 1] | p[k + 2])
            return 1;
    return 0;
}

static const char *dump;
static FILE *wav, *hash_log, *ref_log;
static const char *wram_path, *vram_path, *aram_path;
static long last_frame;
static int dumped_state;

/* --wram / --vram: WRAM, then VRAM, CGRAM and OAM, as they are at the
   last frame edge (a native call still running there doesn't move it). */
static void dump_state(void)
{
    dumped_state = 1;
    if (vram_path) {
        Ppu *ppu = snes_hw_ppu();
        FILE *f = fopen(vram_path, "wb");
        if (!f || fwrite(ppu->vram, 1, sizeof ppu->vram, f) != sizeof ppu->vram ||
            fwrite(ppu->cgram, 1, sizeof ppu->cgram, f) != sizeof ppu->cgram ||
            fwrite(ppu->oam, 1, sizeof ppu->oam, f) != sizeof ppu->oam)
            fprintf(stderr, "ct_boot: cannot write %s\n", vram_path);
        if (f)
            fclose(f);
    }
    if (aram_path) {
        FILE *f = fopen(aram_path, "wb");
        if (!f || fwrite(snes_hw_apu()->ram, 1, 0x10000, f) != 0x10000)
            fprintf(stderr, "ct_boot: cannot write %s\n", aram_path);
        if (f)
            fclose(f);
    }
    if (wram_path) {
        FILE *f = fopen(wram_path, "wb");
        if (!f || fwrite(bus_wram(), 1, 0x20000, f) != 0x20000)
            fprintf(stderr, "ct_boot: cannot write %s\n", wram_path);
        if (f)
            fclose(f);
    }
}
static long dumped, audible;

/* Frame edge: input for the next frame, then this frame's audio, dump
   and hash. */
static void on_frame(long f)
{
    for (int k = 0; k < n_expect_wram; k++)
        if (f == expect_wram[k].frame) {
            unsigned v = bus_wram()[expect_wram[k].addr];
            expect_wram[k].seen = 1;
            expect_wram[k].ok = expect_wram[k].neq ? v != expect_wram[k].val : v == expect_wram[k].val;
            if (!expect_wram[k].ok)
                printf("ct_boot: frame %ld: WRAM $%05X is $%02X, expected %s$%02X\n", f,
                       expect_wram[k].addr, v, expect_wram[k].neq ? "not " : "", expect_wram[k].val);
        }
    static int16_t audio[2048 * 2];   /* this frame's DSP output, 32 kHz */
    sched_set_joypad(0, replay_buttons(&input, f + 1));
    int n = sched_audio_take(audio, 2048);
    for (int k = 0; k < n * 2; k++)
        if (audio[k]) {
            audible++;
            break;
        }
    if (wav)
        wav_write(wav, audio, n);
    if (dump && png_dump_frame(dump, f, sched_frame(), SCHED_WIDTH, SCHED_HEIGHT) > 0)
        dumped++;
    if (hash_log)
        write_state_hashes(hash_log, f);
    if (f == last_frame && (wram_path || vram_path || aram_path))
        dump_state();
    if (ref_log)
        fprintf(ref_log, "%ld %016llx %016llx%s\n", f,
                (unsigned long long)fnv(0xCBF29CE484222325ull, bus_wram(), 0x20000),
                (unsigned long long)frame_hash(), sched_frame_in_dma() ? " dma" : "");
}

/* The script's reset button (replay.h "reset F"). */
static int script_reset(long frame)
{
    return replay_reset(&input, frame);
}

int main(int argc, char **argv)
{
    static long frames = 60, min_nmis;    /* static: survive the longjmp */
    static int require_render, require_audio;
    static const char *wav_path, *hash_path;
    static int profile_top = -1;
    static double min_native = -1;
    static const char *needed;
    for (int k = 1; k < argc; k++) {
        if (!strcmp(argv[k], "--frames") && k + 1 < argc)
            frames = atol(argv[++k]);
        else if (!strcmp(argv[k], "--dump") && k + 1 < argc)
            dump = argv[++k];
        else if (!strcmp(argv[k], "--needed-hw") && k + 1 < argc)
            needed = argv[++k];
        else if (!strcmp(argv[k], "--min-nmis") && k + 1 < argc)
            min_nmis = atol(argv[++k]);
        else if (!strcmp(argv[k], "--require-render"))
            require_render = 1;
        else if (!strcmp(argv[k], "--require-audio"))
            require_audio = 1;
        else if (!strcmp(argv[k], "--wav") && k + 1 < argc)
            wav_path = argv[++k];
        else if (!strcmp(argv[k], "--input") && k + 1 < argc) {
            if (replay_add(&input, argv[++k])) {
                fprintf(stderr, "ct_boot: bad --input %s\n", argv[k]);
                return 2;
            }
        } else if (!strcmp(argv[k], "--script") && k + 1 < argc) {
            if (replay_load(&input, argv[++k])) {
                fprintf(stderr, "ct_boot: bad --script %s\n", argv[k]);
                return 2;
            }
        } else if (!strcmp(argv[k], "--aram") && k + 1 < argc) {
            aram_path = argv[++k];
        } else if (!strcmp(argv[k], "--vram") && k + 1 < argc) {
            vram_path = argv[++k];
        } else if (!strcmp(argv[k], "--wram") && k + 1 < argc) {
            wram_path = argv[++k];
        } else if (!strcmp(argv[k], "--ref-log") && k + 1 < argc) {
            if (!(ref_log = fopen(argv[++k], "w"))) {
                fprintf(stderr, "ct_boot: cannot write %s\n", argv[k]);
                return 2;
            }
        } else if (!strcmp(argv[k], "--hash-log") && k + 1 < argc) {
            hash_path = argv[++k];
        } else if (!strcmp(argv[k], "--min-native") && k + 1 < argc) {
            min_native = atof(argv[++k]);
        } else if (!strcmp(argv[k], "--profile") && k + 1 < argc) {
            profile_top = atoi(argv[++k]);
        } else if (!strcmp(argv[k], "--watch") && k + 1 < argc && n_watch < MAX_WATCH) {
            unsigned a = 0, n = 2;
            if (sscanf(argv[++k], "%x:%u", &a, &n) < 1 || a > 0x1FFFF || !n || n > 16 ||
                a + n > 0x20000) {
                fprintf(stderr, "ct_boot: bad --watch %s\n", argv[k]);
                return 2;
            }
            watch[n_watch].at = a;
            watch[n_watch++].len = n;
        } else if (!strcmp(argv[k], "--at") && k + 1 < argc && n_at < MAX_AT) {
            at_pc[n_at++] = (uint32_t)strtoul(argv[++k], NULL, 16);
        } else if (!strcmp(argv[k], "--wram-entries") && k + 1 < argc) {
            if (!(wram_entries = fopen(argv[++k], "w")) || !(wram_entry_seen = calloc(0x20000, 1))) {
                fprintf(stderr, "ct_boot: cannot write %s\n", argv[k]);
                return 2;
            }
        } else if (!strcmp(argv[k], "--overlay-strict")) {
            ct_overlay_strict = 1;
        } else if (!strcmp(argv[k], "--overlay-fallback")) {
            ct_overlay_strict = 0;
        } else if (!strcmp(argv[k], "--snap-at") && k + 2 < argc) {
            snap_pc = (uint32_t)strtoul(argv[++k], NULL, 16);
            snap_dir = argv[++k];
        } else if (!strcmp(argv[k], "--log-entry") && k + 1 < argc && n_log < MAX_LOG) {
            char *end;
            log_pc[n_log] = (uint32_t)strtoul(argv[++k], &end, 16);
            log_mem[n_log++] = *end == ':' ? (int)strtoul(end + 1, NULL, 16) : -1;
        } else if (!strcmp(argv[k], "--first-exec") && k + 1 < argc) {
            if (!(first_exec = fopen(argv[++k], "w")) || !(seen_pc = calloc(1 << 21, 1))) {
                fprintf(stderr, "ct_boot: cannot write %s\n", argv[k]);
                return 2;
            }
        } else if (!strcmp(argv[k], "--trace") && k + 2 < argc) {
            int got = sscanf(argv[++k], "%llu:%llu@%ld", &trace_lo, &trace_hi, &trace_from);
            if (got < 2 || trace_lo > trace_hi) {
                fprintf(stderr, "ct_boot: bad --trace %s\n", argv[k]);
                return 2;
            }
            if (!(trace_out = fopen(argv[++k], "w"))) {
                fprintf(stderr, "ct_boot: cannot write %s\n", argv[k]);
                return 2;
            }
        } else if (!strcmp(argv[k], "--ref-quirk") && k + 1 < argc) {
            if (!strcmp(argv[++k], "mesen-dma-count8"))
                snes_ref_quirks |= SNES_QUIRK_MESEN_DMA_COUNT8;
            else {
                fprintf(stderr, "ct_boot: unknown --ref-quirk %s\n", argv[k]);
                return 2;
            }
        } else if (!strcmp(argv[k], "--interp-only")) {
            sched_set_native(0);
        } else if (!strcmp(argv[k], "--expect-wram") && k + 1 < argc && n_expect_wram < MAX_EXPECT) {
            long f;
            unsigned a, v;
            char op[3] = {0};
            if (sscanf(argv[++k], "%ld:%x%2[!=]%x", &f, &a, op, &v) != 4 || a > 0x1FFFF || v > 0xFF ||
                (strcmp(op, "=") && strcmp(op, "!="))) {
                fprintf(stderr, "ct_boot: bad --expect-wram %s\n", argv[k]);
                return 2;
            }
            expect_wram[n_expect_wram].frame = f;
            expect_wram[n_expect_wram].addr = a;
            expect_wram[n_expect_wram].val = v;
            expect_wram[n_expect_wram++].neq = op[0] == '!';
        } else if (!strcmp(argv[k], "--expect-pc") && k + 1 < argc && n_expect < MAX_EXPECT)
            expect_pc[n_expect++] = (uint32_t)strtoul(argv[++k], NULL, 16);
        else {
            fprintf(stderr, "usage: ct_boot [--frames N] [--dump DIR] [--needed-hw FILE] "
                            "[--min-nmis K] [--require-render] [--wav FILE] "
                            "[--require-audio] [--input F1-F2:BUTTONS] [--script FILE] "
                            "[--expect-pc ADDR] [--expect-wram F:ADDR=V] [--hash-log FILE] [--ref-log FILE] [--wram FILE] [--vram FILE] [--aram FILE] [--profile N] [--min-native P] [--watch A[:N]] [--at ADDR] [--log-entry ADDR[:MEM]] [--snap-at ADDR DIR] [--wram-entries FILE] [--overlay-strict|--overlay-fallback] [--first-exec FILE] [--trace LO:HI[@N] FILE] [--ref-quirk NAME] [--interp-only]\n");
            return 2;
        }
    }

    static char stop[512];
    bus_init(NULL);
    interp_reset(&cpu);
    sched_init(&cpu);
    ct_fatal_hook = on_fatal;
    ct_trace_hook = trace;

    if (wav_path && !(wav = wav_open(wav_path, SCHED_AUDIO_HZ))) {
        fprintf(stderr, "ct_boot: cannot write %s\n", wav_path);
        return 2;
    }
    if (hash_path && !(hash_log = fopen(hash_path, "w"))) {
        fprintf(stderr, "ct_boot: cannot write %s\n", hash_path);
        return 2;
    }
    if (setjmp(fatal_jmp)) {
        long f = sched_frame_count();
        int l = sched_line();
        printf("ct_boot: stopped in frame %ld, line %d, PC $%02X%04X: %s\n", f, l, cpu.PB,
               cpu.PC, fatal_msg);
        printf("last instructions:\n");
        for (unsigned k = tail_n > TAIL ? tail_n - TAIL : 0; k < tail_n; k++) {
            const CPU *c = &tail[k % TAIL].c;
            printf("  $%06X A=%04X X=%04X Y=%04X S=%04X DP=%04X DB=%02X m%dx%d\n",
                   tail[k % TAIL].at, c->A, c->X, c->Y, c->S, c->DP, c->DB, c->m, c->x);
        }
        snprintf(stop, sizeof stop, "frame %ld, line %d, PC $%02X%04X: %s", f, l, cpu.PB,
                 cpu.PC, fatal_msg);
        if (needed && hw_needed_write(needed, "ct_boot", stop))
            fprintf(stderr, "ct_boot: cannot write %s\n", needed);
        return 1;
    }
    last_frame = frames;
    sched_set_frame_hook(on_frame);
    sched_set_joypad(0, replay_buttons(&input, 1));
    sched_set_reset_hook(script_reset);
    while (sched_frame_count() < frames)
        sched_run_frame();
    if (wav)
        wav_close(wav);
    if (hash_log)
        fclose(hash_log);
    if (first_exec)
        fclose(first_exec);
    if (trace_out)
        fclose(trace_out);
    if (wram_entries)
        fclose(wram_entries);
    if (ref_log)
        fclose(ref_log);
    if ((wram_path || vram_path) && !dumped_state)
        dump_state();   /* stopped before the last frame edge (fatal) */
    printf("ct_boot: %ld frames, %ld NMIs, %ld frames dumped, %ld with audio, PC $%02X%04X\n",
           sched_frame_count(), sched_nmi_count(), (long)dumped, (long)audible, cpu.PB, cpu.PC);
    if (require_audio && !audible)
        printf("ct_boot: no audio\n");
    for (unsigned k = 0; k < hw_note_count(); k++)
        printf("ct_boot: stubbed: %s\n", hw_note_text(k));
    uint64_t nat = sched_native_insns(), all = nat + sched_interp_insns();
    double native_pct = all ? 100.0 * (double)nat / (double)all : 0.0;
    if (profile_top >= 0)
        sched_profile_report(stdout, profile_top);
    if (min_native >= 0 && native_pct < min_native)
        printf("ct_boot: %.2f%% native, below --min-native %.2f\n", native_pct, min_native);
    if (needed && hw_needed_write(needed, "ct_boot", NULL))
        fprintf(stderr, "ct_boot: cannot write %s\n", needed);
    int reached = 1;
    for (int k = 0; k < n_expect; k++)
        if (!expect_hit[k]) {
            printf("ct_boot: $%06X never ran\n", expect_pc[k]);
            reached = 0;
        }
    for (int k = 0; k < n_expect_wram; k++) {
        if (!expect_wram[k].seen)
            printf("ct_boot: frame %ld never reached for --expect-wram\n", expect_wram[k].frame);
        if (!expect_wram[k].ok)
            reached = 0;
    }
    int rendered = nonblack(sched_frame(), (size_t)SCHED_WIDTH * SCHED_HEIGHT * 4);
    if (require_render && !rendered)
        printf("ct_boot: last frame is black\n");
    return sched_nmi_count() >= min_nmis && (rendered || !require_render) &&
           (min_native < 0 || native_pct >= min_native) &&
           (audible || !require_audio) && reached ? 0 : 1;
}
