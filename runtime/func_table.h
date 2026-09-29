/* Tables the translator emits for one game (ct_funcs.c): recompiled
 * functions, extern call boundaries, and (abs,X) jump table bounds. The
 * interpreter and test harness read them through these declarations;
 * func_table_empty.c provides empty ones for builds with no game. */
#ifndef CT_FUNC_TABLE_H
#define CT_FUNC_TABLE_H

#include <stdint.h>

#include "cpu.h"

typedef struct {
    const char *name;
    uint32_t addr;
    uint8_t m, x;
    uint16_t size;
    int db, dp;         /* entry DB/DP from funcs.toml, -1 if unknown */
    uint8_t calls_extern; /* takes an extern hook itself (never dispatched natively) */
    void (*fn)(CPU *);
} ct_func;

extern const ct_func ct_funcs[];
extern const unsigned ct_func_count;

/* call boundaries: target -> runtime hook */
typedef struct {
    uint32_t addr;
    int kind;           /* 0 JSR, 1 JSL, 2 JML */
    void (*hook)(CPU *);
} ct_extern;

extern const ct_extern ct_externs[];
extern const unsigned ct_extern_count;

/* (abs,X) jump table bounds from funcs.toml */
typedef struct {
    uint32_t site;
    uint16_t count;
} ct_jumptable;

extern const ct_jumptable ct_jumptables[];
extern const unsigned ct_jumptable_count;

/* Code the game puts in WRAM, recompiled from the image it was loaded from
 * (overlays, #92; ct_overlays.c). One entry per function and entry state:
 * it runs native only while WRAM $lo-$hi (the bytes it was decoded from)
 * still hashes to `hash` (FNV-1a 64); otherwise it is interpreted. */
typedef struct {
    const char *name;
    uint32_t addr;      /* entry, 24-bit WRAM address */
    uint8_t m, x;
    uint32_t lo, hi;    /* bytes decoded from, inclusive */
    uint64_t hash;
    void (*fn)(CPU *);
    const uint8_t *image;   /* the whole blob it came from (tests load it) */
    uint32_t base, size;    /* where the blob sits in WRAM */
} ct_overlay_func;

extern const ct_overlay_func ct_overlay_funcs[];
extern const unsigned ct_overlay_func_count;

#endif
