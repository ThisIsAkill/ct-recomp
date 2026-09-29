#include "overlay.h"

#include <stdlib.h>
#include <string.h>

#include "bus.h"

static const ct_overlay_func **ovl;   /* sorted by entry address */
static struct {
    uint64_t gens;   /* sum of its pages' write counters at the last check */
    int checked, valid;
} *cache;
static unsigned n_ovl;

static int by_addr(const void *a, const void *b)
{
    uint32_t x = (*(const ct_overlay_func *const *)a)->addr;
    uint32_t y = (*(const ct_overlay_func *const *)b)->addr;
    return x < y ? -1 : x > y;
}

void overlay_init(void)
{
    free(ovl);
    free(cache);
    ovl = malloc((ct_overlay_func_count + 1) * sizeof *ovl);
    cache = calloc(ct_overlay_func_count + 1, sizeof *cache);
    n_ovl = 0;
    const char *env = getenv("CT_OVERLAYS");   /* "0": never dispatch (bisecting) */
    if (env && !strcmp(env, "0"))
        return;
    for (unsigned k = 0; k < ct_overlay_func_count; k++)
        if (ct_overlay_funcs[k].fn)
            ovl[n_ovl++] = &ct_overlay_funcs[k];
    qsort(ovl, n_ovl, sizeof *ovl, by_addr);
}

static uint64_t page_gens(const ct_overlay_func *f)
{
    uint64_t sum = 0;
    for (uint32_t p = (f->lo & 0x1FFFF) >> 8; p <= (f->hi & 0x1FFFF) >> 8; p++)
        sum += ct_wram_gen[p];
    return sum;
}

static int matches(const ct_overlay_func *f)
{
    const uint8_t *w = bus_wram();
    uint64_t h = 0xCBF29CE484222325ull;
    for (uint32_t a = f->lo; a <= f->hi; a++)
        h = (h ^ w[a & 0x1FFFF]) * 0x100000001B3ull;
    return h == f->hash;
}

static int valid(unsigned k)
{
    uint64_t g = page_gens(ovl[k]);
    if (!cache[k].checked || cache[k].gens != g) {
        cache[k].checked = 1;
        cache[k].gens = g;
        cache[k].valid = matches(ovl[k]);
    }
    return cache[k].valid;
}

static unsigned first_at(uint32_t at)
{
    unsigned lo = 0, hi = n_ovl;
    while (lo < hi) {
        unsigned mid = (lo + hi) / 2;
        if (ovl[mid]->addr < at)
            lo = mid + 1;
        else
            hi = mid;
    }
    return lo;
}

const ct_overlay_func *overlay_lookup(const CPU *c)
{
    if (c->e || (c->PB != 0x7E && c->PB != 0x7F) || !n_ovl)
        return NULL;
    uint32_t at = (uint32_t)c->PB << 16 | c->PC;
    for (unsigned k = first_at(at); k < n_ovl && ovl[k]->addr == at; k++)
        if (ovl[k]->m == c->m && ovl[k]->x == c->x && valid(k))
            return ovl[k];
    return NULL;
}

int overlay_state(uint32_t addr, int m, int x, int *stale)
{
    int any = 0;
    *stale = 0;
    for (unsigned k = first_at(addr); k < n_ovl && ovl[k]->addr == addr; k++)
        if (ovl[k]->m == m && ovl[k]->x == x) {
            any = 1;
            if (!valid(k))
                *stale = 1;
        }
    return any;
}
