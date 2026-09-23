/* Runtime hooks for [[extern]] call boundaries in funcs.toml.
 * Entered with the caller's return frame on the stack; a hook that returns
 * must perform the matching RTS/RTL itself. */
#include "bus.h"
#include "cpu.h"
#include "ops.h"

/* $C70004 Audio_Process_Entry: SPC700 command interface. No APU model yet. */
void ct_hook_apu_process(CPU *cpu);
void ct_hook_apu_process(CPU *cpu)
{
    (void)cpu;
    ct_fatal("$C70004: APU boundary: audio driver not implemented");
}

/* $7E9690: self-modifying stub "MVN dst,src ; RTL" whose banks callers patch
   before each JSL. Executes exactly those bytes. */
void ct_hook_wram_mvn_stub(CPU *cpu);
void ct_hook_wram_mvn_stub(CPU *cpu)
{
    uint8_t op = read8(0x7E9690), dst = read8(0x7E9691), src = read8(0x7E9692);
    uint8_t ret = read8(0x7E9693);
    if (op != 0x54 || ret != 0x6B)
        ct_fatal("$7E9690: RAM stub is %02X %02X %02X %02X, expected MVN/RTL", op, dst, src, ret);
    if (cpu->x)
        ct_fatal("$7E9690: RAM stub MVN with 8-bit index not supported");
    mvn16(cpu, dst, src);
    op_rtl(cpu);
}

/* $7E3000: program decompressed into WRAM at boot ($C30073). Not recompiled. */
void ct_hook_wram_boot_program(CPU *cpu);
void ct_hook_wram_boot_program(CPU *cpu)
{
    (void)cpu;
    ct_fatal("$7E3000: RAM-resident boot program not recompiled");
}
