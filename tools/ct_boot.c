/* Boot probe: run Chrono Trigger from reset in the system-mode interpreter
 * under the frame scheduler, headless.
 *
 * usage: ct_boot [--frames N] [--dump DIR] [--needed-hw FILE]
 *
 * --dump DIR       write DIR/frame_NNNNN.png for every frame the PPU drew
 *                  anything into (any nonblack pixel) that differs from the
 *                  last one written
 * --needed-hw FILE on a fatal error (unimplemented register, opcode, ...),
 *                  write FILE describing it: where the boot stops next
 *
 * --min-nmis K     exit 1 unless at least K NMIs were taken
 *
 * On a fatal error the last 64 instruction addresses are printed.
 *
 * Exit 0 after N frames, 1 on a fatal error, 2 on bad usage. */
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bus.h"
#include "interp.h"
#include "png.h"
#include "sched.h"

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

static void trace(const CPU *c, uint32_t at)
{
    tail[tail_n % TAIL].at = at;
    tail[tail_n % TAIL].c = *c;
    tail_n++;
}

static uint32_t hash(const uint8_t *p, size_t n)
{
    uint32_t h = 2166136261u;
    for (size_t k = 0; k < n; k++)
        h = (h ^ p[k]) * 16777619u;
    return h;
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
        else {
            fprintf(stderr, "usage: ct_boot [--frames N] [--dump DIR] [--needed-hw FILE] "
                            "[--min-nmis K]\n");
            return 2;
        }
    }

    static CPU cpu;
    bus_init(NULL);
    interp_reset(&cpu);
    sched_init(&cpu);
    ct_fatal_hook = on_fatal;
    ct_trace_hook = trace;

    static volatile long dumped;
    static volatile uint32_t last;
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
        if (needed) {
            FILE *fp = fopen(needed, "w");
            if (fp) {
                fprintf(fp, "# Needed hardware\n\n"
                            "Written by `ct_boot --needed-hw`. Do not edit by hand.\n\n"
                            "Booting from reset in the system-mode interpreter stops at:\n\n"
                            "- frame %ld, line %d, PC $%02X%04X: %s\n",
                        f, l, cpu.PB, cpu.PC, fatal_msg);
                fclose(fp);
            }
        }
        return 1;
    }
    for (long f = 0; f < frames; f++) {
        sched_run_frame();
        const uint8_t *fb = sched_frame();
        size_t n = (size_t)SCHED_WIDTH * SCHED_HEIGHT * 4;
        if (dump && nonblack(fb, n) && hash(fb, n) != last) {
            char path[4096];
            snprintf(path, sizeof path, "%s/frame_%05ld.png", dump, f + 1);
            if (png_write_bgrx(path, fb, SCHED_WIDTH, SCHED_HEIGHT))
                fprintf(stderr, "ct_boot: cannot write %s\n", path);
            last = hash(fb, n);
            dumped++;
        }
    }
    printf("ct_boot: %ld frames, %ld NMIs, %ld frames dumped, PC $%02X%04X\n",
           sched_frame_count(), sched_nmi_count(), (long)dumped, cpu.PB, cpu.PC);
    return sched_nmi_count() >= min_nmis ? 0 : 1;
}
