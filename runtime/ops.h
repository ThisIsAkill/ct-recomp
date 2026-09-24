/* Instruction semantics shared by generated code. */
#ifndef CT_OPS_H
#define CT_OPS_H

#include <stdint.h>

#include "bus.h"
#include "cpu.h"

/* ---- flags ---- */

static inline void set_nz8(CPU *c, uint8_t v)   { c->n = v >> 7;  c->z = v == 0; }
static inline void set_nz16(CPU *c, uint16_t v) { c->n = v >> 15; c->z = v == 0; }

static inline uint8_t get_p(const CPU *c)
{
    return (uint8_t)(c->n << 7 | c->v << 6 | c->m << 5 | c->x << 4 |
                     c->d << 3 | c->i << 2 | c->z << 1 | c->c);
}

static inline void index_width(CPU *c)
{
    if (c->x) {
        c->X &= 0xFF;
        c->Y &= 0xFF;
    }
}

static inline void set_p(CPU *c, uint8_t p)
{
    c->n = p >> 7 & 1; c->v = p >> 6 & 1; c->m = p >> 5 & 1; c->x = p >> 4 & 1;
    c->d = p >> 3 & 1; c->i = p >> 2 & 1; c->z = p >> 1 & 1; c->c = p & 1;
    if (c->e) {
        c->m = 1;
        c->x = 1;
    }
    index_width(c);
}

static inline void op_rep(CPU *c, uint8_t v) { set_p(c, get_p(c) & (uint8_t)~v); }
static inline void op_sep(CPU *c, uint8_t v) { set_p(c, get_p(c) | v); }

/* ---- mode checks ---- */

static inline void cpu_enter(const CPU *c, uint32_t at, int m, int x)
{
    if (c->e || c->m != m || c->x != x)
        ct_fatal("$%06X: entry state m%d x%d e%d, expected m%d x%d e0",
                 at, c->m, c->x, c->e, m, x);
}

static inline void cpu_check_mx(const CPU *c, uint32_t at, int m, int x)
{
    if (c->m != m || c->x != x)
        ct_fatal("$%06X: runtime state m%d x%d, static state m%d x%d",
                 at, c->m, c->x, m, x);
}

/* After a JSR'd callee returns: it must have returned to the JSR site + 3. */
static inline void cpu_check_return(const CPU *c, uint32_t site, uint16_t expect)
{
    if (c->PC != expect)
        ct_fatal("$%06X: callee returned to $%04X, expected $%04X", site, c->PC, expect);
}

/* ---- effective addresses ---- */

static inline uint32_t ea_dp(const CPU *c, uint8_t d)  { return (uint16_t)(c->DP + d); }
static inline uint32_t ea_abs(const CPU *c, uint16_t a) { return (uint32_t)c->DB << 16 | a; }

/* Indexed absolute/long addresses carry into the next bank. */
static inline uint32_t ea_abs_x(const CPU *c, uint16_t a)
{
    return (((uint32_t)c->DB << 16 | a) + c->X) & 0xFFFFFF;
}

static inline uint32_t ea_abs_y(const CPU *c, uint16_t a)
{
    return (((uint32_t)c->DB << 16 | a) + c->Y) & 0xFFFFFF;
}

static inline uint32_t ea_long_x(const CPU *c, uint32_t a) { return (a + c->X) & 0xFFFFFF; }

/* Direct-page 16-bit access wraps within bank 0. */
static inline uint16_t read16_dp(const CPU *c, uint8_t d)
{
    return (uint16_t)(read8(ea_dp(c, d)) | read8((uint16_t)(c->DP + d + 1)) << 8);
}

static inline void write16_dp(const CPU *c, uint8_t d, uint16_t v)
{
    write8(ea_dp(c, d), (uint8_t)v);
    write8((uint16_t)(c->DP + d + 1), (uint8_t)(v >> 8));
}

/* ---- stack (bank 0) ---- */

static inline void push8(CPU *c, uint8_t v)
{
    write8(c->S, v);
    c->S = c->e ? (uint16_t)(0x0100 | ((c->S - 1) & 0xFF)) : (uint16_t)(c->S - 1);
}

static inline uint8_t pull8(CPU *c)
{
    c->S = c->e ? (uint16_t)(0x0100 | ((c->S + 1) & 0xFF)) : (uint16_t)(c->S + 1);
    return read8(c->S);
}

static inline void push16(CPU *c, uint16_t v)
{
    push8(c, (uint8_t)(v >> 8));
    push8(c, (uint8_t)v);
}

static inline uint16_t pull16(CPU *c)
{
    uint8_t lo = pull8(c);
    return (uint16_t)(lo | pull8(c) << 8);
}

/* RTS: pull return address - 1. */
static inline void op_rts(CPU *c) { c->PC = (uint16_t)(pull16(c) + 1); }

