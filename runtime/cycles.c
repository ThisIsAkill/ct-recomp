#include "cycles.h"

#include "bus.h"

int ct_cyc_cross;
int ct_cyc_taken;
long ct_cyc_moved;

/* CPU cycles per opcode for M=1 X=1 DL=0 (65C816 datasheet), plus which
   penalties apply. Approximate, not cycle-exact: no DRAM refresh, and
   every cycle of an instruction runs at the speed of the region it was
   fetched from (see master_per_cycle). */
enum {
    P_M   = 1,      /* +1 if M=0 */
    P_M2  = 2,      /* +2 if M=0 (read-modify-write) */
    P_X   = 4,      /* +1 if X=0 */
    P_DL  = 8,      /* +1 if the low byte of DP is nonzero */
    P_IDX = 16,     /* +1 if the index crossed a page or X=0 (indexed reads) */
    P_BR  = 32,     /* +1 if the branch was taken */
    P_MV  = 64,     /* 7 per byte moved (MVN/MVP), base unused */
    P_RTI = 128,    /* +1 in native mode */
};
static uint8_t cyc_base[256], cyc_pen[256];
static int built;

static void cyc(uint8_t op, uint8_t base, uint8_t pen)
{
    cyc_base[op] = base;
    cyc_pen[op] = pen;
}

static void build_cycle_table(void)
{
    /* ALU group: ORA AND EOR ADC STA LDA CMP SBC by addressing column. */
    static const struct { uint8_t lo, base, pen; } col[] = {
        {0x01, 6, P_DL}, {0x03, 4, 0}, {0x05, 3, P_DL}, {0x07, 6, P_DL}, {0x09, 2, 0},
        {0x0D, 4, 0}, {0x0F, 5, 0}, {0x11, 5, P_DL | P_IDX}, {0x12, 5, P_DL}, {0x13, 7, 0},
        {0x15, 4, P_DL}, {0x17, 6, P_DL}, {0x19, 4, P_IDX}, {0x1D, 4, P_IDX}, {0x1F, 5, 0},
    };
    for (unsigned hi = 0; hi < 8; hi++)
        for (unsigned k = 0; k < sizeof col / sizeof col[0]; k++)
            cyc((uint8_t)(hi << 5 | col[k].lo), col[k].base, (uint8_t)(col[k].pen | P_M));
    cyc(0x91, 6, P_DL | P_M);      /* STA (dp),Y: writes take the extra cycle always */
    cyc(0x99, 5, P_M);             /* STA abs,Y */
    cyc(0x9D, 5, P_M);             /* STA abs,X */

    /* read-modify-write: ASL ROL LSR ROR / INC DEC */
    static const uint8_t rmw_ops[] = {0x00, 0x20, 0x40, 0x60, 0xE0, 0xC0};
    for (unsigned k = 0; k < sizeof rmw_ops; k++) {
        cyc((uint8_t)(rmw_ops[k] | 0x06), 5, P_DL | P_M2);
        cyc((uint8_t)(rmw_ops[k] | 0x0E), 6, P_M2);
        cyc((uint8_t)(rmw_ops[k] | 0x16), 6, P_DL | P_M2);
        cyc((uint8_t)(rmw_ops[k] | 0x1E), 7, P_M2);
    }
    static const uint8_t two[] = {
        0x0A, 0x2A, 0x4A, 0x6A, 0x1A, 0x3A,                         /* accumulator RMW */
        0xE8, 0xC8, 0xCA, 0x88,                                     /* INX INY DEX DEY */
        0xAA, 0xA8, 0x8A, 0x98, 0x9B, 0xBB, 0xBA, 0x9A, 0x5B, 0x7B, 0x1B, 0x3B,
        0x18, 0x38, 0x58, 0x78, 0xD8, 0xF8, 0xB8, 0xFB, 0xEA, 0x42,
    };
    for (unsigned k = 0; k < sizeof two; k++)
        cyc(two[k], 2, 0);
    cyc(0x04, 5, P_DL | P_M2); cyc(0x0C, 6, P_M2);                 /* TSB */
    cyc(0x14, 5, P_DL | P_M2); cyc(0x1C, 6, P_M2);                 /* TRB */
    cyc(0x89, 2, P_M); cyc(0x24, 3, P_DL | P_M); cyc(0x2C, 4, P_M); /* BIT */
    cyc(0x34, 4, P_DL | P_M); cyc(0x3C, 4, P_M | P_IDX);
    cyc(0xA2, 2, P_X); cyc(0xA6, 3, P_DL | P_X); cyc(0xAE, 4, P_X); /* LDX */
    cyc(0xB6, 4, P_DL | P_X); cyc(0xBE, 4, P_X | P_IDX);
    cyc(0xA0, 2, P_X); cyc(0xA4, 3, P_DL | P_X); cyc(0xAC, 4, P_X); /* LDY */
    cyc(0xB4, 4, P_DL | P_X); cyc(0xBC, 4, P_X | P_IDX);
    cyc(0x86, 3, P_DL | P_X); cyc(0x8E, 4, P_X); cyc(0x96, 4, P_DL | P_X); /* STX */
    cyc(0x84, 3, P_DL | P_X); cyc(0x8C, 4, P_X); cyc(0x94, 4, P_DL | P_X); /* STY */
    cyc(0x64, 3, P_DL | P_M); cyc(0x74, 4, P_DL | P_M);             /* STZ */
    cyc(0x9C, 4, P_M); cyc(0x9E, 5, P_M);
    cyc(0xE0, 2, P_X); cyc(0xE4, 3, P_DL | P_X); cyc(0xEC, 4, P_X); /* CPX */
    cyc(0xC0, 2, P_X); cyc(0xC4, 3, P_DL | P_X); cyc(0xCC, 4, P_X); /* CPY */
    cyc(0xEB, 3, 0);                                                /* XBA */
    cyc(0x48, 3, P_M); cyc(0xDA, 3, P_X); cyc(0x5A, 3, P_X);        /* PHA PHX PHY */
    cyc(0x68, 4, P_M); cyc(0xFA, 4, P_X); cyc(0x7A, 4, P_X);        /* PLA PLX PLY */
    cyc(0x8B, 3, 0); cyc(0xAB, 4, 0); cyc(0x0B, 4, 0); cyc(0x2B, 5, 0);
    cyc(0x4B, 3, 0); cyc(0x08, 3, 0); cyc(0x28, 4, 0);
    cyc(0xF4, 5, 0); cyc(0xD4, 6, P_DL); cyc(0x62, 6, 0);           /* PEA PEI PER */
    cyc(0xC2, 3, 0); cyc(0xE2, 3, 0);                               /* REP SEP */
    static const uint8_t br[] = {0x10, 0x30, 0x50, 0x70, 0x90, 0xB0, 0xD0, 0xF0, 0x80};
    for (unsigned k = 0; k < sizeof br; k++)
        cyc(br[k], 2, P_BR);
    cyc(0x82, 4, 0);                                                /* BRL */
    cyc(0x4C, 3, 0); cyc(0x5C, 4, 0); cyc(0x6C, 5, 0); cyc(0x7C, 6, 0); cyc(0xDC, 6, 0);
    cyc(0x20, 6, 0); cyc(0xFC, 8, 0); cyc(0x22, 8, 0);
    cyc(0x60, 6, 0); cyc(0x6B, 6, 0); cyc(0x40, 6, P_RTI);
    cyc(0x54, 0, P_MV); cyc(0x44, 0, P_MV);
    cyc(0xCB, 3, 0); cyc(0xDB, 3, 0);                               /* WAI STP */
}

