/* Approximate 65816 cycle counts, shared by the interpreter and generated
 * code so both charge exactly the same master clocks per instruction.
 *
 * cyc_begin records an instruction's pre-state (M, X, DP low byte, E, and
 * the speed of the region it was fetched from) and clears the facts only
 * known while it executes; cyc_finish charges it. While it executes, the
 * addressing helpers set ct_cyc_cross (indexed address crossed a page),
 * branches set ct_cyc_taken, and MVN/MVP count ct_cyc_moved bytes. */
#ifndef CT_CYCLES_H
#define CT_CYCLES_H

#include <stdint.h>

#include "cpu.h"

extern int ct_cyc_cross;
extern int ct_cyc_taken;
extern long ct_cyc_moved;

void cyc_begin(const CPU *c, uint32_t at, uint8_t op);
/* The same for compiled code, whose operand bytes are never fetched:
   they're charged from the instruction's length instead. */
void cyc_begin_compiled(const CPU *c, uint32_t at, uint8_t op);
/* Master clocks for the instruction begun last; 0 if none is pending. */
unsigned cyc_finish(void);

/* Add master clocks the CPU spends paused during the current instruction
   (general DMA started by its $420B write), charged by cyc_finish. */
void cyc_stall(unsigned clocks);

/* Master clocks into the current instruction at the access being made:
   its opcode fetch, compiled code's operand fetches, and the bus accesses
   so far (internal cycles in between aren't known; see cycles.c). */
unsigned cyc_elapsed(void);

/* Opcode fetch speed at PB:PC: 6 for FastROM (banks $80-$FF ROM with
   MEMSEL bit 0), otherwise 8. */
unsigned cyc_master_per_cycle(uint8_t pb, uint16_t pc);

#endif
