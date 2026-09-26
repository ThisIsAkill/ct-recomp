/* Bank $C1 status-bar UI.
 *
 * BattleUI_DrawSlotGaugeBar ($C106F0, m1x0, DB=$7E), derived from the ASM:
 *   in  $80/$81 slot (16-bit), cur = $99DD[slot], max = $9F22[slot]
 *   dividend = (cur << 8) & $FFFF      (ShiftLeft7: 8 shifts of 16-bit A)
 *   q  = max ? dividend / max : $FFFF  (Divide; $B5/$B6)
 *   r3 = q >> 3                        (ShiftRight3, 16-bit; stored to $B5)
 *   u  = ($20 - (r3 & $FF)) & $FF      (8-bit SBC)
 *   blocks = u >> 3, frac = u & 7
 *   Y0 = ROM16[$CCFA35 + 2*slot] + $1A
 *   $0CC0/2/4/6+Y0 = $67; $0CC1/3/5/7+Y0 = cur ? $2D : $29
 *   then blocks x ($6F at $0CC0+Y, Y += 2); then if frac: $67+frac at $0CC0+Y
 *   exit: Y = Y0 + 2*blocks, $82 = frac, $83 = 0, $86 = blocks << 3, M=1
 * Tested for slot 0-2 and every (cur, max) pair; other state is covered by
 * test_diff.
 */
#include <string.h>

#include "ct_funcs.h"
#include "harness.h"

#define STACK_TOP 0x1FF0u

static uint8_t *w7e(uint32_t a) { return &bus_wram()[a & 0x1FFFF]; }

static void gauge_case(unsigned slot, unsigned cur, unsigned max)
{
    CPU c;
    cpu_init(&c);
    c.S = STACK_TOP;
    c.DB = 0x7E;
    c.PB = 0xC1;
    c.A = (uint16_t)rnd32();
    c.X = (uint16_t)rnd32();
    c.Y = (uint16_t)rnd32();
    *w7e(0x80) = (uint8_t)slot;
    *w7e(0x81) = 0;
    *w7e(0x99DD + slot) = (uint8_t)cur;
    *w7e(0x9F22 + slot) = (uint8_t)max;

    const uint8_t *rom = bus_rom();
    unsigned y0 = (unsigned)(rom[0x0CFA35 + 2 * slot] | rom[0x0CFA36 + 2 * slot] << 8);
    y0 = (y0 + 0x1A) & 0xFFFF;
    unsigned dividend = (cur << 8) & 0xFFFF;
    unsigned q = max ? dividend / max : 0xFFFF;
    unsigned r3 = q >> 3;
    unsigned u = (0x20 - (r3 & 0xFF)) & 0xFF;
    unsigned blocks = u >> 3, frac = u & 7;

    uint8_t exp[0x80];
    for (unsigned k = 0; k < sizeof exp; k++)
        exp[k] = *w7e(0x0CC0 + y0 + k);
    for (unsigned k = 0; k < 4; k++) {
        exp[2 * k] = 0x67;
        exp[2 * k + 1] = cur ? 0x2D : 0x29;
    }
    for (unsigned k = 0; k < blocks; k++)
        exp[2 * k] = 0x6F;
    if (frac)
        exp[2 * blocks] = (uint8_t)(0x67 + frac);

    call_jsr(&c, f_C106F0_m1x0, 0x0000);
    int same = 1;
    for (unsigned k = 0; k < sizeof exp; k++)
        same &= *w7e(0x0CC0 + y0 + k) == exp[k];
    CHECK(same, "gauge slot %u cur %u max %u: tilemap", slot, cur, max);
    CHECK(c.Y == (uint16_t)(y0 + 2 * blocks) && c.m == 1 && c.x == 0,
          "gauge slot %u cur %u max %u: Y=$%04X", slot, cur, max, c.Y);
    CHECK(*w7e(0x82) == frac && *w7e(0x83) == 0 && *w7e(0x86) == (uint8_t)(blocks << 3) &&
          (unsigned)(*w7e(0xB5) | *w7e(0xB6) << 8) == r3, "gauge slot %u cur %u max %u: scratch",
          slot, cur, max);
}

int main(int argc, char **argv)
{
    th_args(argc, argv);
    bus_init(NULL);
    for (unsigned k = 0; k < CT_WRAM_SIZE; k++)
        bus_wram()[k] = (uint8_t)rnd32();
    for (unsigned slot = 0; slot < 3; slot++)
        for (unsigned cur = 0; cur < 256; cur++)
            for (unsigned max = 0; max < 256; max++)
                gauge_case(slot, cur, max);
    return th_report(th_interp ? "interp" : "gauge");
}
