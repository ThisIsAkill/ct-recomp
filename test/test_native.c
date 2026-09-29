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
#include "overlay.h"

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

/* Calls into code that isn't compiled (#30), shaped like emitter output.
   $7E2300: JSR $2400 / LDA #$07 / STA $13 / RTS, where $7E2400 is the
   loop above, not in the table: interpreted, across NMIs.
   $7E2600: JSR $2500 / LDA #$0007 / STA $13 / SEP #$20 / RTS, where
   $7E2500 is REP #$20 / RTS: it comes back with m0, which the compiled
   continuation (for m1: the same code as $7E2300's) doesn't cover, so the
   rest is interpreted. */
static int ran_caller;

static void native_caller(CPU *cpu, uint32_t base, uint16_t callee)
{
    ran_caller++;
    cpu_enter(cpu, base, 1, 0);
    const uint16_t s0 = cpu->S;
    ct_insn(cpu, base, 0x20, (uint8_t)(callee >> 8));
    push16(cpu, (uint16_t)(base + 2));
    if (!ct_call_interp(cpu, 0x7E0000 | callee, base + 3, 2) || cpu->m != 1 || cpu->x != 0) {
        ct_interp_rest(cpu, s0);
        return;
    }
    ct_insn(cpu, base + 3, 0xA9, 0x07);
    lda8(cpu, 0x07);
    ct_insn(cpu, base + 5, 0x85, 0x13);
    write8(ea_dp(cpu, 0x13), a8(cpu));
    ct_insn(cpu, base + 7, 0xE2, 0x20);
    op_sep(cpu, 0x20);
    ct_insn(cpu, base + 9, 0x60, 0x60);
    op_rts(cpu);
}

static void f_2300(CPU *cpu) { native_caller(cpu, 0x7E2300, 0x2400); }
static void f_2600(CPU *cpu) { native_caller(cpu, 0x7E2600, 0x2500); }

/* $7E2900: loop: INC $16 / BRA loop, natively, never returning (a main
   loop): the CPU runs as a coroutine, so each sched_run_frame still returns
   after one frame. */
static void f_2900(CPU *cpu)
{
    cpu_enter(cpu, 0x7E2900, 1, 0);
loop:
    ct_insn(cpu, 0x7E2900, 0xE6, 0x16);
    write8(ea_dp(cpu, 0x16), (uint8_t)(read8(ea_dp(cpu, 0x16)) + 1));
    ct_insn(cpu, 0x7E2902, 0x80, 0xFC);
    ct_cyc_taken = 1;
    goto loop;
}

static void f_2100(CPU *cpu) { native_loop(cpu, 0x7E2100); }
static void f_2200(CPU *cpu) { native_loop(cpu, 0x7E2200); }

/* The tables a game's generated ct_funcs.c would provide. */
const ct_func ct_funcs[] = {
    {"TestLoop", 0x7E2100, 1, 0, 14, -1, -1, 0, f_2100},
    {"TestLoopDB55", 0x7E2200, 1, 0, 14, 0x55, -1, 0, f_2200},   /* assumes DB=$55 */
    {"TestCaller", 0x7E2300, 1, 0, 10, -1, -1, 0, f_2300},
    {"TestCallerMX", 0x7E2600, 1, 0, 10, -1, -1, 0, f_2600},
    {"TestForever", 0x7E2900, 1, 0, 4, -1, -1, 0, f_2900},
};
const unsigned ct_func_count = 5;
const ct_extern ct_externs[] = {{0, 0, 0}};
const unsigned ct_extern_count = 0;
const ct_jumptable ct_jumptables[] = {{0, 0}};
const unsigned ct_jumptable_count = 0;
/* An overlay (#92): code in WRAM at $7E2800, LDA #$42 / STA $15 / RTS, run
   natively only while those 5 bytes hash to what it was compiled from. */
static const uint8_t ovl_bytes[] = {0xA9, 0x42, 0x85, 0x15, 0x60};
static int ran_overlay;

static void f_ovl_2800(CPU *cpu)
{
    ran_overlay++;
    cpu_enter(cpu, 0x7E2800, 1, 0);
    ct_insn(cpu, 0x7E2800, 0xA9, 0x42);
    lda8(cpu, 0x42);
    ct_insn(cpu, 0x7E2802, 0x85, 0x15);
    write8(ea_dp(cpu, 0x15), a8(cpu));
    ct_insn(cpu, 0x7E2804, 0x60, 0x60);
    op_rts(cpu);
}

const ct_overlay_func ct_overlay_funcs[] = {
    {"TestOverlay", 0x7E2800, 1, 0, 0x7E2800, 0x7E2804, 0xC029F4EF3B675BA0ull, f_ovl_2800,
     ovl_bytes, 0x7E2800, sizeof ovl_bytes},
};
const unsigned ct_overlay_func_count = 1;

typedef struct {
    uint64_t clock, native_insns, interp_insns;
    char profile[1024];
    long nmis;
    uint8_t w10, w11, w12, w13, w14, w15;
    CPU cpu;
} Outcome;

/* Boot a WRAM program that enables NMI, JSRs to `target`, then idles in
   WAI; NMI handler (ROM stub -> $000500) counts in $11. */
static const uint8_t nmi_count[] = {0xE6, 0x11, 0xAD, 0x10, 0x42, 0x40};   /* INC $11 / LDA $4210 / RTI */
/* LDA $4210 / LDA 2,S / INC A / STA 2,S / RTI: returns one byte past the
   interrupted instruction. */
static const uint8_t nmi_skew[] = {0xAD, 0x10, 0x42, 0xA3, 0x02, 0x1A, 0x83, 0x02, 0x40};

