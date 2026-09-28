/* Frame scheduler: runs the system-mode interpreter from reset against the
 * vendored PPU/DMA with an approximate clock (runtime/interp.c cycle
 * counts). NTSC: 262 lines x 1364 master clocks. Per line: render it,
 * run the CPU to HBlank, HDMA, run the CPU to the end of the line. Line 0
 * starts HDMA for the frame; line 225 starts VBlank (RDNMI/HVBJOY flags,
 * NMI if enabled). Events land on instruction boundaries.
 *
 * Owns $4200 NMITIMEN, $4201 WRIO (counter latch only), $4207-$420A
 * H/V timer targets, $4210 RDNMI, $4211 TIMEUP, $4212 HVBJOY, and the PPU
 * counter registers $2137 SLHV, $213C OPHCT, $213D OPVCT, $213F STAT78,
 * and the auto-joypad results $4218-$421F.
 * H/V IRQs set TIMEUP (the IRQ line) until $4211 is read; the CPU takes
 * the IRQ whenever I=0, and an IRQ with I=1 still ends a WAI. */
#ifndef CT_SCHED_H
#define CT_SCHED_H

#include <stdint.h>
#include <stdio.h>

#include "cpu.h"

#define SCHED_CLOCKS_PER_LINE 1364
#define SCHED_LINES           262
#define SCHED_HBLANK_CLOCK    1096   /* dot 274: $4212 H-blank flag */
#define SCHED_HDMA_CLOCK      1104   /* dot 276: HDMA on lines 0-224 */
#define SCHED_VBLANK_LINE     225
#define SCHED_WIDTH           256
#define SCHED_HEIGHT          224

/* Hook the timing registers and attach to cpu (already set up by the
   caller, usually with interp_reset). bus_init must have run. */
void sched_init(CPU *cpu);

/* Native dispatch of recompiled functions (default on); off runs
   everything in the interpreter. */
void sched_set_native(int on);

/* Native coverage since sched_init: instructions run natively and in the
   interpreter, and a report of the functions the interpreted ones ran in
   (top `top` by count, with why each didn't run natively). The report
   consumes the counts it prints. */
uint64_t sched_native_insns(void);
uint64_t sched_interp_insns(void);
void sched_profile_report(FILE *out, int top);

/* Run until at least one frame has completed; returns how many did. It
   is one unless a native function was still running at the frame edge:
   control returns at the next top-level instruction boundary, so a long
   native call can complete several frames. Frame edges themselves happen
   at the exact clock either way (sched_frame() is copied then). */
long sched_run_frame(void);

/* DSP output rate: the SPC700 at SCHED_SPC_HZ makes one stereo sample
   per 32 cycles. 32040 Hz, as bsnes has it from real consoles (fullsnes
   gives the nominal 24.576 MHz crystal, 32000 Hz; decided for bsnes, #31). */
#define SCHED_SPC_HZ   (32040u * 32)
#define SCHED_MASTER_HZ 21477272u   /* NTSC master clock */
#define SCHED_AUDIO_HZ (SCHED_SPC_HZ / 32)

/* Move up to `max` stereo samples (L R interleaved, SCHED_AUDIO_HZ) of DSP
   output not yet taken into `stereo`, oldest first; returns how many.
   About 532.5 per frame. Up to 8192 are kept; older ones are dropped. */
int sched_audio_take(int16_t *stereo, int max);

/* Last rendered frame: SCHED_WIDTH x SCHED_HEIGHT pixels, bytes B G R 0. */
const uint8_t *sched_frame(void);

/* Buttons held on controller port 0-3, read by the next auto-joypad read
   (VBlank start, NMITIMEN bit 0). Bit 15 B, 14 Y, 13 Select, 12 Start,
   11 Up, 10 Down, 9 Left, 8 Right, 7 A, 6 X, 5 L, 4 R. Default 0. */
void sched_set_joypad(int port, uint16_t buttons);

long sched_frame_count(void);
/* Called at each frame edge (line 261 ends), inside the frame run, with
   the number of frames completed: sched_frame() holds that frame, the
   DSP has its audio, and joypad changes made here are read by the next
   frame's auto-read. Same clock native or interpreted, so per-frame input
   and state capture belong here, not after sched_run_frame. */
void sched_set_frame_hook(void (*fn)(long frame));
/* Master clocks since sched_init, at the last instruction boundary. */
uint64_t sched_clock(void);
long sched_nmi_count(void);
int sched_line(void);

#endif
