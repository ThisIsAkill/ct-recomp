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

#endif