/* Master clocks per CPU cycle for code fetched from PB:PC: 6 for FastROM
   (banks $80-$FF ROM with MEMSEL bit 0 set), otherwise 8. */
unsigned cyc_master_per_cycle(uint8_t pb, uint16_t pc)
{
    int rom = pb >= 0xC0 || ((pb & 0x7F) < 0x40 && pc >= 0x8000);
    return (pb & 0x80) && rom && bus_fastrom() ? 6 : 8;
}

/* The instruction begun and not yet charged. */
static struct {
    int pending;
    uint32_t at;
    uint8_t op, m16, x16, dl, native;
    unsigned speed;
} cur;

void cyc_begin(const CPU *c, uint32_t at, uint8_t op)
{
    if (!built) {
        build_cycle_table();
        built = 1;
    }
    cur.pending = 1;
    cur.at = at;
    cur.op = op;
    cur.m16 = !c->m;
    cur.x16 = !c->x;
    cur.dl = (c->DP & 0xFF) != 0;
    cur.native = !c->e;
    cur.speed = cyc_master_per_cycle((uint8_t)(at >> 16), (uint16_t)at);
    ct_cyc_cross = ct_cyc_taken = 0;
    ct_cyc_moved = 0;
}

unsigned cyc_finish(void)
{
    if (!cur.pending)
        return 0;
    cur.pending = 0;
    uint8_t pen = cyc_pen[cur.op];
    unsigned n = cyc_base[cur.op];
    if (pen & P_MV)
        n = 7u * (unsigned)ct_cyc_moved;
    if ((pen & P_M) && cur.m16)
        n += 1;
    if ((pen & P_M2) && cur.m16)
        n += 2;
    if ((pen & P_X) && cur.x16)
        n += 1;
    if ((pen & P_DL) && cur.dl)
        n += 1;
    if ((pen & P_IDX) && (ct_cyc_cross || cur.x16))
        n += 1;
    if ((pen & P_BR) && ct_cyc_taken)
        n += 1;
    if ((pen & P_RTI) && cur.native)
        n += 1;
    if (!n)
        ct_fatal("interp $%06X: no cycle count for opcode $%02X", cur.at, cur.op);
    return n * cur.speed;
}
