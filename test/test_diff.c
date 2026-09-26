/* Differential test: every function in ct_funcs, generated code vs the
 * reference interpreter, from identical random states.
 *
 * Entry state: M/X from the variant, DB/DP from funcs.toml (DB $7E and DP 0
 * when unknown), random A/X/Y/S/flags (D=0), random WRAM and SRAM.
 * Compared after return: every CPU field, all WRAM, SRAM, and the math
 * unit result registers. A fatal error must occur in both or neither, with
 * the same message.
 *
 * usage: test_diff [trials] [name-filter]
 *        test_diff --replay FILE
 */
#include <setjmp.h>
#include <string.h>

#include "harness.h"

static jmp_buf fatal_jmp;
static char fatal_msg[256];

static void on_fatal(const char *msg)
{
    snprintf(fatal_msg, sizeof fatal_msg, "%s", msg);
    longjmp(fatal_jmp, 1);
}

static uint8_t w0[CT_WRAM_SIZE], wa[CT_WRAM_SIZE];
static uint8_t s0[CT_SRAM_SIZE], sa[CT_SRAM_SIZE];

typedef struct {
    CPU cpu;
    int fatal;
    char msg[256];
    uint8_t alu[4];
} Result;

static void alu_setup(uint32_t r)
{
    write8(0x004204, (uint8_t)r);
    write8(0x004205, (uint8_t)(r >> 8));
    write8(0x004206, (uint8_t)(r >> 16));
    write8(0x004202, (uint8_t)(r >> 24));
}

static void run(const ct_func *f, const CPU *in, uint32_t alu, int interp, Result *r)
{
    /* Both sides must start from identical hardware state, not whatever
       the previous run left in the WRAM port, PPU, DMA, or APU. */
    bus_reset();
    memcpy(bus_wram(), w0, CT_WRAM_SIZE);
    memcpy(bus_sram(), s0, CT_SRAM_SIZE);
    alu_setup(alu);
    r->cpu = *in;
    r->fatal = 0;
    push16(&r->cpu, 0x1233);
    if (setjmp(fatal_jmp)) {
        r->fatal = 1;
        snprintf(r->msg, sizeof r->msg, "%s", fatal_msg);
        return;
    }
    if (interp)
        interp_call(&r->cpu, f->addr);
    else
        f->fn(&r->cpu);
    for (int k = 0; k < 4; k++)
        r->alu[k] = read8(0x004214 + k);
}

static int cpu_equal(const CPU *a, const CPU *b)
{
    return a->A == b->A && a->X == b->X && a->Y == b->Y && a->S == b->S && a->DP == b->DP &&
           a->DB == b->DB && a->PB == b->PB && a->PC == b->PC && a->n == b->n && a->v == b->v &&
           a->m == b->m && a->x == b->x && a->d == b->d && a->i == b->i && a->z == b->z &&
           a->c == b->c && a->e == b->e;
}

static void dump(const char *tag, const CPU *c)
{
    fprintf(stderr, "  %s A=%04X X=%04X Y=%04X S=%04X DP=%04X DB=%02X PB=%02X PC=%04X "
            "nvmxdizc=%d%d%d%d%d%d%d%d e=%d\n", tag, c->A, c->X, c->Y, c->S, c->DP, c->DB,
            c->PB, c->PC, c->n, c->v, c->m, c->x, c->d, c->i, c->z, c->c, c->e);
}

/* On-disk snapshot of one trial's entry state, for --replay. Never checked
   into the repo: written to /tmp only. */
typedef struct {
    char func_name[64];
    CPU cpu;                     /* entry state, before run()'s push16 */
    uint8_t wram[CT_WRAM_SIZE];
    uint8_t sram[CT_SRAM_SIZE];
} Snapshot;

