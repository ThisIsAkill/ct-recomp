/* Approximate 65816 cycle counts, shared by the interpreter and generated
 * code so both charge exactly the same master clocks per instruction.
 *
 * cyc_begin records an instruction's pre-state (M, X, DP low byte, E, and
 * the speed of the region it was fetched from) and clears the facts only
 * known while it executes; cyc_finish charges it. While it executes, the
 * addressing helpers set ct_cyc_cross (indexed address crossed a page),
 * and branches set ct_cyc_taken. */
#ifndef CT_CYCLES_H
#define CT_CYCLES_H

#include <stdint.h>

#include "cpu.h"

extern int ct_cyc_cross;
extern int ct_cyc_taken;

void cyc_begin(const CPU *c, uint32_t at, uint8_t op);
/* The same for compiled code, whose operand bytes are never fetched:
   they're charged from the instruction's length instead. */
void cyc_begin_compiled(const CPU *c, uint32_t at, uint8_t op);
/* Master clocks for the instruction begun last; 0 if none is pending. */
unsigned cyc_finish(void);

/* Master clocks into the current instruction at the access being made:
   its opcode fetch, compiled code's operand fetches, and the bus accesses
   so far (internal cycles in between aren't known; see cycles.c). */
unsigned cyc_elapsed(void);

/* The current instruction cycle by cycle, in order, as far as its bus
   accesses have happened: clocks and kind of each cycle, at most `max`;
   stops after the cycle making bus access number `upto` (0-based; ~0u
   for all). idx gets each cycle's bus access number, or -1. Returns the
   number of cycles. */
enum { CY_READ, CY_WRITE, CY_IDLE };
unsigned cyc_cycles(uint8_t *clk, uint8_t *kind, int8_t *idx, unsigned max, unsigned upto);
/* Begin an interrupt entry (its caller charges it): cyc_cycles then
   describes its cycles. */
void cyc_begin_interrupt(const CPU *c);
/* An instruction or interrupt entry is running (begun, not yet charged). */
int cyc_in_progress(void);
/* The interrupt entry begun last has been charged. */
void cyc_done(void);
/* With CT_CYC_CHECK set, cyc_finish checks each instruction's cycles
   against its count and reports mismatches on stderr. */
void cyc_check(void);

/* Opcode fetch speed at PB:PC: 6 for FastROM (banks $80-$FF ROM with
   MEMSEL bit 0), otherwise 8. */
unsigned cyc_master_per_cycle(uint8_t pb, uint16_t pc);

#endif
