/* Bank $C1 number/text cluster ($C1:011F-$C1:01F8).
 *
 * Expected behaviour, derived from the ASM. tile(k) = ROM[$CCF903 + k]
 * (long,X; k may run past the 10-entry digit table and is still a ROM read).
 *
 * BattleMsg_FormatNumberDigits ($C1011F, m0x0, DB=$7E)
 *   in  v = $9499 (16-bit)
 *   out $949C = $FF, $949D = tile(v / 100), $949E = tile(v % 100 / 10),
 *       $949F = tile(v % 10); $949B untouched
 *       $9499 = (v % 10 - 10) & $FFFF (last tens-loop overshoot is stored,
 *       the restore is not)
 *       exit m1x0; A = $00FF; X restored by PLX, N/Z from it; C=1 V=0
 *       (final ADC); Y, DB unchanged
 *
 * Battle_DivTen9499 ($C10174, m0x0, DB=$7E)
 *   in  v = $9499
 *   out $949C = $949D = $FF, $949E = tile(v / 10), $949F = tile(v % 10)
 *       $9499 = (v % 10 - 10) & $FFFF; registers/flags as above
 *
 * BattleMsg_ReencodeTextBuffer ($C101A9, m1x0, any DB)
 *   in  s[0..15] = $7E:94A0..94AF
 *   out $94C0..94CF = s (MVN backup); $94A0..94BF = $FF, then for
 *       i = 0.. while i < 16 and s[i] != 0:
 *         s >= $73        tile s,       plane $FF
 *         $69 <= s < $73  tile s + $17, plane $72
 *         $40 <= s < $69  tile s + $40, plane $71
 *         0 < s < $40     tile s,       plane $FF
 *       tile -> $94A0+i, plane -> $94B0+i
 *       exit m1x0; DB = $7E; Y = $94D0; X = stop index (i or $10)
 *       stopped on $00: A = $0000, Z=1 N=0, C = entry C if i = 0 else 0
 *       all 16:         A = $00 | last plane, Z=1 N=0 C=1 (CPX #$10)
 *       V = 1 if any ADC ran (a char in $40..$72), else entry V
 *
 * All: S balanced, return to JSR site + 3, no other WRAM writes.
 */
#include <string.h>

#include "c1_text.h"
#include "harness.h"

#define STACK_TOP 0x1FF0u

static uint8_t snap[CT_WRAM_SIZE];

static void take_snap(void) { memcpy(snap, bus_wram(), CT_WRAM_SIZE); }

static void check_ram(const char *what, uint16_t lo, uint16_t hi)
{
    uint8_t *w = bus_wram();
    for (unsigned a = lo; a <= hi; a++)
        snap[a] = w[a];
    for (unsigned a = STACK_TOP - 15; a <= STACK_TOP; a++)
        snap[a] = w[a];
    if (memcmp(snap, w, CT_WRAM_SIZE)) {
        for (unsigned a = 0; a < CT_WRAM_SIZE; a++)
            if (snap[a] != w[a]) {
                CHECK(0, "%s: unexpected write at $%05X", what, 0x7E0000 + a);
                return;
            }
    }
    th_checks++;
}

static void entry_cpu(CPU *c, uint8_t m, uint8_t db)
{
    uint32_t r = rnd32();
    cpu_init(c);
    c->A = (uint16_t)rnd32();
    c->X = (uint16_t)rnd32();
    c->Y = (uint16_t)rnd32();
    c->S = STACK_TOP;
    c->DB = db;
    c->PB = 0xC1;
    c->m = m;
    c->n = r & 1;
    c->v = r >> 1 & 1;
    c->i = r >> 2 & 1;
    c->z = r >> 3 & 1;
    c->c = r >> 4 & 1;
}

static uint8_t tile(unsigned k) { return bus_rom()[0x0CF903 + k]; }
static uint8_t *w7e(uint16_t a) { return &bus_wram()[a]; }

/* ---- digit formatting ---- */

static void digits_case(int three, uint16_t v, int full)
{
    CPU c;
    entry_cpu(&c, 0, 0x7E);
    CPU in = c;
    uint8_t b = *w7e(0x949B);
    *w7e(0x9499) = (uint8_t)v;
    *w7e(0x949A) = (uint8_t)(v >> 8);
    if (full)
        take_snap();
    call_jsr(&c, three ? f_C1011F_m0x0 : f_C10174_m0x0, three ? 0x039A : 0x0444);
    const char *nm = three ? "format" : "divten";
    unsigned o = v % 10;
    uint8_t e9c = 0xFF, e9d, e9e, e9f = tile(o);
    if (three) {
        e9d = tile(v / 100);
        e9e = tile(v % 100 / 10);
    } else {
        e9d = 0xFF;
        e9e = tile(v / 10);
    }
    uint16_t left = (uint16_t)(*w7e(0x9499) | *w7e(0x949A) << 8);
    CHECK(*w7e(0x949C) == e9c && *w7e(0x949D) == e9d && *w7e(0x949E) == e9e &&
          *w7e(0x949F) == e9f, "%s v=%u digits %02X %02X %02X %02X", nm, v,
          *w7e(0x949C), *w7e(0x949D), *w7e(0x949E), *w7e(0x949F));
    CHECK(left == (uint16_t)(o - 10), "%s v=%u $9499=$%04X", nm, v, left);
    CHECK(*w7e(0x949B) == b, "%s $949B touched", nm);
    CHECK(c.m == 1 && c.x == 0 && c.A == 0x00FF, "%s exit m%d A=$%04X", nm, c.m, c.A);
    CHECK(c.X == in.X && c.Y == in.Y && c.DB == in.DB, "%s regs", nm);
    CHECK(c.n == (in.X >> 15) && c.z == (in.X == 0) && c.c == 1 && c.v == 0 &&
          c.i == in.i && c.d == 0, "%s flags", nm);
    if (full)
        check_ram(nm, 0x9499, 0x949F);
}

