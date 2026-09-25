/* 65816 register file. */
#ifndef CT_CPU_H
#define CT_CPU_H

#include <stdint.h>

typedef struct CPU {
    uint16_t A;         /* B:A; B preserved when m=1 */
    uint16_t X, Y;      /* high byte 0 when x=1 */
    uint16_t S;
    uint16_t DP;
    uint8_t  DB, PB;
    uint16_t PC;        /* set by RTS/RTL */
    uint8_t  n, v, m, x, d, i, z, c, e;
} CPU;

/* Native mode, m=1 x=0, S=$01FF, everything else zero. */
void cpu_init(CPU *cpu);

#if defined(__GNUC__)
#define CT_NORETURN __attribute__((noreturn))
#define CT_PRINTF(a, b) __attribute__((format(printf, a, b)))
#else
#define CT_NORETURN _Noreturn
#define CT_PRINTF(a, b)
#endif

/* Print "ct: <msg>" to stderr and exit(3). If ct_fatal_hook is set it is
   called with the message instead and must not return (test use). */
CT_NORETURN void ct_fatal(const char *fmt, ...) CT_PRINTF(1, 2);
extern void (*ct_fatal_hook)(const char *msg);

/* Remaining backward-branch budget for generated code (0 = unlimited). */
extern uint64_t ct_budget;

/* CT_TEST_BUILD only (see ops.h ct_loop): hard backward-branch cap,
   independent of ct_budget. 0 = unlimited. */
extern long ct_test_cap;

#endif