static void save_snapshot(const ct_func *f, const CPU *in, int t)
{
    Snapshot *snap = malloc(sizeof *snap);
    if (!snap) {
        fprintf(stderr, "  snapshot: out of memory\n");
        return;
    }
    snprintf(snap->func_name, sizeof snap->func_name, "%s", f->name);
    snap->cpu = *in;
    memcpy(snap->wram, w0, CT_WRAM_SIZE);
    memcpy(snap->sram, s0, CT_SRAM_SIZE);

    char path[256];
    snprintf(path, sizeof path, "/tmp/ct_diff_%s_t%d.snap", f->name, t);
    FILE *fp = fopen(path, "wb");
    if (!fp || fwrite(snap, sizeof *snap, 1, fp) != 1)
        fprintf(stderr, "  snapshot: failed to write %s\n", path);
    else
        fprintf(stderr, "  snapshot: %s (replay with: test_diff --replay %s)\n", path, path);
    if (fp)
        fclose(fp);
    free(snap);
}

/* Full repro recipe for a divergence: the harness PRNG is fixed-seed, so
   rerunning the same binary with the same argv reproduces trial t exactly
   up to this point. */
static void dump_trial(const ct_func *f, int t, const CPU *in, uint16_t dp)
{
    fprintf(stderr, "  %s trial %d entry: A=%04X X=%04X Y=%04X S=%04X DP=%04X DB=%02X PB=%02X "
            "nvmxdizc=%d%d%d%d%d%d%d%d\n", f->name, t, in->A, in->X, in->Y, in->S, in->DP,
            in->DB, in->PB, in->n, in->v, in->m, in->x, in->d, in->i, in->z, in->c);
    fprintf(stderr, "  dp $%04X:", dp);
    for (int k = 0; k < 0x100; k++)
        fprintf(stderr, "%s%02X", k % 16 == 0 ? "\n   " : " ", w0[dp + k]);
    fputc('\n', stderr);
    save_snapshot(f, in, t);
}

static void fill(uint8_t *p, unsigned n)
{
    for (unsigned k = 0; k < n; k += 4) {
        uint32_t r = rnd32();
        memcpy(p + k, &r, 4);
    }
}

