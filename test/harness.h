/* Minimal test harness. */
#ifndef CT_HARNESS_H
#define CT_HARNESS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bus.h"
#include "cpu.h"
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

/* Emulate JSR from caller_pc (address of the JSR opcode, same bank):
   push caller_pc+2, run fn, then require S restored and PC = caller_pc+3. */
typedef void (*th_fn)(CPU *);
static inline int call_jsr(CPU *cpu, th_fn fn, uint16_t caller_pc)
{
    uint16_t s0 = cpu->S;
    push16(cpu, (uint16_t)(caller_pc + 2));
    fn(cpu);
    int ok = cpu->S == s0 && cpu->PC == (uint16_t)(caller_pc + 3);
    CHECK(cpu->S == s0, "S $%04X after call, expected $%04X", cpu->S, s0);
    CHECK(cpu->PC == (uint16_t)(caller_pc + 3), "return PC $%04X", cpu->PC);
    return ok;
}

#endif