static int ovl_patch;

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
    static const uint8_t caller[] = {0x20, 0x00, 0x00, 0xA9, 0x07, 0x85, 0x13, 0xE2, 0x20, 0x60};
    memcpy(w + 0x2300, caller, sizeof caller);
    w[0x2302] = 0x24;   /* JSR $2400 */
    memcpy(w + 0x2400, loop_bytes, sizeof loop_bytes);
    /* JSR $2500 / LDA #$0007 / STA $13 / SEP #$20 / RTS, as it runs in m0 */
    static const uint8_t caller_m0[] = {0x20, 0x00, 0x25, 0xA9, 0x07, 0x00, 0x85, 0x13, 0xE2,
                                        0x20, 0x60};
    memcpy(w + 0x2600, caller_m0, sizeof caller_m0);
    memcpy(w + 0x2800, ovl_bytes, sizeof ovl_bytes);
    if (ovl_patch)
        w[0x2801] = 0x43;   /* LDA #$43: the bytes no longer match the compiled code */
    static const uint8_t to_m0[] = {0xC2, 0x20, 0x60};   /* REP #$20 / RTS */
    memcpy(w + 0x2500, to_m0, sizeof to_m0);

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
    o.w13 = w[0x13];
    o.w14 = w[0x14];
    o.w15 = w[0x15];
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
           a->w12 == b->w12 && a->w13 == b->w13 && a->w14 == b->w14 && a->cpu.A == b->cpu.A && a->cpu.X == b->cpu.X &&
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

    /* Native code calling into interpreted code (#30). */
    Outcome ref3 = run(0x2300, 0);
    ran_caller = ran_native = 0;
    Outcome call = run(0x2300, 1);
    CHECK(ran_caller == 1 && ran_native == 0, "caller native, callee interpreted: %d %d",
          ran_caller, ran_native);
    CHECK(ref3.w10 == 0x42 && ref3.w13 == 0x07 && ref3.nmis >= 5, "callee and caller ran");
    CHECK(same(&ref3, &call), "native caller == interpreter: clock %llu vs %llu, nmis %ld vs %ld",
          (unsigned long long)call.clock, (unsigned long long)ref3.clock, call.nmis, ref3.nmis);
    CHECK(call.w11 == ref3.w11, "NMIs inside the interpreted callee: %u vs %u", call.w11,
          ref3.w11);
    Outcome ref4 = run(0x2600, 0);
    ran_caller = 0;
    Outcome mx = run(0x2600, 1);
    CHECK(ran_caller == 1, "M/X caller dispatched natively");
    CHECK(ref4.w13 == 0x07 && ref4.w14 == 0x00 && ref4.w12 == 1, "rest ran 16-bit: $13 %02X $14 %02X",
          ref4.w13, ref4.w14);
    CHECK(same(&ref4, &mx), "callee changed M: rest interpreted == interpreter: clock %llu vs %llu",
          (unsigned long long)mx.clock, (unsigned long long)ref4.clock);

    /* Overlays (#92): code in WRAM runs natively while its bytes match. */
    ran_overlay = 0;
    Outcome ov = run(0x2800, 1);
    CHECK(ran_overlay == 1 && ov.w15 == 0x42, "matching bytes: overlay native (%d), $15=%02X",
          ran_overlay, ov.w15);
    ran_overlay = 0;
    ovl_patch = 1;
    Outcome pat = run(0x2800, 1);
    ovl_patch = 0;
    CHECK(ran_overlay == 0 && pat.w15 == 0x43, "patched byte: interpreted (%d), $15=%02X",
          ran_overlay, pat.w15);
    Outcome ovi = run(0x2800, 0);
    CHECK(same(&ov, &ovi) && ov.w15 == ovi.w15, "overlay native == interpreter");

    /* Native code that never returns: one frame per sched_run_frame all the
       same, and it keeps running across them. */
    {
        static const uint8_t forever[] = {0xE6, 0x16, 0x80, 0xFC};
        bus_reset();
        memcpy(bus_wram() + 0x2900, forever, sizeof forever);
        static CPU fc;
        interp_reset(&fc);
        fc.e = 0;
        fc.PB = 0x7E;
        fc.PC = 0x2900;
        fc.m = 1;
        fc.x = 0;
        fc.S = 0x01FF;
        sched_set_native(1);
        sched_init(&fc);
        long r1 = sched_run_frame(), r2 = sched_run_frame();
        uint8_t c2 = bus_wram()[0x16];
        long r3 = sched_run_frame();
        CHECK(r1 == 1 && r2 == 1 && r3 == 1 && sched_frame_count() == 3,
              "a frame per call: %ld %ld %ld, count %ld", r1, r2, r3, sched_frame_count());
        CHECK(bus_wram()[0x16] != c2, "still running in frame 3");
    }

    /* The cached check follows CPU writes: a patch, then the original back. */
    bus_reset();
    memcpy(bus_wram() + 0x2800, ovl_bytes, sizeof ovl_bytes);
    overlay_init();
    CPU oc;
    cpu_init(&oc);
    oc.PB = 0x7E;
    oc.PC = 0x2800;
    oc.m = 1;
    oc.x = 0;
    int a0 = overlay_lookup(&oc) != NULL;
    write8(0x7E2801, 0x43);
    int a1 = overlay_lookup(&oc) != NULL;
    write8(0x7E2801, 0x42);
    int a2 = overlay_lookup(&oc) != NULL;
    oc.m = 0;
    int a3 = overlay_lookup(&oc) != NULL;
    CHECK(a0 && !a1 && a2 && !a3, "lookup: match %d, patched %d, restored %d, other M %d", a0, a1,
          a2, a3);

    return th_report("native");
}
