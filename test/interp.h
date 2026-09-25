/* Reference 65816 interpreter (native mode, binary arithmetic).
   Test oracle for generated code: shares only the bus with the runtime. */
#ifndef CT_INTERP_H
#define CT_INTERP_H

#include <stdint.h>

#include "cpu.h"

/* Per-call step budget, shared with generated code's ct_budget (see
   test_diff.c) so a legitimately slow routine doesn't fail differentially
   just because one side's safety net is tighter than the other's. Charged
   in the same unit as ct_loop (runtime/ops.h): once per backward branch/
   jump taken, not per instruction -- see loop_tick() in interp.c. */
#define CT_INTERP_BUDGET 3000000

/* Hard per-call cap on dispatched instructions, charged per fetch unlike
   CT_INTERP_BUDGET's per-backward-branch unit: catches a divergence that
   wanders without ever taking one. Set well above CT_INTERP_BUDGET times
   any realistic loop-body length, so it never preempts that budget's own
   (correctly matched) exhaustion on a legitimately long loop. */
#define CT_INTERP_DISPATCH_CAP 200000000

/* Matching hard backstop for generated code's ct_test_cap (CT_TEST_BUILD,
   see runtime/ops.h): same role, other side. */
#define CT_GEN_BACKWARD_CAP 5000000

/* Run from entry (24-bit) with the return address already pushed; stop at
   the RTS/RTL that pulls S above its entry value. Fatal on anything
   unsupported. */
void interp_call(CPU *c, uint32_t entry);

#endif
