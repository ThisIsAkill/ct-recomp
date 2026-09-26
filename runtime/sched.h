/* Frame scheduler: runs the system-mode interpreter from reset against the
 * vendored PPU/DMA with an approximate clock (runtime/interp.c cycle
 * counts). NTSC: 262 lines x 1364 master clocks. Per line: render it,
 * run the CPU to HBlank, HDMA, run the CPU to the end of the line. Line 0
 * starts HDMA for the frame; line 225 starts VBlank (RDNMI/HVBJOY flags,
 * NMI if enabled). Events land on instruction boundaries.
 *
 * Owns $4200 NMITIMEN, $4201 WRIO (counter latch only), $4207-$420A
 * H/V timer targets, $4210 RDNMI, $4211 TIMEUP, $4212 HVBJOY, and the PPU
 * counter registers $2137 SLHV, $213C OPHCT, $213D OPVCT, $213F STAT78.
 * H/V IRQs set TIMEUP (the IRQ line) until $4211 is read; the CPU takes
 * the IRQ whenever I=0, and an IRQ with I=1 still ends a WAI. */
#ifndef CT_SCHED_H
#define CT_SCHED_H

#include <stdint.h>

#include "cpu.h"

#define SCHED_CLOCKS_PER_LINE 1364
#define SCHED_LINES           262
#define SCHED_HBLANK_CLOCK    1096   /* dot 274 */
#define SCHED_VBLANK_LINE     225
#define SCHED_WIDTH           256
#define SCHED_HEIGHT          224

/* Hook the timing registers and attach to cpu (already set up by the
   caller, usually with interp_reset). bus_init must have run. */
void sched_init(CPU *cpu);

/* Run one full frame, lines 0-261. */
void sched_run_frame(void);

/* Last rendered frame: SCHED_WIDTH x SCHED_HEIGHT pixels, bytes B G R 0. */
const uint8_t *sched_frame(void);

long sched_frame_count(void);
long sched_nmi_count(void);
int sched_line(void);

#endif
