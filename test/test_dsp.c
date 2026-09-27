/* Vendored S-DSP (third_party/snes/dsp.c) against fullsnes and bsnes
 * SPC_DSP where the two agree: GAIN increases saturate at 7FFh, BRR shift
 * 13-15 decodes as shift 12 with nibble SAR 3, and key-on clears the
 * voice's ENDX bit. Voice 0 plays a looping one-block BRR sample from its
 * own 64 KB of APU RAM. */
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "dsp.h"
#include "dsp_regs.h"

static uint8_t ram[0x10000];

/* Directory at $0100 (entry 0 -> $0200), one BRR block at $0200:
   header (shift, filter 0, end+loop) and 8 bytes of `nibble` pairs. */
static Dsp *voice(uint8_t shift, uint8_t nibble)
{
    memset(ram, 0, sizeof ram);
    ram[0x100] = 0x00, ram[0x101] = 0x02, ram[0x102] = 0x00, ram[0x103] = 0x02;
    ram[0x200] = (uint8_t)(shift << 4 | 0x03);
    memset(ram + 0x201, (uint8_t)(nibble << 4 | nibble), 8);
    Dsp *d = dsp_init(ram);
    dsp_reset(d);
    dsp_write(d, FLG, 0x20);   /* out of reset, unmuted, no echo writes */
    dsp_write(d, DIR, 0x01);
    dsp_write(d, V0SRCN, 0);
    dsp_write(d, V0PITCHL, 0x00);
    dsp_write(d, V0PITCHH, 0x10);
    return d;
}

int main(void)
{
    /* GAIN linear increase (mode 2, rate 31: every sample) from 0. */
    Dsp *d = voice(12, 1);
    dsp_write(d, V0ADSR1, 0x00);
    dsp_write(d, V0GAIN, 0xDF);
    dsp_write(d, KON, 0x01);
    int top = 0, after = 0;
    for (int k = 0; k < 200; k++) {
        dsp_cycle(d);
        if (k == 100)
            top = dsp_read(d, V0ENVX);
        after = dsp_read(d, V0ENVX);
    }
    CHECK(top == 0x7F && after == 0x7F, "linear increase holds at 7FFh: ENVX %02X then %02X",
          top, after);
    dsp_free(d);

    /* Bent increase likewise. */
    d = voice(12, 1);
    dsp_write(d, V0ADSR1, 0x00);
    dsp_write(d, V0GAIN, 0xFF);
    dsp_write(d, KON, 0x01);
    for (int k = 0; k < 400; k++)
        dsp_cycle(d);
    CHECK(dsp_read(d, V0ENVX) == 0x7F, "bent increase holds at 7FFh: ENVX %02X",
          dsp_read(d, V0ENVX));
    dsp_free(d);

    /* Shift 13, nibble -1: -800h. Direct gain 7Fh*16. OUTX is the 16-bit
       voice output >> 8, and 15-bit samples are doubled on the way in:
       -1000h * 7F0h / 800h ~ -FE0h -> OUTX -16 (F0h). Before the fix the
       sample was -1000h: OUTX -32. */
    d = voice(13, 0xF);
    dsp_write(d, V0ADSR1, 0x00);
    dsp_write(d, V0GAIN, 0x7F);
    dsp_write(d, KON, 0x01);
    for (int k = 0; k < 64; k++)
        dsp_cycle(d);
    int outx = (int8_t)dsp_read(d, V0OUTX);
    CHECK(outx == -16, "shift 13 nibble -1 decodes to -800h: OUTX %d", outx);
    dsp_free(d);

    /* ENDX: set by reset and by the looping end block; key-on clears it. */
    d = voice(12, 1);
    dsp_write(d, V0ADSR1, 0x00);
    dsp_write(d, V0GAIN, 0x7F);
    CHECK(dsp_read(d, ENDX) & 1, "ENDX.0 set after reset");
    dsp_write(d, KON, 0x01);
    CHECK(!(dsp_read(d, ENDX) & 1), "key-on clears ENDX.0");
    CHECK((dsp_read(d, ENDX) & 0xFE) == 0xFE, "other voices' ENDX bits kept");
    for (int k = 0; k < 64; k++)
        dsp_cycle(d);
    CHECK(dsp_read(d, ENDX) & 1, "end block sets ENDX.0 again");
    dsp_free(d);
    return th_report("dsp");
}
