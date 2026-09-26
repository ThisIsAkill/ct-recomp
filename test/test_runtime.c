/* Runtime: bus map, math unit, stack, flag helpers. */
#include <string.h>

#include "harness.h"

static void test_map(void)
{
    const uint8_t *rom = bus_rom();
    write8(0x7E0123, 0x5A);
    CHECK(read8(0x000123) == 0x5A, "low WRAM mirror bank $00");
    CHECK(read8(0x3F0123) == 0x5A, "low WRAM mirror bank $3F");
    CHECK(read8(0x800123) == 0x5A, "low WRAM mirror bank $80");
    write8(0x7F0123, 0xA5);
    CHECK(read8(0x7E0123) == 0x5A && bus_wram()[0x10123] == 0xA5, "bank $7F separate");
    write8(0x7E2000, 0x11);
    CHECK(bus_wram()[0x2000] == 0x11, "WRAM above mirror");

    write8(0x306000, 0x77);
    CHECK(read8(0x206000) == 0x77 && read8(0xB06000) == 0x77, "SRAM mirrors");

    CHECK(memcmp(&rom[0xFFC0], "CHRONO TRIGGER", 14) == 0, "ROM header");
    CHECK(read8(0xC0FFC0) == 'C' && read8(0x00FFC0) == 'C' && read8(0x40FFC0) == 'C' &&
          read8(0x80FFC0) == 'C', "ROM mirrors");
    CHECK(read8(0xC10089) == rom[0x10089], "bank $C1 offset");
    CHECK(read16(0x7EFFFF) == (uint16_t)(read8(0x7EFFFF) | read8(0x7F0000) << 8),
          "read16 crosses bank");
}

static void test_math_unit(void)
{
    for (unsigned a = 0; a < 256; a++)
        for (unsigned b = 0; b < 256; b++) {
            write8(0x004202, (uint8_t)a);
            write8(0x004203, (uint8_t)b);
            unsigned p = read8(0x004216) | read8(0x004217) << 8;
            CHECK(p == a * b, "%u*%u -> %u", a, b, p);
        }
    for (unsigned n = 0; n < 0x10000; n++)
        for (unsigned d = 0; d < 256; d++) {
            write8(0x004204, (uint8_t)n);
            write8(0x004205, (uint8_t)(n >> 8));
            write8(0x004206, (uint8_t)d);
            unsigned q = read8(0x004214) | read8(0x004215) << 8;
            unsigned r = read8(0x004216) | read8(0x004217) << 8;
            unsigned eq = d ? n / d : 0xFFFF, er = d ? n % d : n;
            CHECK(q == eq && r == er, "%u/%u -> q=%u r=%u", n, d, q, r);
        }
}

static void test_stack(void)
{
    CPU c;
    cpu_init(&c);
    c.S = 0x1FF0;
    push8(&c, 0x12);
    push16(&c, 0x3456);
    CHECK(c.S == 0x1FED, "S after 3 pushes");
    CHECK(read8(0x001FF0) == 0x12 && read8(0x001FEF) == 0x34 && read8(0x001FEE) == 0x56,
          "stack bytes");
    CHECK(pull16(&c) == 0x3456 && pull8(&c) == 0x12 && c.S == 0x1FF0, "pulls");

    c.e = 1;
    c.S = 0x0100;
    push8(&c, 0x99);
    CHECK(c.S == 0x01FF && read8(0x000100) == 0x99, "emulation stack wraps in page 1");
    CHECK(pull8(&c) == 0x99 && c.S == 0x0100, "emulation pull wraps");
}

static void test_index_ops(void)
{
    CPU c;
    cpu_init(&c);
    c.DB = 0x7E;
    c.X = 0x0010;
    CHECK(ea_abs_x(&c, 0xFFF8) == 0x7F0008, "abs,X carries into next bank");
    CHECK(ea_long_x(&c, 0xCCFFF8) == 0xCD0008, "long,X carries into next bank");
    c.X = 0xFFFF;
    inx16(&c);
    CHECK(c.X == 0 && c.z == 1, "INX wraps");
    dex16(&c);
    CHECK(c.X == 0xFFFF && c.n == 1, "DEX wraps");

    for (unsigned k = 0; k < 8; k++)
        write8(0x7E3000 + k, (uint8_t)(0xA0 + k));
    c.X = 0x3000;
    c.Y = 0x3100;
    c.A = 7;
    c.DB = 0x00;
    mvn16(&c, 0x7F, 0x7E);
    CHECK(c.A == 0xFFFF && c.X == 0x3008 && c.Y == 0x3108 && c.DB == 0x7F, "MVN regs");
    CHECK(read8(0x7F3100) == 0xA0 && read8(0x7F3107) == 0xA7, "MVN data");
}

