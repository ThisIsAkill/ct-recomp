/* Overlays (#92): recompiled code the game puts in WRAM (func_table.h,
 * ct_overlay_funcs). An entry runs native only while the WRAM bytes it was
 * compiled from still hash to what they were; otherwise it is interpreted.
 * Checks are cached per entry against the write counters of the 256-byte
 * WRAM pages it covers (bus.h ct_wram_gen): counters only grow, so an
 * unchanged sum means no write landed there since the last check. */
#ifndef CT_OVERLAY_H
#define CT_OVERLAY_H

#include <stdint.h>

#include "bus.h"
#include "cpu.h"
#include "func_table.h"

/* (Re)build the lookup from ct_overlay_funcs; none with CT_OVERLAYS=0 in
   the environment (for bisecting a divergence). */
void overlay_init(void);
/* The overlay function to run natively at the CPU's PB:PC in its M/X, or
   NULL (none compiled there for that state, or its bytes changed). */
const ct_overlay_func *overlay_lookup(const CPU *c);
/* Its index in the lookup, for overlay_enter (set by overlay_lookup). */
extern unsigned overlay_last_idx;
/* Whether any overlay entry is compiled at addr for m/x, and whether one
   is but its bytes don't match (for the profile's report). */
int overlay_state(uint32_t addr, int m, int x, int *stale);

/* Overlay functions running natively (outermost first), for the code-
 * overwrite check: their WRAM pages are watched (bus.h ct_wram_watch), and a
 * write there raises ct_wram_hit. At the next native instruction boundary
 * the scheduler asks overlay_stale_active: if an active function's own
 * bytes changed, strict mode (ct_overlay_strict: debug builds, lockstep and
 * reference runs) fails loudly; otherwise the interpreter carries on from
 * that instruction until the outermost such function returns, and a
 * longjmp to its dispatch point (jb) drops the stale native frames. */
typedef struct {
    const ct_overlay_func *f;
    unsigned idx;       /* in the lookup */
    uint16_t s0;        /* S at entry: it has returned once S is above */
    int dormant;        /* handed its rest to the interpreter: no more of its code runs */
    uint32_t lo, len;   /* the watched range before it entered (restored on leave) */
    /* __builtin_setjmp buffer (GCC/Clang: saves only frame, stack and
       resume address, cheap enough for every dispatch); the matching
       __builtin_longjmp always comes from a deeper function */
    void *jb[5];
} overlay_act;

extern int ct_overlay_strict;

/* Enter and leave run for every native overlay call: inline, and the
   watch is one range covering every running overlay function (a write
   anywhere in it is checked; a check that finds nothing changed is cheap). */
#define OVERLAY_MAX_ACT 64
extern overlay_act ct_ovl_act[OVERLAY_MAX_ACT];
extern int ct_ovl_n;

static inline overlay_act *overlay_enter(const ct_overlay_func *f, unsigned idx, uint16_t s0)
{
    if (ct_ovl_n == OVERLAY_MAX_ACT)
        ct_fatal("overlay: more than %d nested overlay functions", OVERLAY_MAX_ACT);
    overlay_act *a = &ct_ovl_act[ct_ovl_n++];
    a->f = f;
    a->idx = idx;
    a->s0 = s0;
    a->dormant = 0;
    a->lo = ct_wram_watch_lo;
    a->len = ct_wram_watch_len;
    uint32_t lo = f->lo & 0x1FFFF, hi = (f->hi & 0x1FFFF) + 1;
    if (ct_wram_watch_len) {
        uint32_t olo = ct_wram_watch_lo, ohi = olo + ct_wram_watch_len;
        if (olo < lo)
            lo = olo;
        if (ohi > hi)
            hi = ohi;
    }
    ct_wram_watch_lo = lo;
    ct_wram_watch_len = hi - lo;
    return a;
}

/* Pop a and everything entered after it. */
static inline void overlay_leave(overlay_act *a)
{
    ct_wram_watch_lo = a->lo;
    ct_wram_watch_len = a->len;
    ct_ovl_n = (int)(a - ct_ovl_act);
}

/* The outermost active function whose bytes no longer match, or NULL. */
overlay_act *overlay_stale_active(void);
/* No function active (reset). */
void overlay_reset_active(void);
/* ct_interp_rest for overlay code (the emitter uses it there): the calling
   overlay function, the innermost active one, runs none of its own code
   from here on, so its bytes may change freely (the program it belongs to
   may be replaced while the interpreter goes on, as at the end of a title
   sequence). */
void ct_overlay_rest(CPU *cpu, uint16_t s0);

#endif
