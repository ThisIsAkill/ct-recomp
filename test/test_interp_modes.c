/* Interpreter modes (runtime/interp.h). Strict must keep rejecting exactly
 * what it rejected before system mode existed, with the same messages;
 * system mode must accept the same programs as plain 65816 code. Programs
 * run from WRAM at $7E2000. */
#include <setjmp.h>

#include "harness.h"

static jmp_buf fatal_jmp;
static char fatal_msg[256];

static void on_fatal(const char *msg)
{
    snprintf(fatal_msg, sizeof fatal_msg, "%s", msg);
    longjmp(fatal_jmp, 1);
}

static void load(const uint8_t *code, unsigned n, uint16_t at)
{
    memcpy(bus_wram() + at, code, n);
}

static void cpu_at(CPU *c, uint16_t pc)
{
    cpu_init(c);
    c->PB = 0x7E;
    c->DB = 0x7E;
    c->PC = pc;
    c->S = 0x01F0;
}

/* Strict: run as a called routine; returns the fatal message, or "" if it
   returned normally. */
static const char *strict(CPU *c, uint16_t entry)
{
    fatal_msg[0] = 0;
    if (setjmp(fatal_jmp))
        return fatal_msg;
    push16(c, 0x1233);
    interp_call(c, 0x7E0000u | entry);
    return "";
}

/* System: n instructions; returns the fatal message, or "". */
static const char *system_steps(CPU *c, int n)
{
    volatile int left = n;   /* survives the longjmp */
    fatal_msg[0] = 0;
    if (setjmp(fatal_jmp))
        return fatal_msg;
    while (left-- > 0)
        interp_step(c);
    return "";
}

#define EXPECT_MSG(got, want) \
    CHECK(!strcmp((got), (want)), "got '%s', want '%s'", (got), (want))

static void test_emulation_mode(void)
{
    static const uint8_t code[] = {0x60};                  /* RTS */
    CPU c;
    load(code, sizeof code, 0x2000);
    cpu_at(&c, 0x2000);
    c.e = 1;
    EXPECT_MSG(strict(&c, 0x2000), "interp: emulation mode not supported");
}

static void test_undeclared_jump_table(void)
{
    static const uint8_t code[] = {0x7C, 0x00, 0x30};      /* JMP ($3000,X) */
    static const uint8_t jsrx[] = {0xFC, 0x00, 0x30};      /* JSR ($3000,X) */
    CPU c;
    load(code, sizeof code, 0x2000);
    load(jsrx, sizeof jsrx, 0x2100);
    bus_wram()[0x3000] = 0x10;                             /* -> $2010 */
    bus_wram()[0x3001] = 0x20;

    /* Strict mode too reads a table funcs.toml doesn't list, as the CPU does
       (generated code dispatches it at run time, #92): $2010 is RTS, and so
       is the JSR's continuation at $2103. */
    bus_wram()[0x2010] = 0x60;
    bus_wram()[0x2103] = 0x60;
    cpu_at(&c, 0x2000);
    EXPECT_MSG(strict(&c, 0x2000), "");
    CHECK(c.PC == 0x1234 && c.S == 0x01F0, "strict JMP (abs,X) returned: PC $%04X S $%04X", c.PC,
          c.S);
    cpu_at(&c, 0x2100);
    EXPECT_MSG(strict(&c, 0x2100), "");
    CHECK(c.PC == 0x1234 && c.S == 0x01F0, "strict JSR (abs,X) returned: PC $%04X S $%04X", c.PC,
          c.S);

    cpu_at(&c, 0x2000);
    EXPECT_MSG(system_steps(&c, 1), "");
    CHECK(c.PB == 0x7E && c.PC == 0x2010, "system JMP (abs,X): PC $%02X%04X", c.PB, c.PC);
    cpu_at(&c, 0x2100);
    EXPECT_MSG(system_steps(&c, 1), "");
    CHECK(c.PC == 0x2010 && c.S == 0x01EE, "system JSR (abs,X): PC $%04X S $%04X", c.PC, c.S);
}

static void test_shadow_stack(void)
{
    /* $2000: JSR $2010 / RTS. $2010: PLA PLA / PEA $2005 / RTS, which
       returns to $2006 instead of $2003. */
    static const uint8_t caller[] = {0x20, 0x10, 0x20, 0x60};
    static const uint8_t callee[] = {0x68, 0x68, 0xF4, 0x05, 0x20, 0x60};
    /* $2100: JSR $2110; $2110: RTL.  $2200: JSL $7E2210; $2210: RTS. */
    static const uint8_t jsr_rtl[] = {0x20, 0x10, 0x21};
    static const uint8_t rtl[] = {0x6B};
    static const uint8_t jsl_rts[] = {0x22, 0x10, 0x22, 0x7E};
    static const uint8_t rts[] = {0x60};
    CPU c;
    load(caller, sizeof caller, 0x2000);
    load(callee, sizeof callee, 0x2010);
    load(jsr_rtl, sizeof jsr_rtl, 0x2100);
    load(rtl, sizeof rtl, 0x2110);
    load(jsl_rts, sizeof jsl_rts, 0x2200);
    load(rts, sizeof rts, 0x2210);

    cpu_at(&c, 0x2000);
    EXPECT_MSG(strict(&c, 0x2000), "$7E2000: callee returned to $2006, expected $2003");
    cpu_at(&c, 0x2100);
    EXPECT_MSG(strict(&c, 0x2100), "interp $7E2110: RTL returns from a JSR");
    cpu_at(&c, 0x2200);
    EXPECT_MSG(strict(&c, 0x2200), "interp $7E2210: RTS returns from a JSL");

    cpu_at(&c, 0x2000);
    EXPECT_MSG(system_steps(&c, 5), "");                   /* JSR PLA PLA PEA RTS */
    CHECK(c.PC == 0x2006 && c.S == 0x01F0, "system: returned to $%04X, S $%04X", c.PC, c.S);
}

