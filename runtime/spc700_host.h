/* The SPC700: ares's cycle-accurate core (third_party/ares/spc700) driven
 * by runtime/spc700_host.cpp against the vendored APU's memory map, DSP
 * and timers. C interface. */
#ifndef CT_SPC700_HOST_H
#define CT_SPC700_HOST_H

#include <stdint.h>

#include "apu.h"

/* Power on: the core at the IPL ROM's reset vector, which takes the
   first two cycles to read. Call after apu_reset. */
void spc_host_reset(Apu *apu);
/* Run `cycles` more SPC700 cycles, stopping mid-instruction if that's
   where they end. */
void spc_host_run(uint32_t cycles);
/* SPC700 cycles run since reset (during an access: including the cycle
   making it). */
uint64_t spc_host_cycle(void);
/* Called after each cycle's access, and after each SPC700 write (with its
   address and value), when set. */
extern void (*spc_host_cycle_end)(void);
extern void (*spc_host_write_hook)(uint16_t address, uint8_t data);
/* A X Y SP PCL PCH PSW (wait | stop << 1): for state hashes. */
void spc_host_regs(uint8_t out[8]);

#endif
