/* Reference 65816 interpreter (native mode, binary arithmetic).
   Test oracle for generated code: shares only the bus with the runtime. */
#ifndef CT_INTERP_H
#define CT_INTERP_H

#include <stdint.h>

#include "cpu.h"

/* Per-call step budget, shared with generated code's ct_budget (see
   test_diff.c) so a legitimately slow routine doesn't fail differentially
   just because one side's safety net is tighter than the other's. */
#define CT_INTERP_BUDGET 3000000

/* Run from entry (24-bit) with the return address already pushed; stop at
   the RTS/RTL that pulls S above its entry value. Fatal on anything
   unsupported. */
void interp_call(CPU *c, uint32_t entry);

#endif
