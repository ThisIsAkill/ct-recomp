/* Boot probe: run Chrono Trigger from reset in the system-mode interpreter
 * under the frame scheduler, headless.
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
 * --expect-pc ADDR exit 1 unless the instruction at ADDR (hex) ran
 *                  (repeatable)
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
#include "sched.h"
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

#define MAX_INPUT 32
static struct {
    long from, to;
    uint16_t buttons;
} input[MAX_INPUT];
static int n_input;

/* "F1-F2:start+a" or "F1-F2:0x1080"; returns 0 on success. */
static int parse_input(const char *arg)
{
    static const struct { const char *name; uint16_t bit; } names[] = {
        {"b", 0x8000}, {"y", 0x4000}, {"select", 0x2000}, {"start", 0x1000},
        {"up", 0x0800}, {"down", 0x0400}, {"left", 0x0200}, {"right", 0x0100},
        {"a", 0x0080}, {"x", 0x0040}, {"l", 0x0020}, {"r", 0x0010},
    };
    long from, to;
    char spec[128];
    if (n_input == MAX_INPUT || sscanf(arg, "%ld-%ld:%127s", &from, &to, spec) != 3 || from < 1 ||
        to < from)
        return -1;
    uint16_t b = 0;
    if (!strncmp(spec, "0x", 2)) {
        b = (uint16_t)strtoul(spec, NULL, 16);
    } else {
        for (char *t = strtok(spec, "+"); t; t = strtok(NULL, "+")) {
            unsigned k;
            for (k = 0; k < sizeof names / sizeof names[0] && strcmp(t, names[k].name); k++)
                ;
            if (k == sizeof names / sizeof names[0])
                return -1;
            b |= names[k].bit;
        }
    }
    input[n_input].from = from;
    input[n_input].to = to;
    input[n_input++].buttons = b;
    return 0;
}

static uint16_t buttons_at(long frame)
{
    uint16_t b = 0;
    for (int k = 0; k < n_input; k++)
        if (frame >= input[k].from && frame <= input[k].to)
            b |= input[k].buttons;
    return b;
}

static int nonblack(const uint8_t *p, size_t n)
{
    for (size_t k = 0; k < n; k += 4)
        if (p[k] | p[k + 1] | p[k + 2])
            return 1;
    return 0;
}

int main(int argc, char **argv)
{
    static long frames = 60, min_nmis;    /* static: survive the longjmp */
    static int require_render, require_audio;
    static const char *wav_path;
    static const char *dump, *needed;
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
            if (parse_input(argv[++k])) {
                fprintf(stderr, "ct_boot: bad --input %s\n", argv[k]);
                return 2;
            }
        } else if (!strcmp(argv[k], "--expect-pc") && k + 1 < argc && n_expect < MAX_EXPECT)
            expect_pc[n_expect++] = (uint32_t)strtoul(argv[++k], NULL, 16);
        else {
            fprintf(stderr, "usage: ct_boot [--frames N] [--dump DIR] [--needed-hw FILE] "
                            "[--min-nmis K] [--require-render] [--wav FILE] "
                            "[--require-audio] [--input F1-F2:BUTTONS] [--expect-pc ADDR]\n");
            return 2;
        }
    }

    static CPU cpu;
    static char stop[512];
    bus_init(NULL);
    interp_reset(&cpu);
    sched_init(&cpu);
    ct_fatal_hook = on_fatal;
    ct_trace_hook = trace;

    static volatile long dumped, audible;
    static FILE *wav;
    if (wav_path && !(wav = wav_open(wav_path, 48000))) {
        fprintf(stderr, "ct_boot: cannot write %s\n", wav_path);
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
    for (long f = 0; f < frames; f++) {
        static int16_t audio[800 * 2];   /* one frame at 48 kHz */
        sched_set_joypad(0, buttons_at(f + 1));
        sched_run_frame();
        sched_audio(audio, 800);
        for (int k = 0; k < 800 * 2; k++)
            if (audio[k]) {
                audible++;
                break;
            }
        if (wav)
            wav_write(wav, audio, 800);
        if (dump && png_dump_frame(dump, f + 1, sched_frame(), SCHED_WIDTH, SCHED_HEIGHT) > 0)
            dumped++;
    }
    if (wav)
        wav_close(wav);
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
