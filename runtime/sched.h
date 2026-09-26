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

/* The DSP output of the frame just run, resampled to `samples` stereo
   frames (L R interleaved) at 32040 Hz * samples / 534. Call once per
   frame: it also empties the DSP buffer, which otherwise stops filling. */
void sched_audio(int16_t *stereo, int samples);

/* Last rendered frame: SCHED_WIDTH x SCHED_HEIGHT pixels, bytes B G R 0. */
const uint8_t *sched_frame(void);

/* Buttons held on controller port 0-3, read by the next auto-joypad read
   (VBlank start, NMITIMEN bit 0). Bit 15 B, 14 Y, 13 Select, 12 Start,
   11 Up, 10 Down, 9 Left, 8 Right, 7 A, 6 X, 5 L, 4 R. Default 0. */
void sched_set_joypad(int port, uint16_t buttons);

long sched_frame_count(void);
long sched_nmi_count(void);
int sched_line(void);

#endif
