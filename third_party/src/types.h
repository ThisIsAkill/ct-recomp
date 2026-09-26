/* ct-recomp: not from zelda3. ppu.c/ppu.h/dsp.c expect this path
 * (src/types.h, per zelda3's own layout) for a handful of short type
 * aliases and one config enum. Trimmed to just what's referenced --
 * zelda3's real src/ is Zelda-specific game code we don't want. */
#ifndef CT_ZELDA3_TYPES_COMPAT_H
#define CT_ZELDA3_TYPES_COMPAT_H

#include <stdint.h>

typedef uint8_t uint8;
typedef int8_t int8;
typedef uint16_t uint16;
typedef int16_t int16;
typedef uint32_t uint32;
typedef int32_t int32;
typedef uint64_t uint64;
typedef int64_t int64;
typedef unsigned int uint;

#define countof(a) (sizeof(a) / sizeof(*(a)))
#define FORCEINLINE inline
#define NOINLINE

static FORCEINLINE int IntMin(int a, int b) { return a < b ? a : b; }
static FORCEINLINE int IntMax(int a, int b) { return a > b ? a : b; }
static FORCEINLINE uint UintMin(uint a, uint b) { return a < b ? a : b; }

enum {
    /* Real hardware has no side padding; this is a zelda3 PC-port
     * widescreen feature we don't want. */
    kPpuExtraLeftRight = 0,
};

#endif