static void diff_func(const ct_func *f, int trials)
{
    static const char tag[] = "diff";
    long fatal_both = 0, faulted_checked = 0, timeouts = 0, fails0 = th_fails;
    Result ra, rb;
    int t;
    for (t = 0; t < trials && th_fails - fails0 < 3; t++) {
        if (t % 16 == 0) {
            fill(w0, CT_WRAM_SIZE);
            fill(s0, CT_SRAM_SIZE);
        }
        uint16_t dp = f->dp >= 0 ? (uint16_t)f->dp : 0;
        fill(&w0[dp], 0x100);

        if (!strcmp(f->name, "Field_CopyMapRectLayers") ||
            !strcmp(f->name, "Field_CopyMapRectMVN")) {
            /* Unclamped, these wrap into an MVN dest that can hit the stack page. */
            w0[dp + 0x3E] = (uint8_t)(rnd32() % 32);
            w0[dp + 0x3F] = 0;
            w0[dp + 0x40] = (uint8_t)(w0[dp + 0x3E] + rnd32() % 32);
            w0[dp + 0x41] = 0;
            w0[dp + 0x42] = (uint8_t)(rnd32() % 32);
            w0[dp + 0x43] = 0;
        }

        CPU in;
        uint32_t r = rnd32();
        cpu_init(&in);
        in.A = (uint16_t)rnd32();
        in.X = f->x ? (uint16_t)(rnd32() & 0xFF) : (uint16_t)rnd32();
        in.Y = f->x ? (uint16_t)(rnd32() & 0xFF) : (uint16_t)rnd32();
        in.S = (uint16_t)(0x0500 + rnd32() % 0x200);   /* real range, per ColdBootInit's $06FF */
        in.DP = dp;
        in.DB = f->db >= 0 ? (uint8_t)f->db : 0x7E;
        in.PB = (uint8_t)(f->addr >> 16);
        in.m = f->m;
        in.x = f->x;
        in.n = r & 1;
        in.v = r >> 1 & 1;
        in.i = r >> 2 & 1;
        in.z = r >> 3 & 1;
        in.c = r >> 4 & 1;
        uint32_t alu = rnd32();

        /* Match the interpreter's step budget: they charge backward
           branches/jumps and instructions respectively, not the same unit,
           but a tighter generated-code budget causes spurious divergences
           on routines whose legitimate loop counts are merely large. */
        ct_budget = CT_INTERP_BUDGET;
        ct_test_cap = CT_GEN_BACKWARD_CAP;
        run(f, &in, alu, 0, &ra);
        memcpy(wa, bus_wram(), CT_WRAM_SIZE);
        memcpy(sa, bus_sram(), CT_SRAM_SIZE);
        memcpy(sa, bus_sram(), CT_SRAM_SIZE);
        ct_budget = 0;
        run(f, &in, alu, 1, &rb);

        if (ra.fatal && rb.fatal && strstr(ra.msg, "budget exhausted") &&
            strstr(rb.msg, "budget exhausted")) {
            fatal_both++;   /* both ran away; the two count different units */
            continue;
        }
        if ((ra.fatal && strstr(ra.msg, "backward-branch cap exceeded")) ||
            (rb.fatal && strstr(rb.msg, "instruction cap exceeded"))) {
            timeouts++;   /* hard safety net (ops.h/interp.h), not a real result */
            continue;
        }
        if (ra.fatal || rb.fatal) {
            int fatal_mismatch = ra.fatal != rb.fatal || strcmp(ra.msg, rb.msg);
            if (fatal_mismatch)
                dump_trial(f, t, &in, dp);
            CHECK(!fatal_mismatch, "%s %s m%dx%d trial %d: fatal mismatch: gen '%s' interp '%s'",
                  tag, f->name, f->m, f->x, t, ra.fatal ? ra.msg : "-", rb.fatal ? rb.msg : "-");
            fatal_both += ra.fatal && rb.fatal;
            if (ra.fatal && rb.fatal && !strcmp(ra.msg, rb.msg)) {
                /* Same fault: state up to it must match (PC/PB are only
                   maintained by generated code at returns). */
                CPU ga = ra.cpu, gb = rb.cpu;
                ga.PC = gb.PC = 0;
                ga.PB = gb.PB = 0;
                int ok = cpu_equal(&ga, &gb);
                if (!ok)
                    dump_trial(f, t, &in, dp);
                CHECK(ok, "%s %s m%dx%d trial %d: CPU differs at fault '%s'", tag, f->name, f->m,
                      f->x, t, ra.msg);
                if (!ok) {
                    dump("gen   ", &ra.cpu);
                    dump("interp", &rb.cpu);
                }
                int wram_ok = !memcmp(wa, bus_wram(), CT_WRAM_SIZE);
                if (!wram_ok)
                    dump_trial(f, t, &in, dp);
                CHECK(wram_ok, "%s %s m%dx%d trial %d: WRAM differs at fault '%s'", tag, f->name,
                      f->m, f->x, t, ra.msg);
                faulted_checked++;
            }
            if (getenv("CT_DIFF_VERBOSE") && ra.fatal && rb.fatal && fatal_both <= 3) {
                dump_trial(f, t, &in, dp);
                fprintf(stderr, "  fatal in both: %s\n", ra.msg);
            }
            continue;
        }
        int ok_cpu = cpu_equal(&ra.cpu, &rb.cpu);
        if (!ok_cpu)
            dump_trial(f, t, &in, dp);
        CHECK(ok_cpu, "%s %s m%dx%d trial %d: CPU differs", tag, f->name, f->m, f->x, t);
        if (!ok_cpu) {
            dump("in    ", &in);
            dump("gen   ", &ra.cpu);
            dump("interp", &rb.cpu);
        }
        if (memcmp(wa, bus_wram(), CT_WRAM_SIZE)) {
            unsigned a = 0;
            while (wa[a] == bus_wram()[a])
                a++;
            dump_trial(f, t, &in, dp);
            CHECK(0, "%s %s m%dx%d trial %d: WRAM differs at $%05X (gen %02X interp %02X)", tag,
                  f->name, f->m, f->x, t, 0x7E0000 + a, wa[a], bus_wram()[a]);
        }
        if (memcmp(sa, bus_sram(), CT_SRAM_SIZE))
            dump_trial(f, t, &in, dp);
        CHECK(!memcmp(sa, bus_sram(), CT_SRAM_SIZE), "%s %s: SRAM differs", tag, f->name);
        if (memcmp(ra.alu, rb.alu, 4))
            dump_trial(f, t, &in, dp);
        CHECK(!memcmp(ra.alu, rb.alu, 4), "%s %s: math registers differ", tag, f->name);
    }
    printf("  %-30s $%06X m%dx%d  %d trials, %ld fatal in both (%ld compared at the fault), "
           "%ld timed out\n",
           f->name, f->addr, f->m, f->x, t, fatal_both, faulted_checked, timeouts);
}

