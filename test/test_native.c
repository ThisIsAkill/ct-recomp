/* Native dispatch in the frame scheduler (runtime/sched.c), with its own
 * function table in place of a game's: a hand-written function shaped like
 * emitter output (ct_insn per instruction, ops.h helpers) runs natively
 * when the CPU reaches its entry, charges the same cycles the interpreter
 * would, takes NMIs mid-function, and is interpreted instead when the CPU
 * doesn't match its declared DB. Each case is checked against the same
 * bytes run by the interpreter alone. An interrupt that returns anywhere
 * but the interrupted instruction and stack level is fatal. */
#include <setjmp.h>

#include "harness.h"
#include "ops.h"
#include "sched.h"

static int ran_native;
static long nmis_in_native;

/* $7E2100 / $7E2200: LDX #$0000 / loop: INX / CPX #$4000 / BNE loop /
   LDA #$42 / STA $10 / RTS. ~1M master clocks: spans several VBlanks. */
static void native_loop(CPU *cpu, uint32_t base)
{
    long nmi0 = sched_nmi_count();
    ran_native++;
    cpu_enter(cpu, base, 1, 0);
    ct_insn(cpu, base + 0x0, 0xA2, 0x00);
    ldx16(cpu, 0x0000);
loop:
    ct_insn(cpu, base + 0x3, 0xE8, 0xE8);
    cpu->X = (uint16_t)(cpu->X + 1);
    set_nz16(cpu, cpu->X);
    ct_insn(cpu, base + 0x4, 0xE0, 0x40);
    cmp16(cpu, cpu->X, 0x4000);
    ct_insn(cpu, base + 0x7, 0xD0, 0xFA);
    if (!cpu->z) {
        ct_cyc_taken = 1;
        goto loop;
    }
    ct_insn(cpu, base + 0x9, 0xA9, 0x42);
    lda8(cpu, 0x42);
    ct_insn(cpu, base + 0xB, 0x85, 0x10);
    write8(ea_dp(cpu, 0x10), a8(cpu));
    ct_insn(cpu, base + 0xD, 0x60, 0x60);
    nmis_in_native += sched_nmi_count() - nmi0;
    op_rts(cpu);
}

static void f_2100(CPU *cpu) { native_loop(cpu, 0x7E2100); }
static void f_2200(CPU *cpu) { native_loop(cpu, 0x7E2200); }

/* The tables a game's generated ct_funcs.c would provide. */
const ct_func ct_funcs[] = {
    {"TestLoop", 0x7E2100, 1, 0, 14, -1, -1, 0, f_2100},
    {"TestLoopDB55", 0x7E2200, 1, 0, 14, 0x55, -1, 0, f_2200},   /* assumes DB=$55 */
};
const unsigned ct_func_count = 2;
const ct_extern ct_externs[] = {{0, 0, 0}};
const unsigned ct_extern_count = 0;
const ct_jumptable ct_jumptables[] = {{0, 0}};
const unsigned ct_jumptable_count = 0;

typedef struct {
    uint64_t clock, native_insns, interp_insns;
    char profile[1024];
    long nmis;
    uint8_t w10, w11, w12;
    CPU cpu;
} Outcome;

/* Boot a WRAM program that enables NMI, JSRs to `target`, then idles in
   WAI; NMI handler (ROM stub -> $000500) counts in $11. */
static const uint8_t nmi_count[] = {0xE6, 0x11, 0xAD, 0x10, 0x42, 0x40};   /* INC $11 / LDA $4210 / RTI */
/* LDA $4210 / LDA 2,S / INC A / STA 2,S / RTI: returns one byte past the
   interrupted instruction. */
static const uint8_t nmi_skew[] = {0xAD, 0x10, 0x42, 0xA3, 0x02, 0x1A, 0x83, 0x02, 0x40};

