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
    long fatal_both = 0, faulted_checked = 0, fails0 = th_fails;
    Result ra, rb;
    int t;
    for (t = 0; t < trials && th_fails - fails0 < 3; t++) {
        if (t % 16 == 0) {
            fill(w0, CT_WRAM_SIZE);
            fill(s0, CT_SRAM_SIZE);
        }
        uint16_t dp = f->dp >= 0 ? (uint16_t)f->dp : 0;
        fill(&w0[dp], 0x100);

        CPU in;
        uint32_t r = rnd32();
        cpu_init(&in);
        in.A = (uint16_t)rnd32();
        in.X = f->x ? (uint16_t)(rnd32() & 0xFF) : (uint16_t)rnd32();
        in.Y = f->x ? (uint16_t)(rnd32() & 0xFF) : (uint16_t)rnd32();
        in.S = (uint16_t)(0x1E00 + rnd32() % 0x1F0);
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

        ct_budget = 100000;
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
        if (ra.fatal || rb.fatal) {
            CHECK(ra.fatal == rb.fatal && !strcmp(ra.msg, rb.msg),
                  "%s %s m%dx%d trial %d: fatal mismatch: gen '%s' interp '%s'", tag, f->name,
                  f->m, f->x, t, ra.fatal ? ra.msg : "-", rb.fatal ? rb.msg : "-");
            fatal_both += ra.fatal && rb.fatal;
            if (ra.fatal && rb.fatal && !strcmp(ra.msg, rb.msg)) {
                /* Same fault: state up to it must match (PC/PB are only
                   maintained by generated code at returns). */
                CPU ga = ra.cpu, gb = rb.cpu;
                ga.PC = gb.PC = 0;
                ga.PB = gb.PB = 0;
                int ok = cpu_equal(&ga, &gb);
                CHECK(ok, "%s %s m%dx%d trial %d: CPU differs at fault '%s'", tag, f->name, f->m,
                      f->x, t, ra.msg);
                if (!ok) {
                    dump("gen   ", &ra.cpu);
                    dump("interp", &rb.cpu);
                }
                CHECK(!memcmp(wa, bus_wram(), CT_WRAM_SIZE),
                      "%s %s m%dx%d trial %d: WRAM differs at fault '%s'", tag, f->name, f->m, f->x,
                      t, ra.msg);
                faulted_checked++;
            }
            if (getenv("CT_DIFF_VERBOSE") && ra.fatal && rb.fatal && fatal_both <= 3)
                fprintf(stderr, "  fatal in both: %s\n", ra.msg);
            continue;
        }
        int ok_cpu = cpu_equal(&ra.cpu, &rb.cpu);
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
            CHECK(0, "%s %s m%dx%d trial %d: WRAM differs at $%05X (gen %02X interp %02X)", tag,
                  f->name, f->m, f->x, t, 0x7E0000 + a, wa[a], bus_wram()[a]);
        }
        CHECK(!memcmp(sa, bus_sram(), CT_SRAM_SIZE), "%s %s: SRAM differs", tag, f->name);
        CHECK(!memcmp(ra.alu, rb.alu, 4), "%s %s: math registers differ", tag, f->name);
    }
    printf("  %-30s $%06X m%dx%d  %d trials, %ld fatal in both (%ld compared at the fault)\n",
           f->name, f->addr, f->m, f->x, t, fatal_both, faulted_checked);
}

int main(int argc, char **argv)
{
    int trials = argc > 1 ? atoi(argv[1]) : 2000;
    const char *filter = argc > 2 ? argv[2] : NULL;
    bus_init(NULL);
    ct_fatal_hook = on_fatal;
    for (unsigned k = 0; k < ct_func_count; k++)
        if (!filter || strstr(ct_funcs[k].name, filter))
            diff_func(&ct_funcs[k], trials);
    return th_report("diff");
}
