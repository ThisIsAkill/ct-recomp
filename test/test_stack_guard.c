/* diff_all's stack-clobber guard (harness.h). Programs run from WRAM at
 * $7E2000 as a called routine: entry S=$01F0, the harness return address
 * at $01EF-$01F0, so the live frames start as ($01EE, $01F0]. */
#include <setjmp.h>

#include "harness.h"

static jmp_buf fatal_jmp;
static char fatal_msg[128];

static void on_fatal(const char *msg)
{
    snprintf(fatal_msg, sizeof fatal_msg, "%s", msg);
    longjmp(fatal_jmp, 1);
}

/* Run code at $7E2000; returns the clobbering write count. */
static long guarded(const uint8_t *code, unsigned n)
{
    static CPU c;
    memcpy(bus_wram() + 0x2000, code, n);
    cpu_init(&c);
    c.PB = 0x7E;
    c.DB = 0x7E;
    c.m = 1;
    c.x = 0;
    c.S = 0x01F0;
    push16(&c, 0x1233);
    stack_guard_begin(&c, 0x01F0);
    if (!setjmp(fatal_jmp))
        interp_call(&c, 0x7E2000);
    return stack_guard_end();
}

int main(void)
{
    th_bus_init();
    ct_fatal_hook = on_fatal;

    /* PHA / PHP / JSR $2010 / PLP / PLA / STA $1000 / RTS; $2010: PHA PLA RTS */
    static const uint8_t clean[] = {
        0x48, 0x08, 0x20, 0x10, 0x20, 0x28, 0x68, 0x8D, 0x00, 0x10, 0x60,
        0, 0, 0, 0, 0,
        0x48, 0x68, 0x60};
    CHECK(guarded(clean, sizeof clean) == 0, "pushes, JSR/RTS, and plain stores are not clobbers");

    /* PHP (saves P at $01EE) / STA $01EE / PLP / RTS */
    static const uint8_t own_frame[] = {0x08, 0x8D, 0xEE, 0x01, 0x28, 0x60};
    CHECK(guarded(own_frame, sizeof own_frame) == 1, "store into the routine's own saved P");

    /* STA $01EF: the harness return address */
    static const uint8_t ret_addr[] = {0x8D, 0xEF, 0x01, 0x60};
    CHECK(guarded(ret_addr, sizeof ret_addr) == 1, "store into the return address");

    /* WMADD = $0001EF, then WMDATA (long stores: DB is $7E) */
    static const uint8_t port[] = {
        0xA9, 0xEF, 0x8F, 0x81, 0x21, 0x00, 0xA9, 0x01, 0x8F, 0x82, 0x21, 0x00,
        0xA9, 0x00, 0x8F, 0x83, 0x21, 0x00, 0xA9, 0x55, 0x8F, 0x80, 0x21, 0x00, 0x60};
    CHECK(guarded(port, sizeof port) == 1, "WRAM data port write into the return address");

    /* REP #$20 / LDA #$0001 / LDX #$1000 / LDY #$01EF / MVN $7E,$7E / SEP #$20 / RTS:
       two bytes over the return address */
    static const uint8_t mvn[] = {
        0xC2, 0x20, 0xA9, 0x01, 0x00, 0xA2, 0x00, 0x10, 0xA0, 0xEF, 0x01,
        0x54, 0x7E, 0x7E, 0xE2, 0x20, 0x60};
    CHECK(guarded(mvn, sizeof mvn) == 2, "MVN over the return address");

    /* STA $01EE (at S: free space) / STA $01F5 (above entry S: not a frame) / RTS */
    static const uint8_t outside[] = {0x8D, 0xEE, 0x01, 0x8D, 0xF5, 0x01, 0x60};
    CHECK(guarded(outside, sizeof outside) == 0, "writes at S or above the entry S are not clobbers");

    /* Write budget: an MVN of 4 KB with a cap of 100 WRAM writes stops at
       the 101st write, whatever the length the routine asked for. */
    static const uint8_t big_mvn[] = {
        0xC2, 0x20, 0xA9, 0xFF, 0x0F, 0xA2, 0x00, 0x30, 0xA0, 0x00, 0x40,
        0x54, 0x7E, 0x7E, 0xE2, 0x20, 0x60};
    sg_write_cap = 100;
    fatal_msg[0] = 0;
    guarded(big_mvn, sizeof big_mvn);
    CHECK(!strcmp(fatal_msg, "write budget exhausted after 100 WRAM writes"),
          "write budget: '%s'", fatal_msg);
    CHECK(sg_writes == 101, "stopped at write %ld", sg_writes);
    sg_write_cap = 0;
    fatal_msg[0] = 0;
    guarded(big_mvn, sizeof big_mvn);
    CHECK(!fatal_msg[0] && sg_writes == 0x1000, "no cap: all %ld writes", sg_writes);

    return th_report("stack_guard");
}