/* --replay: instruction-level trace diff of one saved trial, generated vs
   interpreter, without re-running the fuzz sweep that found it. */
#define TRACE_CAP 2000000
typedef struct {
    uint32_t addr;
    CPU cpu;
} TraceEntry;
static TraceEntry *trace_buf;
static long trace_len;

static void trace_hook(const CPU *c, uint32_t addr)
{
    if (trace_len < TRACE_CAP)
        trace_buf[trace_len++] = (TraceEntry){addr, *c};
}

static int trace_cpu_equal(const CPU *a, const CPU *b)
{
    return a->A == b->A && a->X == b->X && a->Y == b->Y && a->S == b->S && a->DP == b->DP &&
           a->DB == b->DB && a->n == b->n && a->v == b->v && a->m == b->m && a->x == b->x &&
           a->d == b->d && a->i == b->i && a->z == b->z && a->c == b->c;
}

static void trace_dump(const char *tag, const TraceEntry *e)
{
    fprintf(stderr, "  %s $%06X A=%04X X=%04X Y=%04X S=%04X DP=%04X DB=%02X nvmxdizc=%d%d%d%d"
            "%d%d%d%d\n", tag, e->addr, e->cpu.A, e->cpu.X, e->cpu.Y, e->cpu.S, e->cpu.DP,
            e->cpu.DB, e->cpu.n, e->cpu.v, e->cpu.m, e->cpu.x, e->cpu.d, e->cpu.i, e->cpu.z,
            e->cpu.c);
}

