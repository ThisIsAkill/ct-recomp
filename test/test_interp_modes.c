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

    cpu_at(&c, 0x2000);
    EXPECT_MSG(strict(&c, 0x2000), "interp $7E2000: no jumptable entry in funcs.toml");
    cpu_at(&c, 0x2100);
    EXPECT_MSG(strict(&c, 0x2100), "interp $7E2100: no jumptable entry in funcs.toml");

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
    static const uint8_t adc_d[] = {0xF8, 0x69, 0x01};     /* SED / ADC #$01 */
    CPU c;
    load(brk, sizeof brk, 0x2300);
    load(adc_d, sizeof adc_d, 0x2310);

    cpu_at(&c, 0x2300);
    EXPECT_MSG(strict(&c, 0x2300), "$7E2300: BRK executed");
    cpu_at(&c, 0x2300);
    EXPECT_MSG(system_steps(&c, 1), "$7E2300: BRK executed");
    cpu_at(&c, 0x2310);
    EXPECT_MSG(strict(&c, 0x2310), "interp $7E2311: decimal mode");
    cpu_at(&c, 0x2310);
    EXPECT_MSG(system_steps(&c, 2), "interp $7E2311: decimal mode");
    cpu_at(&c, 0x2300);
    c.e = 1;
    EXPECT_MSG(system_steps(&c, 1), "interp $7E2300: opcode $00 in emulation mode not implemented");
}

int main(void)
{
    bus_init(NULL);
    ct_fatal_hook = on_fatal;
    test_emulation_mode();
    test_undeclared_jump_table();
    test_shadow_stack();
    /* Strict after system: the mode switch must not leak into strict. */
    test_undeclared_jump_table();
    test_shared_fatals();
    return th_report("interp_modes");
}
