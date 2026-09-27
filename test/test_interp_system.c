/* System mode (runtime/interp.h): the test ROM's reset stub, the emulation-mode
 * allowlist, cycle counts against the 65C816 datasheet, NMI entry through
 * the ROM vector, RTI, and WAI. Test programs run from WRAM ($7E, 8 master
 * clocks per cycle). */
#include <setjmp.h>

#include "harness.h"

static jmp_buf fatal_jmp;
static char fatal_msg[256];

static void on_fatal(const char *msg)
{
    snprintf(fatal_msg, sizeof fatal_msg, "%s", msg);
    longjmp(fatal_jmp, 1);
}

/* One step; returns master clocks, or 0 with fatal_msg set. */
static unsigned step1(CPU *c)
{
    fatal_msg[0] = 0;
    if (setjmp(fatal_jmp))
        return 0;
    return interp_step(c);
}

static void at(CPU *c, uint16_t pc)
{
    cpu_init(c);
    c->PB = 0x7E;
    c->DB = 0x7E;
    c->PC = pc;
    c->S = 0x01F0;
    c->m = 1;
    c->x = 1;
}

static void load(const uint8_t *code, unsigned n, uint16_t pc)
{
    memcpy(bus_wram() + pc, code, n);
}

static void test_reset_stub(void)
{
    CPU c;
    interp_reset(&c);
    CHECK(c.e && c.i && c.PB == 0 && c.PC == 0x8000 && c.S == 0x01FF, "power-on state, PC $%04X",
          c.PC);
    unsigned t[4];
    for (int k = 0; k < 4; k++)
        t[k] = step1(&c);                               /* SEI CLC XCE JML $C08100 */
    CHECK(!fatal_msg[0], "reset stub ran: %s", fatal_msg);
    CHECK(!c.e && c.m && c.x && c.i, "native after XCE, M/X still 1");
    CHECK(c.PB == 0xC0 && c.PC == 0x8100, "JML out of the stub: $%02X%04X", c.PB, c.PC);
    /* Slow ROM fetches 8 clocks, internal cycles 6: SEI/CLC/XCE fetch +
       idle, JML four fetches. */
    CHECK(t[0] == 14 && t[1] == 14 && t[2] == 14 && t[3] == 32,
          "stub cycles %u %u %u %u (8+6 x3, 4x8)", t[0], t[1], t[2], t[3]);

    at(&c, 0x2000);
    c.e = 1;
    static const uint8_t lda_dp[] = {0xA5, 0x10};
    load(lda_dp, sizeof lda_dp, 0x2000);
    CHECK(step1(&c) == 0 &&
          !strcmp(fatal_msg, "interp $7E2000: opcode $A5 in emulation mode not implemented"),
          "emulation mode outside the allowlist: '%s'", fatal_msg);
}

