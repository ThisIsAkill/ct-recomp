/* Runtime hooks for [[extern]] call boundaries in funcs.toml.
 * Entered with the caller's return frame on the stack; a hook that returns
 * must perform the matching RTS/RTL itself. */
#include "cpu.h"

/* $C70004 Audio_Process_Entry: SPC700 command interface. No APU model yet. */
void ct_hook_apu_process(CPU *cpu);
void ct_hook_apu_process(CPU *cpu)
{
    (void)cpu;
    ct_fatal("$C70004: APU boundary: audio driver not implemented");
}