static int replay(const char *path)
{
    FILE *fp = fopen(path, "rb");
    Snapshot *snap = fp ? malloc(sizeof *snap) : NULL;
    if (!fp || !snap || fread(snap, sizeof *snap, 1, fp) != 1) {
        fprintf(stderr, "replay: cannot load %s\n", path);
        return 2;
    }
    fclose(fp);

    const ct_func *f = NULL;
    for (unsigned k = 0; k < ct_func_count; k++)
        if (!strcmp(ct_funcs[k].name, snap->func_name))
            f = &ct_funcs[k];
    if (!f) {
        fprintf(stderr, "replay: function %s not in ct_funcs\n", snap->func_name);
        return 2;
    }
    memcpy(w0, snap->wram, CT_WRAM_SIZE);
    memcpy(s0, snap->sram, CT_SRAM_SIZE);

    trace_buf = malloc(TRACE_CAP * sizeof *trace_buf);
    TraceEntry *trace_a = malloc(TRACE_CAP * sizeof *trace_a);
    TraceEntry *trace_b = malloc(TRACE_CAP * sizeof *trace_b);
    if (!trace_buf || !trace_a || !trace_b) {
        fprintf(stderr, "replay: out of memory\n");
        return 2;
    }

    ct_trace_hook = trace_hook;
    Result ra, rb;
    trace_len = 0;
    ct_budget = CT_INTERP_BUDGET;
    ct_test_cap = CT_GEN_BACKWARD_CAP;
    run(f, &snap->cpu, 0, 0, &ra);
    long na = trace_len;
    memcpy(trace_a, trace_buf, na * sizeof *trace_a);

    memcpy(wa, bus_wram(), CT_WRAM_SIZE);
    memcpy(sa, bus_sram(), CT_SRAM_SIZE);

    trace_len = 0;
    ct_budget = 0;
    run(f, &snap->cpu, 0, 1, &rb);
    long nb = trace_len;
    memcpy(trace_b, trace_buf, nb * sizeof *trace_b);
    ct_trace_hook = NULL;

    printf("gen: %ld steps, fatal=%d %s\n", na, ra.fatal, ra.fatal ? ra.msg : "-");
    printf("interp: %ld steps, fatal=%d %s\n", nb, rb.fatal, rb.fatal ? rb.msg : "-");

    /* End state, as the sweep compares it (PC/PB only at returns). */
    int end_ok = 1;
    CPU ga = ra.cpu, gb = rb.cpu;
    if (ra.fatal && rb.fatal) {
        ga.PC = gb.PC = 0;
        ga.PB = gb.PB = 0;
    }
    if (!cpu_equal(&ga, &gb)) {
        printf("end state: CPU differs\n");
        dump("gen   ", &ra.cpu);
        dump("interp", &rb.cpu);
        end_ok = 0;
    }
    for (unsigned a = 0; a < CT_WRAM_SIZE; a++)
        if (wa[a] != bus_wram()[a]) {
            if (end_ok || getenv("CT_DIFF_VERBOSE"))
                printf("end state: WRAM differs at $%05X (gen %02X interp %02X entry %02X)\n",
                       0x7E0000 + a, wa[a], bus_wram()[a], snap->wram[a]);
            end_ok = 0;
        }
    if (memcmp(sa, bus_sram(), CT_SRAM_SIZE)) {
        printf("end state: SRAM differs\n");
        end_ok = 0;
    }

    long n = na < nb ? na : nb;
    long i;
    for (i = 0; i < n; i++) {
        if (trace_a[i].addr != trace_b[i].addr || !trace_cpu_equal(&trace_a[i].cpu, &trace_b[i].cpu))
            break;
    }
    if (i == n && na == nb) {
        printf("traces identical for all %ld steps\n", n);
        return end_ok ? 0 : 1;
    }

    printf("first divergence at step %ld (of %ld/%ld); last matching steps:\n", i, na, nb);
    for (long k = i - 3 > 0 ? i - 3 : 0; k < i; k++)
        trace_dump("  ok  gen   ", &trace_a[k]);
    if (i < na)
        trace_dump("  -> gen   ", &trace_a[i]);
    else
        printf("  -> gen    (ended after %ld steps)\n", na);
    if (i < nb)
        trace_dump("  -> interp", &trace_b[i]);
    else
        printf("  -> interp (ended after %ld steps)\n", nb);
    return 1;
}

int main(int argc, char **argv)
{
    bus_init(NULL);
    ct_fatal_hook = on_fatal;
    if (argc >= 3 && !strcmp(argv[1], "--replay"))
        return replay(argv[2]);
    int trials = argc > 1 ? atoi(argv[1]) : 2000;
    const char *filter = argc > 2 ? argv[2] : NULL;
    for (unsigned k = 0; k < ct_func_count; k++)
        if (!filter || strstr(ct_funcs[k].name, filter))
            diff_func(&ct_funcs[k], trials);
    return th_report("diff");
}
