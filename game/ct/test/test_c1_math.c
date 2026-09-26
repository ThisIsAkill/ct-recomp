/* Bank $C1 math cluster ($C1:0089-$C1:011E) against plain arithmetic.
 *
 * Expected behaviour, derived from the ASM:
 *
 * Battle_Mul8 ($C10089, m1x0)
 *   in  $AD, $AE
 *   out $AF/$B0 = $AD * $AE; $77 = $AD; $78 = $AE
 *       A = DP (TDC), N/Z from DP; M=1 X=0; C V I D, X, Y, DB unchanged
 *
 * Battle_MulAccum ($C100A7, m1x0)
 *   in  $A5; $A7 (lo), $A8 (hi)
 *   out $A9/$AA/$AB = $A5 * (($A8 << 8) | $A7)  (8x16 -> 24; WRMPYA keeps $A5
 *       for the second multiply; $AA is not an input)
 *       $77 = $A5, $78 = $A8, X = $A5 * $A7, A = DP
 *       PLP restores P from after LDA $A7, then PLB restores DB: N/Z from
 *       DB; C V I D from entry; M=1 X=0; Y unchanged
 *
 * Battle_Divide ($C100D7, m1x0)
 *   in  $B1/$B2 dividend, $B3 divisor
 *   out $B5/$B6 quotient, $B7/$B8 remainder (divisor 0: $FFFF, dividend)
 *       $79/$7A/$7B = $B1/$B2/$B3; DB:$A029 incremented then cleared -> 0
 *       A = DP, N/Z from DP; C V I D, X, Y, DB unchanged
 *
 * Shift entries (m1x0 and m0x0): ASL A / LSR A chains ending in one RTS.
 *   $C1010D ShiftLeft7  8x ASL      $C10116 Loc_C10116  8x LSR
 *   $C10111 ShiftLeft4  4x ASL      $C10118 ShiftRight6 6x LSR
 *   $C10112 ShiftLeft3  3x ASL      $C10119 ShiftRight5 5x LSR
 *                                   $C1011A ShiftRight4 4x LSR
 *                                   $C1011B ShiftRight3 3x LSR
 *   m1: low byte shifted, B unchanged; m0: full 16 bits.
 *   C = last bit shifted out, N/Z from result; V, X, Y, DB unchanged.
 *
 * All: S balanced, return to JSR site + 3, no WRAM writes other than the
 * listed outputs and the stack bytes below entry S.
 */
#include <string.h>

#include "c1_math.h"
#include "harness.h"

#define STACK_TOP 0x1FF0u

static uint8_t snap[CT_WRAM_SIZE];

static void fill_wram(void)
{
    uint8_t *w = bus_wram();
    for (unsigned k = 0; k < CT_WRAM_SIZE; k++)
        w[k] = (uint8_t)rnd32();
}

static void take_snap(void) { memcpy(snap, bus_wram(), CT_WRAM_SIZE); }

/* Compare WRAM with the snapshot, ignoring listed bank-$7E offsets and the
   stack bytes [STACK_TOP-15, STACK_TOP]. */
static void check_ram(const char *what, const uint16_t *allow, int n)
{
    uint8_t *w = bus_wram();
    for (int k = 0; k < n; k++)
        snap[allow[k]] = w[allow[k]];
    for (unsigned a = STACK_TOP - 15; a <= STACK_TOP; a++)
        snap[a] = w[a];
    int diff = memcmp(snap, w, CT_WRAM_SIZE);
    if (diff) {
        for (unsigned a = 0; a < CT_WRAM_SIZE; a++)
            if (snap[a] != w[a]) {
                CHECK(0, "%s: unexpected write at $%05X", what, 0x7E0000 + a);
                return;
            }
    }
    th_checks++;
}

