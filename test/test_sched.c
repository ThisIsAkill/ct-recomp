/* Frame scheduler (runtime/sched.h) end to end on a WRAM program: VBlank
 * NMI through the ROM vector into a RAM handler, WAI between frames, and a
 * per-line HDMA gradient rendered by the vendored PPU. */
#include "harness.h"
#include "sched.h"

static void put(uint16_t at, const uint8_t *p, unsigned n)
{
    memcpy(bus_wram() + at, p, n);
}

int main(void)
{
    bus_init(NULL);

    /* HDMA channel 7, mode 3 (4 bytes to $2121 $2121 $2122 $2122): every
       line sets CGRAM[0] (the backdrop) to red = row & 31. */
    uint8_t *w = bus_wram();
    for (int r = 0; r < SCHED_HEIGHT; r++) {
        uint8_t *e = w + 0x3000 + r * 5;
        e[0] = 1;
        e[1] = e[2] = 0;
        e[3] = (uint8_t)(r & 31);
        e[4] = 0;
    }
    w[0x3000 + SCHED_HEIGHT * 5] = 0;

    static const uint8_t prog[] = {
        0xA9, 0x0F, 0x8D, 0x00, 0x21,   /* LDA #$0F / STA $2100  INIDISP */
        0x9C, 0x2C, 0x21,               /* STZ $212C             TM: backdrop only */
        0xA9, 0x03, 0x8D, 0x70, 0x43,   /* DMAP7 = 3 */
        0xA9, 0x21, 0x8D, 0x71, 0x43,   /* BBAD7 = $21 */
        0xA9, 0x00, 0x8D, 0x72, 0x43,   /* A1T7 = $7E3000 */
        0xA9, 0x30, 0x8D, 0x73, 0x43,
        0xA9, 0x7E, 0x8D, 0x74, 0x43,
        0xA9, 0x80, 0x8D, 0x0C, 0x42,   /* HDMAEN = ch 7 */
        0xA9, 0x80, 0x8D, 0x00, 0x42,   /* NMITIMEN: NMI on */
        0xCB, 0x80, 0xFD,               /* loop: WAI / BRA loop */
    };
    static const uint8_t nmi[] = {
        0xE6, 0x10,                     /* INC $10 */
        0xAD, 0x10, 0x42,               /* LDA $4210 (ack) */
        0x40,                           /* RTI */
    };
    put(0x2000, prog, sizeof prog);
    put(0x0500, nmi, sizeof nmi);       /* ROM NMI stub: JML $000500 */
    w[0x10] = 0;

    static CPU c;
    cpu_init(&c);
    c.PB = 0x7E;
    c.PC = 0x2000;
    c.DB = 0x00;
    c.m = c.x = 1;
    c.i = 1;
    c.S = 0x01FF;
    sched_init(&c);
    for (int f = 0; f < 3; f++)
        sched_run_frame();

    CHECK(sched_frame_count() == 3, "frames %ld", sched_frame_count());
    CHECK(sched_nmi_count() == 3, "one NMI per frame: %ld", sched_nmi_count());
    CHECK(w[0x10] == 3, "RAM handler ran 3 times: %u", w[0x10]);
    CHECK(c.PB == 0x7E && c.PC == 0x202C, "waiting after the WAI at $202B: $%02X%04X", c.PB,
          c.PC);

    const uint8_t *fb = sched_frame();
    int bad = 0;
    for (int r = 0; r < SCHED_HEIGHT && bad < 5; r++) {
        int v = r & 31, want = (v << 3) | (v >> 2);
        const uint8_t *px = fb + (size_t)r * SCHED_WIDTH * 4 + 100 * 4;   /* B G R x */
        if (px[2] != want || px[1] || px[0]) {
            CHECK(0, "row %d: RGB %u %u %u, want %d 0 0", r, px[2], px[1], px[0], want);
            bad++;
        }
    }
    CHECK(!bad, "HDMA gradient: row r red = r & 31");
    return th_report("sched");
}
