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
 * --hash-log FILE  write "frame hash" per frame to FILE: FNV-1a 64 of the
 *                  CPU registers, WRAM, SRAM, VRAM, CGRAM, OAM, the frame,
 *                  and the APU RAM and DSP registers, taken at the frame
 *                  edge (native and interpreted runs must agree)
 * --expect-pc ADDR exit 1 unless the instruction at ADDR (hex) ran
 *                  (repeatable)
 * --wram FILE      write the 128 KB of WRAM to FILE at the end of the run
 * --vram FILE      likewise the 64 KB of VRAM, then 512 bytes of CGRAM
 * --interp-only    run everything in the interpreter (no native dispatch)
 *
 * --min-nmis K     exit 1 unless at least K NMIs were taken
 * --require-render exit 1 if the last frame is all black
 * --wav FILE       write the audio (48 kHz stereo) to FILE
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
#include "snes_adapter.h"
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

static void trace(const CPU *c, uint32_t at)
{
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

static uint64_t state_hash(void)
{
    uint64_t h = 0xCBF29CE484222325ull;
    const CPU *c = &cpu;
    uint16_t r[] = {c->A, c->X, c->Y, c->S, c->DP, c->DB, c->PB, c->PC,
                    (uint16_t)(c->m | c->x << 1 | c->e << 2 | c->i << 3 | c->d << 4 | c->c << 5 |
                               c->z << 6 | c->v << 7 | c->n << 8)};
    h = fnv(h, r, sizeof r);
    h = fnv(h, bus_wram(), 0x20000);
    h = fnv(h, bus_sram(), CT_SRAM_SIZE);
    Ppu *ppu = snes_hw_ppu();
    h = fnv(h, ppu->vram, sizeof ppu->vram);
    h = fnv(h, ppu->cgram, sizeof ppu->cgram);
    h = fnv(h, ppu->oam, sizeof ppu->oam);
    h = fnv(h, sched_frame(), (size_t)SCHED_WIDTH * SCHED_HEIGHT * 4);
    Apu *apu = snes_hw_apu();
    h = fnv(h, apu->ram, sizeof apu->ram);
    h = fnv(h, apu->dsp->ram, sizeof apu->dsp->ram);
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
static FILE *wav, *hash_log;
static long dumped, audible;

/* Frame edge: input for the next frame, then this frame's audio, dump
   and hash. */
static void on_frame(long f)
{
    static int16_t audio[800 * 2];   /* one frame at 48 kHz */
    sched_set_joypad(0, replay_buttons(&input, f + 1));
    sched_audio(audio, 800);
    for (int k = 0; k < 800 * 2; k++)
        if (audio[k]) {
            audible++;
            break;
        }
    if (wav)
        wav_write(wav, audio, 800);
    if (dump && png_dump_frame(dump, f, sched_frame(), SCHED_WIDTH, SCHED_HEIGHT) > 0)
        dumped++;
    if (hash_log)
        fprintf(hash_log, "%ld %016llx\n", f, (unsigned long long)state_hash());
}

int main(int argc, char **argv)
{
    static long frames = 60, min_nmis;    /* static: survive the longjmp */
    static int require_render, require_audio;
    static const char *wav_path, *hash_path, *wram_path, *vram_path;
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
        } else if (!strcmp(argv[k], "--vram") && k + 1 < argc) {
            vram_path = argv[++k];
        } else if (!strcmp(argv[k], "--wram") && k + 1 < argc) {
            wram_path = argv[++k];
        } else if (!strcmp(argv[k], "--hash-log") && k + 1 < argc) {
            hash_path = argv[++k];
        } else if (!strcmp(argv[k], "--interp-only")) {
            sched_set_native(0);
        } else if (!strcmp(argv[k], "--expect-pc") && k + 1 < argc && n_expect < MAX_EXPECT)
            expect_pc[n_expect++] = (uint32_t)strtoul(argv[++k], NULL, 16);
        else {
            fprintf(stderr, "usage: ct_boot [--frames N] [--dump DIR] [--needed-hw FILE] "
                            "[--min-nmis K] [--require-render] [--wav FILE] "
                            "[--require-audio] [--input F1-F2:BUTTONS] [--script FILE] "
                            "[--expect-pc ADDR] [--hash-log FILE] [--wram FILE] [--vram FILE] [--interp-only]\n");
            return 2;
        }
    }

    static char stop[512];
    bus_init(NULL);
    interp_reset(&cpu);
    sched_init(&cpu);
    ct_fatal_hook = on_fatal;
    ct_trace_hook = trace;

    if (wav_path && !(wav = wav_open(wav_path, 48000))) {
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
    sched_set_frame_hook(on_frame);
    sched_set_joypad(0, replay_buttons(&input, 1));
    while (sched_frame_count() < frames)
        sched_run_frame();
    if (wav)
        wav_close(wav);
    if (hash_log)
        fclose(hash_log);
    if (vram_path) {
        Ppu *ppu = snes_hw_ppu();
        FILE *f = fopen(vram_path, "wb");
        if (!f || fwrite(ppu->vram, 1, sizeof ppu->vram, f) != sizeof ppu->vram ||
            fwrite(ppu->cgram, 1, sizeof ppu->cgram, f) != sizeof ppu->cgram)
            fprintf(stderr, "ct_boot: cannot write %s\n", vram_path);
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
    printf("ct_boot: %ld frames, %ld NMIs, %ld frames dumped, %ld with audio, PC $%02X%04X\n",
           sched_frame_count(), sched_nmi_count(), (long)dumped, (long)audible, cpu.PB, cpu.PC);
    if (require_audio && !audible)
        printf("ct_boot: no audio\n");
    for (unsigned k = 0; k < hw_note_count(); k++)
        printf("ct_boot: stubbed: %s\n", hw_note_text(k));
    if (needed && hw_needed_write(needed, "ct_boot", NULL))
        fprintf(stderr, "ct_boot: cannot write %s\n", needed);
    int reached = 1;
    for (int k = 0; k < n_expect; k++)
        if (!expect_hit[k]) {
            printf("ct_boot: $%06X never ran\n", expect_pc[k]);
            reached = 0;
        }
    int rendered = nonblack(sched_frame(), (size_t)SCHED_WIDTH * SCHED_HEIGHT * 4);
    if (require_render && !rendered)
        printf("ct_boot: last frame is black\n");
    return sched_nmi_count() >= min_nmis && (rendered || !require_render) &&
           (audible || !require_audio) && reached ? 0 : 1;
}