static void test_digits(int three)
{
    for (unsigned v = 0; v < 0x10000; v++)
        digits_case(three, (uint16_t)v, v % 257 == 0);
}

/* ---- text re-encode ---- */

static void reencode_case(const uint8_t s[16], int full)
{
    static const uint8_t dbs[] = {0x00, 0x7E, 0x7F, 0x80, 0xC1};
    CPU c;
    entry_cpu(&c, 1, dbs[rnd32() % 5]);
    CPU in = c;
    memcpy(w7e(0x94A0), s, 16);
    if (full)
        take_snap();
    call_jsr(&c, f_C101A9_m1x0, 0x09E2);

    uint8_t tile_e[16], plane_e[16];
    memset(tile_e, 0xFF, 16);
    memset(plane_e, 0xFF, 16);
    unsigned i = 0;
    int adc = 0;
    uint8_t last = 0;
    for (; i < 16 && s[i]; i++) {
        uint8_t ch = s[i];
        if (ch >= 0x73 || ch < 0x40) {
            tile_e[i] = ch;
            plane_e[i] = 0xFF;
        } else if (ch >= 0x69) {
            tile_e[i] = (uint8_t)(ch + 0x17);
            plane_e[i] = 0x72;
            adc = 1;
        } else {
            tile_e[i] = (uint8_t)(ch + 0x40);
            plane_e[i] = 0x71;
            adc = 1;
        }
        last = plane_e[i];
    }
    CHECK(!memcmp(w7e(0x94C0), s, 16), "reencode backup");
    CHECK(!memcmp(w7e(0x94A0), tile_e, 16), "reencode tiles (stop %u)", i);
    CHECK(!memcmp(w7e(0x94B0), plane_e, 16), "reencode planes (stop %u)", i);
    CHECK(c.m == 1 && c.x == 0 && c.DB == 0x7E && c.Y == 0x94D0 && c.X == i,
          "reencode regs X=$%04X Y=$%04X DB=$%02X", c.X, c.Y, c.DB);
    if (i < 16)
        CHECK(c.A == 0 && c.z == 1 && c.n == 0 && c.c == (i == 0 ? in.c : 0),
              "reencode stop %u A=$%04X c%d", i, c.A, c.c);
    else
        CHECK(c.A == last && c.z == 1 && c.n == 0 && c.c == 1, "reencode full A=$%04X", c.A);
    CHECK(c.v == (adc ? 1 : in.v) && c.i == in.i && c.d == 0, "reencode v%d", c.v);
    if (full)
        check_ram("reencode", 0x94A0, 0x94CF);
}

static uint8_t rnd_char(void)
{
    uint32_t r = rnd32();
    switch (r & 7) {
    case 0: return (uint8_t)(0x40 + (r >> 8) % 0x29);     /* $40-$68 */
    case 1: return (uint8_t)(0x69 + (r >> 8) % 0x0A);     /* $69-$72 */
    case 2: return (uint8_t)(0x73 + (r >> 8) % 0x8D);     /* $73-$FF */
    case 3: return (uint8_t)(0x01 + (r >> 8) % 0x3F);     /* $01-$3F */
    default: return (uint8_t)((r >> 8) | 1);
    }
}

static void test_reencode(void)
{
    uint8_t s[16];
    /* Every byte value at every position, other positions non-zero. */
    for (unsigned p = 0; p < 16; p++)
        for (unsigned b = 0; b < 256; b++) {
            for (unsigned k = 0; k < 16; k++)
                s[k] = rnd_char();
            s[p] = (uint8_t)b;
            reencode_case(s, b % 17 == 0);
        }
    /* Random strings, some with an early terminator. */
    for (unsigned n = 0; n < 1000000; n++) {
        for (unsigned k = 0; k < 16; k++)
            s[k] = rnd_char();
        if (n & 1)
            s[rnd32() % 16] = 0;
        reencode_case(s, n % 9973 == 0);
    }
}

int main(int argc, char **argv)
{
    const char *which = argc > 1 ? argv[1] : "all";
    int all = !strcmp(which, "all");
    bus_init(NULL);
    for (unsigned k = 0; k < CT_WRAM_SIZE; k++)
        bus_wram()[k] = (uint8_t)rnd32();
    if (all || !strcmp(which, "format"))
        test_digits(1);
    if (all || !strcmp(which, "divten"))
        test_digits(0);
    if (all || !strcmp(which, "reencode"))
        test_reencode();
    return th_report(which);
}
