/* Overlays (#92): recompiled code the game puts in WRAM (func_table.h,
 * ct_overlay_funcs). An entry runs native only while the WRAM bytes it was
 * compiled from still hash to what they were; otherwise it is interpreted.
 * Checks are cached per entry against the write counters of the 256-byte
 * WRAM pages it covers (bus.h ct_wram_gen): counters only grow, so an
 * unchanged sum means no write landed there since the last check. */
#ifndef CT_OVERLAY_H
#define CT_OVERLAY_H

#include <stdint.h>

#include "cpu.h"
#include "func_table.h"

/* (Re)build the lookup from ct_overlay_funcs; none with CT_OVERLAYS=0 in
   the environment (for bisecting a divergence). */
void overlay_init(void);
/* The overlay function to run natively at the CPU's PB:PC in its M/X, or
   NULL (none compiled there for that state, or its bytes changed). */
const ct_overlay_func *overlay_lookup(const CPU *c);
/* Whether any overlay entry is compiled at addr for m/x, and whether one
   is but its bytes don't match (for the profile's report). */
int overlay_state(uint32_t addr, int m, int x, int *stale);

#endif
