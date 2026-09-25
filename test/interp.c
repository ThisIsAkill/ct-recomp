#include "interp.h"

#include "bus.h"
#include "ct_funcs.h"

typedef struct {
    uint32_t a;
    int bank0;      /* 16-bit access wraps within bank 0 */
} EA;

static uint8_t fetch8(CPU *c)
{
    uint8_t v = read8((uint32_t)c->PB << 16 | c->PC);
    c->PC = (uint16_t)(c->PC + 1);
    return v;
}

static uint16_t fetch16(CPU *c)
{
    uint8_t lo = fetch8(c);
    return (uint16_t)(lo | fetch8(c) << 8);
}

static uint32_t fetch24(CPU *c)
{
    uint16_t lo = fetch16(c);
    return lo | (uint32_t)fetch8(c) << 16;
}

static uint32_t next_addr(EA e)
{
    return e.bank0 ? (uint16_t)(e.a + 1) : (e.a + 1) & 0xFFFFFF;
}

static uint16_t rd(EA e, int wide)
{
    uint16_t v = read8(e.a);
    if (wide)
        v |= (uint16_t)(read8(next_addr(e)) << 8);
    return v;
}

static void wr(EA e, int wide, uint16_t v)
{
    write8(e.a, (uint8_t)v);
    if (wide)
        write8(next_addr(e), (uint8_t)(v >> 8));
}

static uint16_t rd_bank0_16(uint16_t a)
{
    return (uint16_t)(read8(a) | read8((uint16_t)(a + 1)) << 8);
}

static uint32_t rd_bank0_24(uint16_t a)
{
    return rd_bank0_16(a) | (uint32_t)read8((uint16_t)(a + 2)) << 16;
}

/* Shadow call stack: generated code requires every JSR'd callee to return
   to its call site + 3, so the interpreter enforces the same. */
static struct {
    uint32_t site;
    uint32_t ret;   /* PB:PC expected after return */
    int is_long;
} shadow[256];
static int depth;

/* Extern hook for a call target, or NULL. */
static void (*extern_hook(uint32_t addr, int kind))(CPU *)
{
    for (unsigned k = 0; k < ct_extern_count; k++)
        if (ct_externs[k].addr == addr) {
            if (ct_externs[k].kind != kind)
                ct_fatal("interp: extern $%06X called with wrong return kind", addr);
            return ct_externs[k].hook;
        }
    return 0;
}

/* Jump-table bounds from funcs.toml; same failure as generated code. */
static void table_index(const CPU *c, uint32_t at)
{
    for (unsigned k = 0; k < ct_jumptable_count; k++)
        if (ct_jumptables[k].site == at) {
            if ((c->X & 1) || c->X >= 2u * ct_jumptables[k].count)
                ct_fatal("$%06X: jump table index $%04X out of range", at, c->X);
            return;
        }
    ct_fatal("interp $%06X: no jumptable entry in funcs.toml", at);
}

/* ---- flags / stack ---- */

static void nz(CPU *c, uint16_t v, int wide)
{
    if (wide) {
        c->n = v >> 15;
        c->z = v == 0;
    } else {
        c->n = (v >> 7) & 1;
        c->z = (v & 0xFF) == 0;
    }
}

static uint8_t pack_p(const CPU *c)
{
    return (uint8_t)(c->n << 7 | c->v << 6 | c->m << 5 | c->x << 4 | c->d << 3 |
                     c->i << 2 | c->z << 1 | c->c);
}

static void unpack_p(CPU *c, uint8_t p)
{
    c->n = p >> 7 & 1;
    c->v = p >> 6 & 1;
    c->m = p >> 5 & 1;
    c->x = p >> 4 & 1;
    c->d = p >> 3 & 1;
    c->i = p >> 2 & 1;
    c->z = p >> 1 & 1;
    c->c = p & 1;
    if (c->e)
        c->m = c->x = 1;
    if (c->x) {
        c->X &= 0xFF;
        c->Y &= 0xFF;
    }
}

static void push(CPU *c, uint8_t v)
{
    write8(c->S, v);
    c->S = (uint16_t)(c->S - 1);
    if (c->e)
        c->S = (uint16_t)(0x0100 | (c->S & 0xFF));
}

