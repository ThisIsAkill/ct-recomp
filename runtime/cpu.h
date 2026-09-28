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

/* Called before every instruction (generated: ct_trace() in ops.h; interp:
   interp_call()) with the CPU state on entry to it, if set (test use). */
extern void (*ct_trace_hook)(const CPU *cpu, uint32_t addr);

/* Called at the start of every generated instruction when set (the frame
   scheduler's tick: charges the previous instruction, runs due events and
   interrupts, begins this one). NULL outside the scheduler. */
extern void (*ct_tick_hook)(CPU *cpu, uint32_t addr, uint8_t op);

/* Generated code calling code that isn't compiled (#30). Both run the
   CPU as the system executor does (ct_exec_hook: the frame scheduler, with
   native code for compiled callees, interrupts and timing; without it, the
   strict interpreter, interp_call, as diff_all's oracle runs code) until:
   - ct_call_interp: the callee, entered at `target` with its return
     address already pushed (n = 2 for JSR, 3 for JSL), pops it (S back at
     or above where it was). Nonzero if it came back to `back` (PB:PC)
     with S exactly there; zero if it went anywhere else (code that skips
     inline data after the call, or drops its return address): the caller
     then runs its rest with ct_interp_rest;
   - ct_interp_rest: the calling function itself returns, S above s0 (its
     S at entry), after a callee came back in an M/X state its compiled
     continuation doesn't cover. */
typedef struct {
    uint32_t back;   /* ct_call_interp: PB:PC it should return to; ~0u: ct_interp_rest */
    uint16_t s;      /* S to reach or pass (call), or to exceed (rest) */
} ct_exec_until;
extern void (*ct_exec_hook)(CPU *cpu, const ct_exec_until *until);
int ct_exec_done(const CPU *cpu, const ct_exec_until *until);
int ct_call_interp(CPU *cpu, uint32_t target, uint32_t back, unsigned n);
void ct_interp_rest(CPU *cpu, uint16_t s0);

#endif