static Outcome run_with(uint16_t target, int native, const uint8_t *nmi, size_t nmi_len)
{
    static const uint8_t loop_bytes[] = {0xA2, 0x00, 0x00, 0xE8, 0xE0, 0x00, 0x40, 0xD0, 0xFA,
                                         0xA9, 0x42, 0x85, 0x10, 0x60};
    uint8_t prog[] = {
        0xA9, 0x80, 0x8D, 0x00, 0x42,               /* LDA #$80 / STA $4200 */
        0x20, (uint8_t)target, (uint8_t)(target >> 8), /* JSR target */
        0xA9, 0x01, 0x85, 0x12,                     /* LDA #$01 / STA $12 */
        0xCB, 0x80, 0xFD,                           /* idle: WAI / BRA idle */
    };
    bus_reset();
    uint8_t *w = bus_wram();
    memcpy(w + 0x2000, prog, sizeof prog);
    memcpy(w + 0x2100, loop_bytes, sizeof loop_bytes);
    memcpy(w + 0x2200, loop_bytes, sizeof loop_bytes);
    memcpy(w + 0x0500, nmi, nmi_len);

    static CPU c;
    interp_reset(&c);   /* power-on: also clears a pending WAI */
    c.e = 0;
    c.PB = 0x7E;
    c.PC = 0x2000;
    c.DB = 0x00;
    c.m = 1;
    c.x = 0;
    c.S = 0x01FF;
    sched_set_native(native);
    sched_init(&c);
    while (sched_frame_count() < 6)
        sched_run_frame();
    CHECK(sched_frame_count() == 6, "stopped at frame %ld", sched_frame_count());
    Outcome o = {0};
    o.clock = sched_clock();
    o.native_insns = sched_native_insns();
    o.interp_insns = sched_interp_insns();
    FILE *f = tmpfile();
    if (f) {
        sched_profile_report(f, 3);
        rewind(f);
        size_t n = fread(o.profile, 1, sizeof o.profile - 1, f);
        o.profile[n] = 0;
        fclose(f);
    }
    o.nmis = sched_nmi_count();
    o.w10 = w[0x10];
    o.w11 = w[0x11];
    o.w12 = w[0x12];
    o.cpu = c;
    return o;
}

static Outcome run(uint16_t target, int native)
{
    return run_with(target, native, nmi_count, sizeof nmi_count);
}

static jmp_buf fatal_jmp;
static char fatal_msg[256];

static void on_fatal(const char *msg)
{
    snprintf(fatal_msg, sizeof fatal_msg, "%s", msg);
    longjmp(fatal_jmp, 1);
}

static int same(const Outcome *a, const Outcome *b)
{
    return a->clock == b->clock && a->nmis == b->nmis && a->w10 == b->w10 && a->w11 == b->w11 &&
           a->w12 == b->w12 && a->cpu.A == b->cpu.A && a->cpu.X == b->cpu.X &&
           a->cpu.S == b->cpu.S && a->cpu.PC == b->cpu.PC && a->cpu.PB == b->cpu.PB;
}

int main(void)
{
    th_bus_init();

    ran_native = 0;
    Outcome ref = run(0x2100, 0);
    CHECK(ran_native == 0, "interpreter only: native never ran");
    CHECK(ref.w10 == 0x42 && ref.w12 == 1 && ref.nmis >= 5, "program ran: $10=%02X $12=%02X nmis %ld",
          ref.w10, ref.w12, ref.nmis);

    ran_native = 0;
    nmis_in_native = 0;
    Outcome nat = run(0x2100, 1);
    CHECK(ran_native == 1, "TestLoop dispatched natively once: %d", ran_native);
    CHECK(nmis_in_native >= 2, "NMIs taken mid-function: %ld", nmis_in_native);
    CHECK(same(&ref, &nat), "native == interpreter: clock %llu vs %llu, nmis %ld vs %ld",
          (unsigned long long)nat.clock, (unsigned long long)ref.clock, nat.nmis, ref.nmis);
    CHECK(nat.w11 == ref.w11, "NMI handler count %u vs %u", nat.w11, ref.w11);

    /* Coverage: the loop's 3 x $4000 instructions run natively in one run;
       in the interpreter-only run they're charged to the function they run
       in, $7E2100. Both runs execute the same instructions in total. */
    CHECK(ref.native_insns == 0 && ref.interp_insns > 3 * 0x4000,
          "interpreter only: %llu native, %llu interpreted", (unsigned long long)ref.native_insns,
          (unsigned long long)ref.interp_insns);
    CHECK(nat.native_insns >= 3 * 0x4000 && nat.native_insns + nat.interp_insns ==
              ref.interp_insns, "native run: %llu native + %llu interpreted vs %llu",
          (unsigned long long)nat.native_insns, (unsigned long long)nat.interp_insns,
          (unsigned long long)ref.interp_insns);
    CHECK(strstr(ref.profile, "$7E2100 m1x0e0") != NULL,
          "interpreted loop charged to its entry:\n%s", ref.profile);

    /* DB assumption: TestLoopDB55 says DB=$55, the CPU has DB=$00. */
    ran_native = 0;
    Outcome ref2 = run(0x2200, 0);
    Outcome db = run(0x2200, 1);
    CHECK(ran_native == 0, "DB mismatch: interpreted, not dispatched: %d", ran_native);
    CHECK(same(&ref2, &db), "DB mismatch run == interpreter");

    /* An NMI handler that moves the return address: fatal, naming both
       contexts, rather than native code carrying on. */
    fatal_msg[0] = 0;
    ct_fatal_hook = on_fatal;
    if (!setjmp(fatal_jmp))
        run_with(0x2100, 1, nmi_skew, sizeof nmi_skew);
    ct_fatal_hook = NULL;
    CHECK(strstr(fatal_msg, "NMI did not return to the interrupted native code") &&
              strstr(fatal_msg, "interrupted at $7E21"),
          "changed return context is fatal: \"%s\"", fatal_msg);

    return th_report("native");
}