static uint8_t pull(CPU *c)
{
    c->S = (uint16_t)(c->S + 1);
    if (c->e)
        c->S = (uint16_t)(0x0100 | (c->S & 0xFF));
    return read8(c->S);
}

static void pushw(CPU *c, uint16_t v)
{
    push(c, (uint8_t)(v >> 8));
    push(c, (uint8_t)v);
}

static uint16_t pullw(CPU *c)
{
    uint8_t lo = pull(c);
    return (uint16_t)(lo | pull(c) << 8);
}

/* ---- addressing ---- */

enum mode {
    M_IMM, M_DP, M_DPX, M_DPY, M_DPI, M_DPXI, M_DPIY, M_DPIL, M_DPILY,
    M_SR, M_SRIY, M_ABS, M_ABSX, M_ABSY, M_LONG, M_LONGX
};

static EA bank0(uint32_t a) { EA e = {a & 0xFFFF, 1}; return e; }
static EA full(uint32_t a)  { EA e = {a & 0xFFFFFF, 0}; return e; }

static EA ea(CPU *c, enum mode md)
{
    uint32_t db = (uint32_t)c->DB << 16;
    switch (md) {
    case M_DP:    return bank0(c->DP + fetch8(c));
    case M_DPX:   return bank0(c->DP + fetch8(c) + c->X);
    case M_DPY:   return bank0(c->DP + fetch8(c) + c->Y);
    case M_DPI:   return full(db | rd_bank0_16((uint16_t)(c->DP + fetch8(c))));
    case M_DPXI:  return full(db | rd_bank0_16((uint16_t)(c->DP + fetch8(c) + c->X)));
    case M_DPIY:  return full((db | rd_bank0_16((uint16_t)(c->DP + fetch8(c)))) + c->Y);
    case M_DPIL:  return full(rd_bank0_24((uint16_t)(c->DP + fetch8(c))));
    case M_DPILY: return full(rd_bank0_24((uint16_t)(c->DP + fetch8(c))) + c->Y);
    case M_SR:    return bank0(c->S + fetch8(c));
    case M_SRIY:  return full((db | rd_bank0_16((uint16_t)(c->S + fetch8(c)))) + c->Y);
    case M_ABS:   return full(db | fetch16(c));
    case M_ABSX:  return full((db | fetch16(c)) + c->X);
    case M_ABSY:  return full((db | fetch16(c)) + c->Y);
    case M_LONG:  return full(fetch24(c));
    case M_LONGX: return full(fetch24(c) + c->X);
    default: ct_fatal("interp: bad mode %d", md);
    }
}

/* Operand value: immediate or memory. */
static uint16_t operand(CPU *c, enum mode md, int wide)
{
    if (md == M_IMM)
        return wide ? fetch16(c) : fetch8(c);
    return rd(ea(c, md), wide);
}

/* ---- accumulator ---- */

static uint16_t get_a(const CPU *c) { return c->m ? (c->A & 0xFF) : c->A; }

static void set_a(CPU *c, uint16_t v)
{
    if (c->m)
        c->A = (uint16_t)((c->A & 0xFF00) | (v & 0xFF));
    else
        c->A = v;
}

static uint16_t add(CPU *c, uint16_t a, uint16_t b, int wide, uint32_t at)
{
    if (c->d)
        ct_fatal("interp $%06X: decimal mode", at);
    uint32_t mask = wide ? 0xFFFF : 0xFF, sign = wide ? 0x8000 : 0x80;
    uint32_t r = (uint32_t)a + b + c->c;
    c->c = r > mask;
    c->v = ((a ^ r) & (b ^ r) & sign) != 0;
    r &= mask;
    nz(c, (uint16_t)r, wide);
    return (uint16_t)r;
}

static void compare(CPU *c, uint16_t r, uint16_t v, int wide)
{
    uint16_t mask = wide ? 0xFFFF : 0xFF;
    r &= mask;
    v &= mask;
    c->c = r >= v;
    nz(c, (uint16_t)(r - v), wide);
}

/* ---- read-modify-write ---- */

enum rmw { R_ASL, R_ROL, R_LSR, R_ROR, R_INC, R_DEC };