static void entry_cpu(CPU *c, uint8_t m, uint16_t dp, uint8_t db)
{
    uint32_t r = rnd32();
    cpu_init(c);
    c->A = (uint16_t)rnd32();
    c->X = (uint16_t)rnd32();
    c->Y = (uint16_t)rnd32();
    c->S = STACK_TOP;
    c->DP = dp;
    c->DB = db;
    c->PB = 0xC1;
    c->m = m;
    c->x = 0;
    c->n = r & 1;
    c->v = r >> 1 & 1;
    c->i = r >> 2 & 1;
    c->z = r >> 3 & 1;
    c->c = r >> 4 & 1;
    c->d = 0;
}

static uint8_t wr(uint16_t dp, uint8_t off) { return bus_wram()[(uint16_t)(dp + off)]; }
static void ww(uint16_t dp, uint8_t off, uint8_t v) { bus_wram()[(uint16_t)(dp + off)] = v; }
static uint16_t wr16(uint16_t dp, uint8_t off) { return (uint16_t)(wr(dp, off) | wr(dp, off + 1) << 8); }

/* ---- Battle_Mul8 ---- */

static void mul8_case(uint8_t a, uint8_t b, uint16_t dp, int full)
{
    CPU c;
    entry_cpu(&c, 1, dp, (uint8_t)(rnd32() & 1 ? 0x7E : 0x80));
    CPU in = c;
    ww(dp, 0xAD, a);
    ww(dp, 0xAE, b);
    if (full)
        take_snap();
    call_jsr(&c, f_C10089_m1x0, 0x14A7);
    unsigned p = wr16(dp, 0xAF);
    CHECK(p == (unsigned)a * b, "mul8 %u*%u = %u", a, b, p);
    CHECK(wr(dp, 0x77) == a && wr(dp, 0x78) == b, "mul8 mirrors");
    CHECK(c.A == dp && c.n == (dp >> 15) && c.z == (dp == 0), "mul8 A/NZ");
    CHECK(c.m == 1 && c.x == 0 && c.c == in.c && c.v == in.v && c.i == in.i && c.d == 0,
          "mul8 flags");
    CHECK(c.X == in.X && c.Y == in.Y && c.DB == in.DB && c.DP == dp, "mul8 regs");
    if (full) {
        uint16_t allow[] = {(uint16_t)(dp + 0x77), (uint16_t)(dp + 0x78),
                            (uint16_t)(dp + 0xAF), (uint16_t)(dp + 0xB0)};
        check_ram("mul8", allow, 4);
    }
}

static void test_mul8(void)
{
    for (unsigned a = 0; a < 256; a++)
        for (unsigned b = 0; b < 256; b++)
            mul8_case((uint8_t)a, (uint8_t)b, 0, ((a * 256 + b) % 257) == 0);
    for (int k = 0; k < 10000; k++)
        mul8_case((uint8_t)rnd32(), (uint8_t)rnd32(), (uint16_t)(rnd32() % 0x1E00), k % 100 == 0);
}

/* ---- Battle_MulAccum ---- */

static void mulaccum_case(uint8_t mul, uint8_t lo, uint8_t hi, int full)
{
    static const uint8_t dbs[] = {0x00, 0x7E, 0x7F, 0x80, 0xC1};
    CPU c;
    entry_cpu(&c, 1, 0, dbs[rnd32() % 5]);
    CPU in = c;
    ww(0, 0xA5, mul);
    ww(0, 0xA7, lo);
    ww(0, 0xA8, hi);
    if (full)
        take_snap();
    call_jsr(&c, f_C100A7_m1x0, 0x021C);
    uint32_t got = wr(0, 0xA9) | wr(0, 0xAA) << 8 | (uint32_t)wr(0, 0xAB) << 16;
    uint32_t exp = (uint32_t)mul * (uint32_t)(hi << 8 | lo);
    CHECK(got == exp, "mulaccum %u*$%02X%02X = %u, expected %u", mul, hi, lo, got, exp);
    CHECK(wr(0, 0x77) == mul && wr(0, 0x78) == hi, "mulaccum mirrors");
    CHECK(c.X == (uint16_t)(mul * lo) && c.A == 0 && c.Y == in.Y, "mulaccum regs");
    CHECK(c.n == (in.DB >> 7) && c.z == (in.DB == 0), "mulaccum NZ from DB");
    CHECK(c.m == 1 && c.x == 0 && c.c == in.c && c.v == in.v && c.i == in.i && c.d == 0,
          "mulaccum flags");
    CHECK(c.DB == in.DB, "mulaccum DB restored");
    if (full) {
        uint16_t allow[] = {0x77, 0x78, 0xA9, 0xAA, 0xAB};
        check_ram("mulaccum", allow, 5);
    }
}

