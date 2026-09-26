/* Minimal test harness. */
#ifndef CT_HARNESS_H
#define CT_HARNESS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bus.h"
#include "cpu.h"
#include "ct_funcs.h"
#include "interp.h"
#include "ops.h"

static long th_checks, th_fails;

#define CHECK(cond, ...)                                              \
    do {                                                              \
        th_checks++;                                                  \
        if (!(cond)) {                                                \
            if (th_fails++ < 20) {                                    \
                fprintf(stderr, "%s:%d: FAIL %s: ", __FILE__, __LINE__, #cond); \
                fprintf(stderr, __VA_ARGS__);                         \
                fputc('\n', stderr);                                  \
            }                                                         \
        }                                                             \
    } while (0)

static inline int th_report(const char *name)
{
    printf("%s: %ld checks, %ld failed\n", name, th_checks, th_fails);
    return th_fails ? 1 : 0;
}

/* xorshift64*, fixed seed per test for reproducibility. */
static uint64_t th_rng = 0x9E3779B97F4A7C15ull;
static inline uint32_t rnd32(void)
{
    th_rng ^= th_rng >> 12;
    th_rng ^= th_rng << 25;
    th_rng ^= th_rng >> 27;
    return (uint32_t)((th_rng * 0x2545F4914F6CDD1Dull) >> 32);
}

/* Run through the reference interpreter instead of generated code. */
static int th_interp;

static inline void th_args(int argc, char **argv)
{
    for (int k = 1; k < argc; k++)
        if (!strcmp(argv[k], "interp"))
            th_interp = 1;
}

/* Stack-clobber guard (diff_all). A trial's live stack frames are the
   bytes in (S, top]: everything the routine and its callees have pushed
   and not yet pulled, up to the return address the harness pushed at top.
   Pushes and JSR/JSL write at S and never land there. Any other write into
   that range (a store, MVN, the WRAM data port, DMA) clobbers the saved
   return addresses and registers the routine will pull back; with fuzzed
   state that is a trial artifact, not a result. Counts the clobbering
   writes. The routine must not use stack-relative stores (none of the
   recompiled set does). */
static const CPU *sg_cpu;
static uint16_t sg_top;
static long sg_hits;

static inline void sg_hook(uint32_t off)
{
    if (off < 0x2000 && off > sg_cpu->S && off <= sg_top)
        sg_hits++;
}

static inline void stack_guard_begin(const CPU *cpu, uint16_t top)
{
    sg_cpu = cpu;
    sg_top = top;
    sg_hits = 0;
    ct_wram_write_hook = sg_hook;
}

static inline long stack_guard_end(void)
{
    ct_wram_write_hook = NULL;
    return sg_hits;
}

typedef void (*th_fn)(CPU *);

static inline const ct_func *th_lookup(th_fn fn)
{
    for (unsigned k = 0; k < ct_func_count; k++)
        if (ct_funcs[k].fn == fn)
            return &ct_funcs[k];
    fprintf(stderr, "th_lookup: function not in ct_funcs\n");
    exit(2);
}

/* Emulate JSR from caller_pc (address of the JSR opcode, same bank):
   push caller_pc+2, run fn, then require S restored and PC = caller_pc+3. */
static inline int call_jsr(CPU *cpu, th_fn fn, uint16_t caller_pc)
{
    uint16_t s0 = cpu->S;
    push16(cpu, (uint16_t)(caller_pc + 2));
    if (th_interp) {
        static th_fn last;
        static uint32_t addr;
        if (fn != last) {
            addr = th_lookup(fn)->addr;
            last = fn;
        }
        interp_call(cpu, addr);
    } else {
        fn(cpu);
    }
    int ok = cpu->S == s0 && cpu->PC == (uint16_t)(caller_pc + 3);
    CHECK(cpu->S == s0, "S $%04X after call, expected $%04X", cpu->S, s0);
    CHECK(cpu->PC == (uint16_t)(caller_pc + 3), "return PC $%04X", cpu->PC);
    return ok;
}

#endif