/* ---- accumulator ---- */

static inline uint8_t a8(const CPU *c) { return (uint8_t)c->A; }
static inline void set_a8(CPU *c, uint8_t v) { c->A = (uint16_t)((c->A & 0xFF00) | v); }

static inline void lda8(CPU *c, uint8_t v)   { set_a8(c, v); set_nz8(c, v); }
static inline void lda16(CPU *c, uint16_t v) { c->A = v; set_nz16(c, v); }
static inline void ldx16(CPU *c, uint16_t v) { c->X = v; set_nz16(c, v); }
static inline void ldy16(CPU *c, uint16_t v) { c->Y = v; set_nz16(c, v); }

/* ---- index registers (16-bit forms; x=0) ---- */

static inline void tax16(CPU *c) { c->X = c->A; set_nz16(c, c->X); }
static inline void tay16(CPU *c) { c->Y = c->A; set_nz16(c, c->Y); }
static inline void iny16(CPU *c) { c->Y = (uint16_t)(c->Y + 1); set_nz16(c, c->Y); }
static inline void inx16(CPU *c) { c->X = (uint16_t)(c->X + 1); set_nz16(c, c->X); }
static inline void dex16(CPU *c) { c->X = (uint16_t)(c->X - 1); set_nz16(c, c->X); }

/* MVN with 16-bit index registers: copy C+1 bytes src:X -> dst:Y ascending.
   Ends with A=$FFFF, DB=dst. */
static inline void mvn16(CPU *c, uint8_t dst, uint8_t src)
{
    c->DB = dst;
    do {
        write8((uint32_t)dst << 16 | c->Y, read8((uint32_t)src << 16 | c->X));
        c->X = (uint16_t)(c->X + 1);
        c->Y = (uint16_t)(c->Y + 1);
        c->A = (uint16_t)(c->A - 1);
    } while (c->A != 0xFFFF);
}

static inline void op_tdc(CPU *c) { c->A = c->DP; set_nz16(c, c->A); }

static inline void asl_a8(CPU *c)
{
    uint8_t v = a8(c);
    c->c = v >> 7;
    v = (uint8_t)(v << 1);
    set_a8(c, v);
    set_nz8(c, v);
}

static inline void asl_a16(CPU *c)
{
    c->c = c->A >> 15;
    c->A = (uint16_t)(c->A << 1);
    set_nz16(c, c->A);
}

static inline void lsr_a8(CPU *c)
{
    uint8_t v = a8(c);
    c->c = v & 1;
    v >>= 1;
    set_a8(c, v);
    set_nz8(c, v);
}

static inline void lsr_a16(CPU *c)
{
    c->c = c->A & 1;
    c->A >>= 1;
    set_nz16(c, c->A);
}

static inline uint8_t inc8(CPU *c, uint8_t v) { v = (uint8_t)(v + 1); set_nz8(c, v); return v; }
static inline uint8_t dec8(CPU *c, uint8_t v) { v = (uint8_t)(v - 1); set_nz8(c, v); return v; }

/* ---- arithmetic (binary only; decimal mode is fatal in v0) ---- */

static inline void adc8(CPU *c, uint8_t b, uint32_t at)
{
    if (c->d)
        ct_fatal("$%06X: ADC with D=1 not supported", at);
    uint8_t a = a8(c);
    unsigned r = (unsigned)a + b + c->c;
    c->c = r > 0xFF;
    c->v = ((~(a ^ b) & (a ^ r)) >> 7) & 1;
    set_a8(c, (uint8_t)r);
    set_nz8(c, (uint8_t)r);
}

static inline void adc16(CPU *c, uint16_t b, uint32_t at)
{
    if (c->d)
        ct_fatal("$%06X: ADC with D=1 not supported", at);
    uint16_t a = c->A;
    unsigned r = (unsigned)a + b + c->c;
    c->c = r > 0xFFFF;
    c->v = ((~(a ^ b) & (a ^ r)) >> 15) & 1;
    c->A = (uint16_t)r;
    set_nz16(c, c->A);
}

static inline void sbc8(CPU *c, uint8_t b, uint32_t at)
{
    if (c->d)
        ct_fatal("$%06X: SBC with D=1 not supported", at);
    adc8(c, (uint8_t)~b, at);
}

static inline void sbc16(CPU *c, uint16_t b, uint32_t at)
{
    if (c->d)
        ct_fatal("$%06X: SBC with D=1 not supported", at);
    adc16(c, (uint16_t)~b, at);
}

static inline void cmp8(CPU *c, uint8_t r, uint8_t b)
{
    c->c = r >= b;
    set_nz8(c, (uint8_t)(r - b));
}

static inline void cmp16(CPU *c, uint16_t r, uint16_t b)
{
    c->c = r >= b;
    set_nz16(c, (uint16_t)(r - b));
}

#endif