static void test_mulaccum(void)
{
    uint32_t k = 0;
    for (unsigned mul = 0; mul < 256; mul++)
        for (unsigned hi = 0; hi < 256; hi++)
            for (unsigned lo = 0; lo < 256; lo++, k++)
                mulaccum_case((uint8_t)mul, (uint8_t)lo, (uint8_t)hi, k % 65521 == 0);
}

/* ---- Battle_Divide ---- */

static void divide_case(uint16_t n, uint8_t d, uint8_t db, int full)
{
    CPU c;
    entry_cpu(&c, 1, 0, db);
    CPU in = c;
    uint32_t guard = ((uint32_t)db << 16 | 0xA029) - 0x7E0000;
    bus_wram()[guard] = (uint8_t)rnd32();
    ww(0, 0xB1, (uint8_t)n);
    ww(0, 0xB2, (uint8_t)(n >> 8));
    ww(0, 0xB3, d);
    if (full)
        take_snap();
    call_jsr(&c, f_C100D7_m1x0, 0x071D);
    unsigned q = wr16(0, 0xB5), r = wr16(0, 0xB7);
    unsigned eq = d ? n / d : 0xFFFF, er = d ? n % d : n;
    CHECK(q == eq && r == er, "divide %u/%u = q%u r%u, expected q%u r%u", n, d, q, r, eq, er);
    CHECK(wr(0, 0x79) == (uint8_t)n && wr(0, 0x7A) == n >> 8 && wr(0, 0x7B) == d,
          "divide mirrors");
    CHECK(bus_wram()[guard] == 0, "divide guard cleared");
    CHECK(c.A == 0 && c.n == 0 && c.z == 1, "divide A/NZ");
    CHECK(c.m == 1 && c.x == 0 && c.c == in.c && c.v == in.v && c.i == in.i && c.d == 0,
          "divide flags");
    CHECK(c.X == in.X && c.Y == in.Y && c.DB == in.DB, "divide regs");
    if (full) {
        uint16_t allow[] = {0x79, 0x7A, 0x7B, 0xB5, 0xB6, 0xB7, 0xB8};
        snap[guard] = 0;
        check_ram("divide", allow, 7);
    }
}

static void test_divide(void)
{
    static const struct { uint16_t n; uint8_t d; } edge[] = {
        {0, 0}, {1, 0}, {0xFFFF, 0}, {0x1234, 0}, {0, 1}, {1, 1}, {0xFFFF, 1},
        {0xFFFF, 0xFF}, {0xFFFF, 0x80}, {0xFF, 0xFF}, {0xFE, 0xFF}, {0x100, 2},
        {0x7FFF, 3}, {0x8000, 0x80}, {0xFF00, 0xFF}, {0, 0xFF},
    };
    for (unsigned k = 0; k < sizeof edge / sizeof edge[0]; k++) {
        divide_case(edge[k].n, edge[k].d, 0x7E, 1);
        divide_case(edge[k].n, edge[k].d, 0x7F, 1);
    }
    uint32_t k = 0;
    for (unsigned n = 0; n < 0x10000; n++)
        for (unsigned d = 0; d < 256; d++, k++)
            divide_case((uint16_t)n, (uint8_t)d, 0x7E, k % 65521 == 0);
    for (k = 0; k < 1000000; k++)
        divide_case((uint16_t)rnd32(), (uint8_t)rnd32(), rnd32() & 1 ? 0x7E : 0x7F,
                    k % 9973 == 0);
}