static void test_cycles(void)
{
    CPU c;
    static const uint8_t prog[] = {
        0xA9, 0x12,             /* $2000 LDA #$12        2           */
        0xA5, 0x10,             /* $2002 LDA $10         3 +1 DL     */
        0xBD, 0xFF, 0x12,       /* $2004 LDA $12FF,X     4 +1 cross  */
        0xC2, 0x20,             /* $2007 REP #$20        3           */
        0xA9, 0x34, 0x12,       /* $2009 LDA #$1234      2 +1 M=0    */
        0x9D, 0x00, 0x10,       /* $200C STA $1000,X     5 +1 M=0    */
        0xD0, 0x00,             /* $200F BNE +0 (taken)  2 +1        */
        0xF0, 0x00,             /* $2011 BEQ (not taken) 2           */
        0xA9, 0x01, 0x00,       /* $2013 LDA #$0001      3           */
        0xA2, 0x00,             /* $2016 LDX #$00        2 (X=1)     */
        0xA0, 0x00,             /* $2018 LDY #$00        2           */
        0x54, 0x7E, 0x7E,       /* $201A MVN $7E,$7E     7 x 2 bytes */
    };
    /* Master clocks, each cycle at its own speed (bsnes CPU::wait): code
       and data in WRAM 8, internal cycles 6. LDA $10: 2 fetches + DL idle
       + read = 16+6+8; LDA abs,X cross: 24+6+8; REP: 16+6; STA abs,X M=0:
       24+6+2x8; branch taken 16+6; MVN per byte: 3 fetches, read, write,
       2 idle = 52. */
    static const unsigned want[] = {16, 30, 38, 22, 24, 46, 22, 16, 24, 16, 16, 104};
    load(prog, sizeof prog, 0x2000);
    at(&c, 0x2000);
    c.DP = 0x0001;
    c.X = 1;
    for (unsigned k = 0; k < sizeof want / sizeof want[0]; k++) {
        unsigned got = step1(&c);
        CHECK(got == want[k], "step %u at $%04X: %u master clocks, want %u", k, c.PC, got,
              want[k]);
    }

    /* FastROM: SEI at $C08000 (the test ROM's reset stub) */
    at(&c, 0x8000);
    c.PB = 0xC0;
    CHECK(step1(&c) == 14, "bank $C0, MEMSEL=0: fetch 8 + idle 6");
    write8(0x00420D, 1);
    at(&c, 0x8000);
    c.PB = 0xC0;
    CHECK(step1(&c) == 12, "bank $C0, MEMSEL=1: fetch 6 + idle 6");
    at(&c, 0x8000);
    c.PB = 0x00;
    CHECK(step1(&c) == 14, "bank $00 stays slow with MEMSEL=1");
    write8(0x00420D, 0);
}

static void test_nmi_rti_wai(void)
{
    CPU c;
    static const uint8_t wai[] = {0xCB, 0xEA};          /* $2000 WAI / NOP */
    static const uint8_t rti[] = {0x40};                /* $000500: RAM NMI handler */
    load(wai, sizeof wai, 0x2000);
    load(rti, sizeof rti, 0x0500);
    at(&c, 0x2000);
    c.c = 1;
    c.i = 0;
    CHECK(step1(&c) == 20 && interp_waiting(), "WAI: fetch + 2 idle, then waiting");
    CHECK(step1(&c) == 0 &&
          !strcmp(fatal_msg, "interp: step while waiting for an interrupt (WAI)"),
          "stepping while waiting is fatal: '%s'", fatal_msg);

    /* Entry: fetch at PB:PC (WRAM 8), idle 6, 4 stack writes (WRAM 8),
       2 vector reads (bank 0 ROM 8). */
    CHECK(interp_interrupt(&c, 1) == 62 && !interp_waiting(), "NMI entry ends the wait");
    CHECK(c.PB == 0 && c.PC == 0x8010 && c.i && !c.d && c.S == 0x01EC,
          "NMI through $FFEA: $%02X%04X S $%04X", c.PB, c.PC, c.S);
    CHECK(step1(&c) == 32 && c.PB == 0 && c.PC == 0x0500, "ROM stub JML $000500");
    CHECK(step1(&c) == 52, "RTI native: fetch 8, 2 idle, 4 pulls (WRAM 8)");
    CHECK(c.PB == 0x7E && c.PC == 0x2001 && c.S == 0x01F0 && c.c && !c.i,
          "RTI back after the WAI: $%02X%04X S $%04X", c.PB, c.PC, c.S);
}

/* Write-only CPU registers read back the last data bus value: for an
   absolute load that is the address high byte, as on hardware (the mode-7
   engine's RAM code does LDX $4202). */
static void test_open_bus(void)
{
    CPU c;
    static const uint8_t ldx[] = {0xAE, 0x02, 0x42};    /* LDX $4202 (X=0: $4202/$4203) */
    load(ldx, sizeof ldx, 0x2400);
    at(&c, 0x2400);
    c.x = 0;
    c.DB = 0x00;
    step1(&c);
    CHECK(!fatal_msg[0] && c.X == 0x4242, "LDX $4202: X $%04X, want $4242 (%s)", c.X, fatal_msg);
}

int main(void)
{
    th_bus_init();
    ct_fatal_hook = on_fatal;
    test_open_bus();
    test_reset_stub();
    test_cycles();
    test_nmi_rti_wai();
    return th_report("interp_system");
}