static void test_shared_fatals(void)
{
    /* Fatal in both modes: unimplemented behavior is never guessed. */
    static const uint8_t brk[] = {0x00, 0x00};
    CPU c;
    load(brk, sizeof brk, 0x2300);

    cpu_at(&c, 0x2300);
    EXPECT_MSG(strict(&c, 0x2300), "$7E2300: BRK executed");
    cpu_at(&c, 0x2300);
    EXPECT_MSG(system_steps(&c, 1), "$7E2300: BRK executed");
    cpu_at(&c, 0x2300);
    c.e = 1;
    EXPECT_MSG(system_steps(&c, 1), "interp $7E2300: opcode $00 in emulation mode not implemented");
}

static void test_decimal_mode(void)
{
    /* SED / LDA #$09 / ADC #$01 / RTS: BCD 09 + 01 = 10, no carry. */
    static const uint8_t adc8_d[] = {0xF8, 0xA9, 0x09, 0x69, 0x01, 0x60};
    /* SED / LDA #$50 / ADC #$50 / RTS: BCD 50 + 50 = 00, carry set. */
    static const uint8_t adc8_c[] = {0xF8, 0xA9, 0x50, 0x69, 0x50, 0x60};
    /* SED / LDA #$10 / SEC / SBC #$01 / RTS: BCD 10 - 01 = 09. */
    static const uint8_t sbc8_d[] = {0xF8, 0xA9, 0x10, 0x38, 0xE9, 0x01, 0x60};
    /* REP #$20 / SED / LDA #$0999 / ADC #$0001 / RTS: BCD 0999 + 1 = 1000. */
    static const uint8_t adc16_d[] = {0xC2, 0x20, 0xF8, 0xA9, 0x99, 0x09,
                                      0x69, 0x01, 0x00, 0x60};
    CPU c;
    load(adc8_d, sizeof adc8_d, 0x2310);
    load(adc8_c, sizeof adc8_c, 0x2320);
    load(sbc8_d, sizeof sbc8_d, 0x2330);
    load(adc16_d, sizeof adc16_d, 0x2340);

    cpu_at(&c, 0x2310);
    EXPECT_MSG(strict(&c, 0x2310), "");
    CHECK((c.A & 0xFF) == 0x10 && c.c == 0, "strict dec adc8: A $%02X c %d", c.A & 0xFF, c.c);
    cpu_at(&c, 0x2320);
    EXPECT_MSG(strict(&c, 0x2320), "");
    CHECK((c.A & 0xFF) == 0x00 && c.c == 1 && c.z == 1, "strict dec adc8 carry: A $%02X c %d",
          c.A & 0xFF, c.c);
    cpu_at(&c, 0x2330);
    EXPECT_MSG(strict(&c, 0x2330), "");
    CHECK((c.A & 0xFF) == 0x09 && c.c == 1, "strict dec sbc8: A $%02X c %d", c.A & 0xFF, c.c);
    cpu_at(&c, 0x2340);
    EXPECT_MSG(strict(&c, 0x2340), "");
    CHECK(c.A == 0x1000 && c.c == 0, "strict dec adc16: A $%04X c %d", c.A, c.c);

    cpu_at(&c, 0x2310);
    EXPECT_MSG(system_steps(&c, 4), "");
    CHECK((c.A & 0xFF) == 0x10 && c.c == 0, "system dec adc8: A $%02X c %d", c.A & 0xFF, c.c);
    cpu_at(&c, 0x2330);
    EXPECT_MSG(system_steps(&c, 5), "");
    CHECK((c.A & 0xFF) == 0x09 && c.c == 1, "system dec sbc8: A $%02X c %d", c.A & 0xFF, c.c);
}

int main(void)
{
    th_bus_init();
    ct_fatal_hook = on_fatal;
    test_emulation_mode();
    test_undeclared_jump_table();
    test_shadow_stack();
    /* Strict after system: the mode switch must not leak into strict. */
    test_undeclared_jump_table();
    test_shared_fatals();
    test_decimal_mode();
    return th_report("interp_modes");
}