static void test_p(void)
{
    CPU c;
    cpu_init(&c);
    c.X = 0x1234;
    c.Y = 0xABCD;
    op_sep(&c, 0x10);
    CHECK(c.x == 1 && c.X == 0x34 && c.Y == 0xCD, "SEP #$10 clears index high bytes");
    op_rep(&c, 0x30);
    CHECK(c.m == 0 && c.x == 0, "REP #$30");
    op_sep(&c, 0xFF);
    CHECK(get_p(&c) == 0xFF, "SEP #$FF");
    op_rep(&c, 0xFF);
    CHECK(get_p(&c) == 0x00, "REP #$FF");
    c.e = 1;
    op_rep(&c, 0x30);
    CHECK(c.m == 1 && c.x == 1, "emulation keeps m/x set");
}

static void test_arith(void)
{
    CPU c;
    cpu_init(&c);
    for (int a = 0; a < 256; a++)
        for (int b = 0; b < 256; b++)
            for (int ci = 0; ci < 2; ci++) {
                int sr = (int8_t)a + (int8_t)b + ci;
                int ur = a + b + ci;
                c.A = (uint16_t)(0xEE00 | a);
                c.c = (uint8_t)ci;
                adc8(&c, (uint8_t)b, 0);
                CHECK(c.A == (0xEE00 | (ur & 0xFF)) && c.c == (ur > 255) &&
                      c.v == (sr < -128 || sr > 127) && c.z == ((ur & 0xFF) == 0) &&
                      c.n == ((ur >> 7) & 1), "adc8 %d+%d+%d", a, b, ci);

                sr = (int8_t)a - (int8_t)b - (1 - ci);
                ur = a - b - (1 - ci);
                c.A = (uint16_t)(0xEE00 | a);
                c.c = (uint8_t)ci;
                sbc8(&c, (uint8_t)b, 0);
                CHECK((c.A & 0xFF) == (ur & 0xFF) && c.c == (ur >= 0) &&
                      c.v == (sr < -128 || sr > 127), "sbc8 %d-%d-%d", a, b, 1 - ci);
            }
    for (int k = 0; k < 1000000; k++) {
        uint32_t r = rnd32();
        int a = (int)(r & 0xFFFF), b = (int)(rnd32() & 0xFFFF), ci = (int)(r >> 31);
        int sr = (int16_t)a + (int16_t)b + ci;
        int ur = a + b + ci;
        c.A = (uint16_t)a;
        c.c = (uint8_t)ci;
        adc16(&c, (uint16_t)b, 0);
        CHECK(c.A == (ur & 0xFFFF) && c.c == (ur > 0xFFFF) &&
              c.v == (sr < -32768 || sr > 32767) && c.n == ((ur >> 15) & 1),
              "adc16 %d+%d+%d", a, b, ci);
        sr = (int16_t)a - (int16_t)b - (1 - ci);
        ur = a - b - (1 - ci);
        c.A = (uint16_t)a;
        c.c = (uint8_t)ci;
        sbc16(&c, (uint16_t)b, 0);
        CHECK(c.A == (ur & 0xFFFF) && c.c == (ur >= 0) &&
              c.v == (sr < -32768 || sr > 32767), "sbc16 %d-%d-%d", a, b, 1 - ci);
        cmp16(&c, (uint16_t)a, (uint16_t)b);
        CHECK(c.c == (a >= b) && c.z == (a == b), "cmp16 %d %d", a, b);
    }
}

/* Fatal paths: each must print its message and exit nonzero. */
static int fatal_case(const char *name)
{
    CPU c;
    cpu_init(&c);
    if (!strcmp(name, "fatal_open_bus")) read8(0x006000);
    if (!strcmp(name, "fatal_rom_write")) write8(0xC10000, 0);
    if (!strcmp(name, "fatal_unhooked")) read8(0x004016);
    if (!strcmp(name, "fatal_decimal")) { c.d = 1; adc8(&c, 1, 0xC10000); }
    if (!strcmp(name, "fatal_entry")) { c.m = 0; cpu_enter(&c, 0xC10089, 1, 0); }
    fprintf(stderr, "no fatal for %s\n", name);
    return 0;
}

int main(int argc, char **argv)
{
    bus_init(NULL);
    if (argc > 1)
        return fatal_case(argv[1]);
    test_map();
    bus_reset();
    test_math_unit();
    bus_reset();
    test_stack();
    test_index_ops();
    test_p();
    test_arith();
    return th_report("runtime");
}