static uint16_t rmw(CPU *c, enum rmw op, uint16_t v, int wide)
{
    uint16_t mask = wide ? 0xFFFF : 0xFF, top = wide ? 0x8000 : 0x80;
    uint16_t r;
    switch (op) {
    case R_ASL: c->c = (v & top) != 0; r = (uint16_t)(v << 1); break;
    case R_ROL: r = (uint16_t)(v << 1 | c->c); c->c = (v & top) != 0; break;
    case R_LSR: c->c = v & 1; r = v >> 1; break;
    case R_ROR: r = (uint16_t)(v >> 1 | (c->c ? top : 0)); c->c = v & 1; break;
    case R_INC: r = (uint16_t)(v + 1); break;
    default:    r = (uint16_t)(v - 1); break;
    }
    r &= mask;
    nz(c, r, wide);
    return r;
}

static void rmw_mem(CPU *c, enum rmw op, enum mode md)
{
    int wide = !c->m;
    EA e = ea(c, md);
    wr(e, wide, rmw(c, op, rd(e, wide), wide));
}

/* Step budget, charged the same way as generated code's ct_loop (runtime/
   ops.h): once per backward branch/jump actually taken, not per fetched
   instruction. A flat per-instruction count would exhaust far sooner than
   generated code's per-backward-edge count for any loop with more than one
   instruction in its body, making a merely slow (not broken) loop diverge
   only on the interpreter side. */
static long interp_budget;

static void loop_tick(uint32_t at, uint16_t new_pc)
{
    if (new_pc > (uint16_t)at)
        return;
    if (interp_budget && --interp_budget == 0)
        ct_fatal("$%06X: step budget exhausted", at);
}

static void branch(CPU *c, uint32_t at, int cond)
{
    int8_t off = (int8_t)fetch8(c);
    if (cond) {
        c->PC = (uint16_t)(c->PC + off);
        loop_tick(at, c->PC);
    }
}

static void block_move(CPU *c, int step)
{
    uint8_t dst = fetch8(c), src = fetch8(c);
    c->DB = dst;
    do {
        write8((uint32_t)dst << 16 | c->Y, read8((uint32_t)src << 16 | c->X));
        c->X = (uint16_t)(c->X + step);
        c->Y = (uint16_t)(c->Y + step);
        if (c->x) {
            c->X &= 0xFF;
            c->Y &= 0xFF;
        }
        c->A = (uint16_t)(c->A - 1);
    } while (c->A != 0xFFFF);
}

/* ALU group: opcodes aaa.bbbbb with a 65816 addressing column. */
static int alu_mode(uint8_t op, enum mode *md)
{
    switch (op & 0x1F) {
    case 0x01: *md = M_DPXI; return 1;
    case 0x03: *md = M_SR; return 1;
    case 0x05: *md = M_DP; return 1;
    case 0x07: *md = M_DPIL; return 1;
    case 0x09: *md = M_IMM; return op != 0x89;
    case 0x0D: *md = M_ABS; return 1;
    case 0x0F: *md = M_LONG; return 1;
    case 0x11: *md = M_DPIY; return 1;
    case 0x12: *md = M_DPI; return 1;
    case 0x13: *md = M_SRIY; return 1;
    case 0x15: *md = M_DPX; return 1;
    case 0x17: *md = M_DPILY; return 1;
    case 0x19: *md = M_ABSY; return 1;
    case 0x1D: *md = M_ABSX; return 1;
    case 0x1F: *md = M_LONGX; return 1;
    default: return 0;
    }
}

static void alu(CPU *c, uint8_t op, enum mode md, uint32_t at)
{
    int wide = !c->m;
    if (op >> 5 == 4) {                           /* STA */
        if (md == M_IMM)
            ct_fatal("interp $%06X: STA #", at);
        wr(ea(c, md), wide, get_a(c));
        return;
    }
    uint16_t v = operand(c, md, wide), a = get_a(c);
    switch (op >> 5) {
    case 0: a |= v; nz(c, a, wide); set_a(c, a); break;                     /* ORA */
    case 1: a &= v; nz(c, a, wide); set_a(c, a); break;                     /* AND */
    case 2: a ^= v; nz(c, a, wide); set_a(c, a); break;                     /* EOR */
    case 3: set_a(c, add(c, a, v, wide, at)); break;                        /* ADC */
    case 5: nz(c, v, wide); set_a(c, v); break;                             /* LDA */
    case 6: compare(c, a, v, wide); break;                                  /* CMP */
    case 7: set_a(c, add(c, a, (uint16_t)~v & (wide ? 0xFFFF : 0xFF), wide, at)); break; /* SBC */
    }
}