/* ---- shifts ---- */

static const struct {
    const char *name;
    void (*m1)(CPU *);
    void (*m0)(CPU *);
    int count;          /* >0: ASL count, <0: LSR count */
    uint16_t caller;    /* a real JSR site for this entry */
} shifts[] = {
    {"ShiftLeft7",  f_C1010D_m1x0, f_C1010D_m0x0,  8, 0x0715},
    {"ShiftLeft4",  f_C10111_m1x0, f_C10111_m0x0,  4, 0x30CF},
    {"ShiftLeft3",  f_C10112_m1x0, f_C10112_m0x0,  3, 0x3410},
    {"Loc_C10116",  f_C10116_m1x0, f_C10116_m0x0, -8, 0x2D73},
    {"ShiftRight6", f_C10118_m1x0, f_C10118_m0x0, -6, 0x0369},
    {"ShiftRight5", f_C10119_m1x0, f_C10119_m0x0, -5, 0x39F6},
    {"ShiftRight4", f_C1011A_m1x0, f_C1011A_m0x0, -4, 0x2708},
    {"ShiftRight3", f_C1011B_m1x0, f_C1011B_m0x0, -3, 0x0724},
};

static void test_shifts(void)
{
    for (unsigned s = 0; s < sizeof shifts / sizeof shifts[0]; s++) {
        int n = shifts[s].count < 0 ? -shifts[s].count : shifts[s].count;
        int left = shifts[s].count > 0;
        for (int m = 0; m <= 1; m++)
            for (unsigned a = 0; a < 0x10000; a++) {
                CPU c;
                entry_cpu(&c, (uint8_t)m, 0, 0x7E);
                c.A = (uint16_t)a;
                CPU in = c;
                int full = a % 4093 == 0;
                if (full)
                    take_snap();
                call_jsr(&c, m ? shifts[s].m1 : shifts[s].m0, shifts[s].caller);
                unsigned bits = m ? 8 : 16, mask = m ? 0xFF : 0xFFFF;
                unsigned v = m ? (a & 0xFF) : a;
                unsigned res = left ? (v << n) & mask : v >> n;
                unsigned carry = left ? (v >> (bits - n)) & 1 : (v >> (n - 1)) & 1;
                unsigned got = c.A & mask;
                CHECK(got == res, "%s m%d A=$%04X -> $%04X, expected $%04X",
                      shifts[s].name, m, a, got, res);
                if (m)
                    CHECK((c.A >> 8) == (a >> 8), "%s m1 B preserved", shifts[s].name);
                CHECK(c.c == carry && c.n == ((res >> (bits - 1)) & 1) && c.z == (res == 0),
                      "%s m%d A=$%04X flags c%d n%d z%d", shifts[s].name, m, a, c.c, c.n, c.z);
                CHECK(c.m == m && c.x == 0 && c.v == in.v && c.i == in.i && c.d == 0,
                      "%s mode flags", shifts[s].name);
                CHECK(c.X == in.X && c.Y == in.Y && c.DB == in.DB && c.DP == in.DP,
                      "%s regs", shifts[s].name);
                if (full)
                    check_ram(shifts[s].name, NULL, 0);
            }
    }
}

int main(int argc, char **argv)
{
    const char *which = argc > 1 ? argv[1] : "all";
    th_args(argc, argv);
    int all = !strcmp(which, "all");
    bus_init(NULL);
    fill_wram();
    if (all || !strcmp(which, "mul8"))
        test_mul8();
    if (all || !strcmp(which, "mulaccum"))
        test_mulaccum();
    if (all || !strcmp(which, "divide"))
        test_divide();
    if (all || !strcmp(which, "shifts"))
        test_shifts();
    return th_report(th_interp ? "interp" : which);
}
