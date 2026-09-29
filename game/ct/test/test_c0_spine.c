/* Bank $C0 reset path: the NMI and IRQ trampolines in WRAM.
 *
 * The ROM's native NMI and IRQ vectors point at $FF10 / $FF14, which are
 * JML $000500 / JML $000504: trampolines in WRAM, where the field engine
 * installs a JML to its handlers (derived from the ASM):
 *   Field_InstallNmiVector ($C00B64, m1x0): $0500 = 5C 63 EA C0  (JML $C0EA63)
 *   Field_InstallIrqVector ($C00B75, m1x0): $0504 = 5C CC EC C0  (JML $C0ECCC)
 *   exit: A low = $C0, X = $EA63 / $ECCC, nothing else in WRAM written.
 * Checked with DB $00 and $7E (both map $0500 to WRAM) over random WRAM, and
 * that the ROM vectors do lead to the trampolines. The rest of the spine
 * (VBlank/NMI, the main loop, sprites) is compared native against
 * interpreted on every frame by the lockstep replays.
 */
#include <string.h>

#include "ct_funcs.h"
#include "harness.h"

#define STACK_TOP 0x1FF0u

static uint8_t before[CT_WRAM_SIZE];

static void install_case(th_fn fn, uint16_t at, uint8_t db, uint16_t tramp, uint16_t target)
{
    for (unsigned k = 0; k < CT_WRAM_SIZE; k++)
        bus_wram()[k] = (uint8_t)rnd32();
    memcpy(before, bus_wram(), sizeof before);
    CPU c;
    cpu_init(&c);
    c.S = STACK_TOP;
    c.DB = db;
    c.PB = 0xC0;
    c.m = 1;
    c.x = 0;
    c.A = (uint16_t)rnd32();
    c.X = (uint16_t)rnd32();
    c.Y = (uint16_t)rnd32();
    uint16_t a_hi = c.A & 0xFF00, y = c.Y;
    call_jsr(&c, fn, 0x0000);
    const uint8_t *w = bus_wram();
    uint8_t exp[4] = {0x5C, (uint8_t)target, (uint8_t)(target >> 8), 0xC0};
    CHECK(!memcmp(w + tramp, exp, 4), "$%06X DB $%02X: $%04X = %02X %02X %02X %02X", 0xC00000u | at,
          db, tramp, w[tramp], w[tramp + 1], w[tramp + 2], w[tramp + 3]);
    int others = 1;
    for (unsigned k = 0; k < CT_WRAM_SIZE; k++)
        if ((k < tramp || k >= tramp + 4u) && w[k] != before[k] &&
            !(k >= STACK_TOP - 2 && k <= STACK_TOP))   /* the return address */
            others = 0;
    CHECK(others, "$%06X DB $%02X: wrote only the trampoline", 0xC00000u | at, db);
    CHECK(c.A == (a_hi | 0xC0) && c.X == target && c.Y == y && c.m && !c.x,
          "$%06X DB $%02X: A=%04X X=%04X", 0xC00000u | at, db, c.A, c.X);
}

int main(int argc, char **argv)
{
    th_args(argc, argv);
    bus_init(NULL);
    uint16_t nmi = (uint16_t)(bus_peek(0x00FFEA) | bus_peek(0x00FFEB) << 8);
    uint16_t irq = (uint16_t)(bus_peek(0x00FFEE) | bus_peek(0x00FFEF) << 8);
    CHECK(nmi == 0xFF10 && irq == 0xFF14, "ROM vectors NMI $%04X IRQ $%04X", nmi, irq);
    static const uint8_t stubs[8] = {0x5C, 0x00, 0x05, 0x00, 0x5C, 0x04, 0x05, 0x00};
    int stubs_ok = 1;
    for (unsigned k = 0; k < 8; k++)
        stubs_ok &= bus_peek(0x00FF10 + k) == stubs[k];
    CHECK(stubs_ok, "$FF10/$FF14: JML $000500 / JML $000504");
    for (int n = 0; n < 16; n++)
        for (int d = 0; d < 2; d++) {
            uint8_t db = d ? 0x7E : 0x00;
            install_case(f_C00B64_m1x0, 0x0B64, db, 0x0500, 0xEA63);
            install_case(f_C00B75_m1x0, 0x0B75, db, 0x0504, 0xECCC);
        }
    return th_report(th_interp ? "interp" : "spine");
}