static void bit(CPU *c, enum mode md)
{
    int wide = !c->m;
    uint16_t v = operand(c, md, wide);
    c->z = (get_a(c) & v) == 0;
    if (md != M_IMM) {
        c->n = (v >> (wide ? 15 : 7)) & 1;
        c->v = (v >> (wide ? 14 : 6)) & 1;
    }
}

static uint16_t idx(const CPU *c, uint16_t v) { return c->x ? (v & 0xFF) : v; }

static void load_index(CPU *c, uint16_t *r, enum mode md)
{
    *r = operand(c, md, !c->x);
    nz(c, *r, !c->x);
}

static void step(CPU *c, uint32_t at, uint8_t op)
{
    enum mode md;
    int m16 = !c->m, x16 = !c->x;

    if (((op & 3) == 1 || (op & 3) == 3 || (op & 0x1F) == 0x12) && alu_mode(op, &md)) {
        alu(c, op, md, at);
        return;
    }

    switch (op) {
    /* read-modify-write */
    case 0x0A: set_a(c, rmw(c, R_ASL, get_a(c), m16)); break;
    case 0x2A: set_a(c, rmw(c, R_ROL, get_a(c), m16)); break;
    case 0x4A: set_a(c, rmw(c, R_LSR, get_a(c), m16)); break;
    case 0x6A: set_a(c, rmw(c, R_ROR, get_a(c), m16)); break;
    case 0x1A: set_a(c, rmw(c, R_INC, get_a(c), m16)); break;
    case 0x3A: set_a(c, rmw(c, R_DEC, get_a(c), m16)); break;
    case 0x06: rmw_mem(c, R_ASL, M_DP); break;
    case 0x0E: rmw_mem(c, R_ASL, M_ABS); break;
    case 0x16: rmw_mem(c, R_ASL, M_DPX); break;
    case 0x1E: rmw_mem(c, R_ASL, M_ABSX); break;
    case 0x26: rmw_mem(c, R_ROL, M_DP); break;
    case 0x2E: rmw_mem(c, R_ROL, M_ABS); break;
    case 0x36: rmw_mem(c, R_ROL, M_DPX); break;
    case 0x3E: rmw_mem(c, R_ROL, M_ABSX); break;
    case 0x46: rmw_mem(c, R_LSR, M_DP); break;
    case 0x4E: rmw_mem(c, R_LSR, M_ABS); break;
    case 0x56: rmw_mem(c, R_LSR, M_DPX); break;
    case 0x5E: rmw_mem(c, R_LSR, M_ABSX); break;
    case 0x66: rmw_mem(c, R_ROR, M_DP); break;
    case 0x6E: rmw_mem(c, R_ROR, M_ABS); break;
    case 0x76: rmw_mem(c, R_ROR, M_DPX); break;
    case 0x7E: rmw_mem(c, R_ROR, M_ABSX); break;
    case 0xE6: rmw_mem(c, R_INC, M_DP); break;
    case 0xEE: rmw_mem(c, R_INC, M_ABS); break;
    case 0xF6: rmw_mem(c, R_INC, M_DPX); break;
    case 0xFE: rmw_mem(c, R_INC, M_ABSX); break;
    case 0xC6: rmw_mem(c, R_DEC, M_DP); break;
    case 0xCE: rmw_mem(c, R_DEC, M_ABS); break;
    case 0xD6: rmw_mem(c, R_DEC, M_DPX); break;
    case 0xDE: rmw_mem(c, R_DEC, M_ABSX); break;

    /* TSB / TRB */
    case 0x04: case 0x0C: case 0x14: case 0x1C: {
        EA e = ea(c, (op & 0x08) ? M_ABS : M_DP);
        uint16_t v = rd(e, m16), a = get_a(c);
        c->z = (a & v) == 0;
        wr(e, m16, (op & 0x10) ? (uint16_t)(v & ~a) : (uint16_t)(v | a));
        break;
    }

    /* BIT */
    case 0x89: bit(c, M_IMM); break;
    case 0x24: bit(c, M_DP); break;
    case 0x2C: bit(c, M_ABS); break;
    case 0x34: bit(c, M_DPX); break;
    case 0x3C: bit(c, M_ABSX); break;

    /* index loads / stores / compares */
    case 0xA2: load_index(c, &c->X, M_IMM); break;
    case 0xA6: load_index(c, &c->X, M_DP); break;
    case 0xAE: load_index(c, &c->X, M_ABS); break;
    case 0xB6: load_index(c, &c->X, M_DPY); break;
    case 0xBE: load_index(c, &c->X, M_ABSY); break;
    case 0xA0: load_index(c, &c->Y, M_IMM); break;
    case 0xA4: load_index(c, &c->Y, M_DP); break;
    case 0xAC: load_index(c, &c->Y, M_ABS); break;
    case 0xB4: load_index(c, &c->Y, M_DPX); break;
    case 0xBC: load_index(c, &c->Y, M_ABSX); break;
    case 0x86: wr(ea(c, M_DP), x16, c->X); break;
    case 0x8E: wr(ea(c, M_ABS), x16, c->X); break;
    case 0x96: wr(ea(c, M_DPY), x16, c->X); break;
    case 0x84: wr(ea(c, M_DP), x16, c->Y); break;
    case 0x8C: wr(ea(c, M_ABS), x16, c->Y); break;
    case 0x94: wr(ea(c, M_DPX), x16, c->Y); break;
    case 0x64: wr(ea(c, M_DP), m16, 0); break;
    case 0x74: wr(ea(c, M_DPX), m16, 0); break;
    case 0x9C: wr(ea(c, M_ABS), m16, 0); break;
    case 0x9E: wr(ea(c, M_ABSX), m16, 0); break;
    case 0xE0: compare(c, c->X, operand(c, M_IMM, x16), x16); break;
    case 0xE4: compare(c, c->X, operand(c, M_DP, x16), x16); break;
    case 0xEC: compare(c, c->X, operand(c, M_ABS, x16), x16); break;
    case 0xC0: compare(c, c->Y, operand(c, M_IMM, x16), x16); break;
    case 0xC4: compare(c, c->Y, operand(c, M_DP, x16), x16); break;
    case 0xCC: compare(c, c->Y, operand(c, M_ABS, x16), x16); break;
    case 0xE8: c->X = idx(c, (uint16_t)(c->X + 1)); nz(c, c->X, x16); break;
    case 0xC8: c->Y = idx(c, (uint16_t)(c->Y + 1)); nz(c, c->Y, x16); break;
    case 0xCA: c->X = idx(c, (uint16_t)(c->X - 1)); nz(c, c->X, x16); break;
    case 0x88: c->Y = idx(c, (uint16_t)(c->Y - 1)); nz(c, c->Y, x16); break;

    /* transfers */
    case 0xAA: c->X = idx(c, c->A); nz(c, c->X, x16); break;
    case 0xA8: c->Y = idx(c, c->A); nz(c, c->Y, x16); break;
    case 0x8A: set_a(c, c->X); nz(c, get_a(c), m16); break;
    case 0x98: set_a(c, c->Y); nz(c, get_a(c), m16); break;
    case 0x9B: c->Y = c->X; nz(c, c->Y, x16); break;
    case 0xBB: c->X = c->Y; nz(c, c->X, x16); break;
    case 0xBA: c->X = idx(c, c->S); nz(c, c->X, x16); break;
    case 0x9A: c->S = c->e ? (uint16_t)(0x0100 | (c->X & 0xFF)) : c->X; break;
    case 0x5B: c->DP = c->A; nz(c, c->DP, 1); break;
    case 0x7B: c->A = c->DP; nz(c, c->A, 1); break;
    case 0x1B: c->S = c->e ? (uint16_t)(0x0100 | (c->A & 0xFF)) : c->A; break;
    case 0x3B: c->A = c->S; nz(c, c->A, 1); break;
    case 0xEB: c->A = (uint16_t)(c->A >> 8 | c->A << 8); nz(c, c->A & 0xFF, 0); break;

    /* stack */
    case 0x48: if (m16) pushw(c, c->A); else push(c, (uint8_t)c->A); break;
    case 0xDA: if (x16) pushw(c, c->X); else push(c, (uint8_t)c->X); break;
    case 0x5A: if (x16) pushw(c, c->Y); else push(c, (uint8_t)c->Y); break;
    case 0x68: set_a(c, m16 ? pullw(c) : pull(c)); nz(c, get_a(c), m16); break;
    case 0xFA: c->X = x16 ? pullw(c) : pull(c); nz(c, c->X, x16); break;
    case 0x7A: c->Y = x16 ? pullw(c) : pull(c); nz(c, c->Y, x16); break;
    case 0x8B: push(c, c->DB); break;
    case 0xAB: c->DB = pull(c); nz(c, c->DB, 0); break;
    case 0x0B: pushw(c, c->DP); break;
    case 0x2B: c->DP = pullw(c); nz(c, c->DP, 1); break;
    case 0x4B: push(c, c->PB); break;
    case 0x08: push(c, pack_p(c)); break;
    case 0x28: unpack_p(c, pull(c)); break;
    case 0xF4: pushw(c, fetch16(c)); break;
    case 0xD4: pushw(c, rd_bank0_16((uint16_t)(c->DP + fetch8(c)))); break;
    case 0x62: { uint16_t off = fetch16(c); pushw(c, (uint16_t)(c->PC + off)); break; }

    /* flags */
    case 0x18: c->c = 0; break;
    case 0x38: c->c = 1; break;
    case 0x58: c->i = 0; break;
    case 0x78: c->i = 1; break;
    case 0xD8: c->d = 0; break;
    case 0xF8: c->d = 1; break;
    case 0xB8: c->v = 0; break;
    case 0xC2: unpack_p(c, pack_p(c) & (uint8_t)~fetch8(c)); break;
    case 0xE2: unpack_p(c, pack_p(c) | fetch8(c)); break;
    case 0xFB: {
        uint8_t t = c->c;
        c->c = c->e;
        c->e = t;
        if (c->e) {
            c->m = c->x = 1;
            c->X &= 0xFF;
            c->Y &= 0xFF;
            c->S = (uint16_t)(0x0100 | (c->S & 0xFF));
        }
        break;
    }

    /* branches */
    case 0x10: branch(c, at, !c->n); break;
    case 0x30: branch(c, at, c->n); break;
    case 0x50: branch(c, at, !c->v); break;
    case 0x70: branch(c, at, c->v); break;
    case 0x90: branch(c, at, !c->c); break;
    case 0xB0: branch(c, at, c->c); break;
    case 0xD0: branch(c, at, !c->z); break;
    case 0xF0: branch(c, at, c->z); break;
    case 0x80: branch(c, at, 1); break;
    case 0x82: {
        uint16_t off = fetch16(c);
        c->PC = (uint16_t)(c->PC + off);
        loop_tick(at, c->PC);
        break;
    }

    /* jumps and calls */
    case 0x4C: c->PC = fetch16(c); loop_tick(at, c->PC); break;
    case 0x5C: {
        uint32_t t = fetch24(c);
        c->PB = (uint8_t)(t >> 16);
        c->PC = (uint16_t)t;
        void (*hook)(CPU *) = extern_hook(t, 2);
        if (hook)
            hook(c);
        break;
    }
    case 0x6C: c->PC = rd_bank0_16(fetch16(c)); break;
    case 0x7C: {
        table_index(c, at);
        uint16_t p = (uint16_t)(fetch16(c) + c->X);
        c->PC = (uint16_t)(read8((uint32_t)c->PB << 16 | p) |
                           read8((uint32_t)c->PB << 16 | (uint16_t)(p + 1)) << 8);
        break;
    }
    case 0xDC: { uint32_t t = rd_bank0_24(fetch16(c)); c->PB = (uint8_t)(t >> 16); c->PC = (uint16_t)t; break; }
    case 0x20: {
        uint16_t t = fetch16(c);
        pushw(c, (uint16_t)(c->PC - 1));
        void (*hook)(CPU *) = extern_hook((uint32_t)c->PB << 16 | t, 0);
        if (hook) {
            hook(c);
            break;
        }
        if (depth == 256)
            ct_fatal("interp $%06X: call depth", at);
        shadow[depth].site = at;
        shadow[depth].is_long = 0;
        shadow[depth++].ret = (uint32_t)c->PB << 16 | c->PC;
        c->PC = t;
        break;
    }
    case 0xFC: {
        uint16_t p = (uint16_t)(fetch16(c) + c->X);
        pushw(c, (uint16_t)(c->PC - 1));
        table_index(c, at);
        if (depth == 256)
            ct_fatal("interp $%06X: call depth", at);
        shadow[depth].site = at;
        shadow[depth].is_long = 0;
        shadow[depth++].ret = (uint32_t)c->PB << 16 | c->PC;
        c->PC = (uint16_t)(read8((uint32_t)c->PB << 16 | p) |
                           read8((uint32_t)c->PB << 16 | (uint16_t)(p + 1)) << 8);
        break;
    }
    case 0x22: {
        uint32_t t = fetch24(c);
        push(c, c->PB);
        pushw(c, (uint16_t)(c->PC - 1));
        void (*hook)(CPU *) = extern_hook(t, 1);
        if (hook) {
            c->PB = (uint8_t)(t >> 16);
            hook(c);
            break;
        }
        if (depth == 256)
            ct_fatal("interp $%06X: call depth", at);
        shadow[depth].site = at;
        shadow[depth].is_long = 1;
        shadow[depth++].ret = (uint32_t)c->PB << 16 | c->PC;
        c->PB = (uint8_t)(t >> 16);
        c->PC = (uint16_t)t;
        break;
    }
    case 0x60:
        c->PC = (uint16_t)(pullw(c) + 1);
        if (depth > 0) {
            depth--;
            if (shadow[depth].is_long)
                ct_fatal("interp $%06X: RTS returns from a JSL", at);
            if (c->PC != (uint16_t)shadow[depth].ret)
                ct_fatal("$%06X: callee returned to $%04X, expected $%04X", shadow[depth].site,
                         c->PC, (uint16_t)shadow[depth].ret);
        }
        break;
    case 0x6B:
        c->PC = (uint16_t)(pullw(c) + 1);
        c->PB = pull(c);
        if (depth > 0) {
            depth--;
            if (!shadow[depth].is_long)
                ct_fatal("interp $%06X: RTL returns from a JSR", at);
            if (((uint32_t)c->PB << 16 | c->PC) != shadow[depth].ret)
                ct_fatal("$%06X: callee returned to $%02X%04X, expected $%06X",
                         shadow[depth].site, c->PB, c->PC, shadow[depth].ret);
        }
        break;

    /* block move */
    case 0x54: block_move(c, 1); break;
    case 0x44: block_move(c, -1); break;

    case 0x00: ct_fatal("$%06X: BRK executed", at);
    case 0x02: ct_fatal("$%06X: COP executed", at);
    case 0xDB: ct_fatal("$%06X: STP executed", at);
    case 0xCB: ct_fatal("$%06X: WAI executed", at);
    case 0xEA: break;
    case 0x42: fetch8(c); break;

    default:
        ct_fatal("interp $%06X: opcode $%02X not supported", at, op);
    }
}

void interp_call(CPU *c, uint32_t entry)
{
    if (c->e)
        ct_fatal("interp: emulation mode not supported");
    uint16_t s0 = c->S;
    depth = 0;
    interp_budget = CT_INTERP_BUDGET;
    long dispatch_cap = CT_INTERP_DISPATCH_CAP;
    c->PB = (uint8_t)(entry >> 16);
    c->PC = (uint16_t)entry;
    for (;;) {
        uint32_t at = (uint32_t)c->PB << 16 | c->PC;
        if (--dispatch_cap == 0)
            ct_fatal("$%06X: instruction cap exceeded", at);
        uint8_t op = fetch8(c);
        step(c, at, op);
        if ((op == 0x60 || op == 0x6B) && c->S > s0)
            return;
    }
}
