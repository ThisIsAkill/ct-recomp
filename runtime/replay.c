#include "replay.h"

#include <stdlib.h>
#include <string.h>

static const struct {
    const char *name;
    uint16_t bit;
} names[] = {
    {"b", 0x8000},  {"y", 0x4000},    {"select", 0x2000}, {"start", 0x1000},
    {"up", 0x0800}, {"down", 0x0400}, {"left", 0x0200},   {"right", 0x0100},
    {"a", 0x0080},  {"x", 0x0040},    {"l", 0x0020},      {"r", 0x0010},
};
#define N_NAMES (sizeof names / sizeof names[0])

int replay_add(replay *r, const char *spec)
{
    long from, to;
    char buttons[128];
    if (sscanf(spec, "%ld-%ld:%127s", &from, &to, buttons) != 3 || from < 1 || to < from)
        return -1;
    uint16_t b = 0;
    if (!strncmp(buttons, "0x", 2)) {
        char *end;
        unsigned long v = strtoul(buttons + 2, &end, 16);
        if (*end || v > 0xFFFF)
            return -1;
        b = (uint16_t)v;
    } else {
        for (char *t = strtok(buttons, "+"); t; t = strtok(NULL, "+")) {
            unsigned k;
            for (k = 0; k < N_NAMES && strcmp(t, names[k].name); k++)
                ;
            if (k == N_NAMES)
                return -1;
            b |= names[k].bit;
        }
    }
    if (r->n == r->cap) {
        int cap = r->cap ? 2 * r->cap : 64;
        replay_span *s = realloc(r->spans, (size_t)cap * sizeof *s);
        if (!s)
            return -1;
        r->spans = s;
        r->cap = cap;
    }
    r->spans[r->n++] = (replay_span){from, to, b};
    return 0;
}

static int replay_add_reset(replay *r, long frame)
{
    if (r->n_resets == r->cap_resets) {
        int cap = r->cap_resets ? 2 * r->cap_resets : 8;
        long *v = realloc(r->resets, (size_t)cap * sizeof *v);
        if (!v)
            return -1;
        r->resets = v;
        r->cap_resets = cap;
    }
    r->resets[r->n_resets++] = frame;
    return 0;
}

int replay_load(replay *r, const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f)
        return -1;
    char line[256];
    int bad = 0, n = 0;
    while (!bad && fgets(line, sizeof line, f)) {
        n++;
        char *h = strchr(line, '#');
        if (h)
            *h = 0;
        char tok[160];
        long rf;
        char extra;
        if (sscanf(line, " reset %ld %c", &rf, &extra) == 1 && rf >= 1) {
            if (replay_add_reset(r, rf)) {
                bad = 1;
                break;
            }
            continue;
        }
        if (sscanf(line, "%159s", tok) == 1 && replay_add(r, tok)) {
            fprintf(stderr, "replay: %s:%d: bad span \"%s\"\n", path, n, tok);
            bad = 1;
        }
    }
    fclose(f);
    return bad ? -1 : 0;
}

uint16_t replay_buttons(const replay *r, long frame)
{
    uint16_t b = 0;
    for (int k = 0; k < r->n; k++)
        if (frame >= r->spans[k].from && frame <= r->spans[k].to)
            b |= r->spans[k].buttons;
    return b;
}

int replay_reset(const replay *r, long frame)
{
    for (int k = 0; k < r->n_resets; k++)
        if (r->resets[k] == frame)
            return 1;
    return 0;
}

void replay_free(replay *r)
{
    free(r->spans);
    free(r->resets);
    *r = (replay){0};
}

int replay_rec_open(replay_rec *w, const char *path, const char *source)
{
    *w = (replay_rec){0};
    if (!(w->f = fopen(path, "w")))
        return -1;
    fprintf(w->f, "# Pad 1 input recorded by %s, one span per run of held buttons.\n"
                  "# Replay: ct_boot --script FILE --frames N (N on the last line).\n",
            source);
    return 0;
}

static void flush_span(replay_rec *w)
{
    if (!w->buttons)
        return;
    fprintf(w->f, "%ld-%ld:", w->from, w->last);
    const char *sep = "";
    for (unsigned k = 0; k < N_NAMES; k++)
        if (w->buttons & names[k].bit) {
            fprintf(w->f, "%s%s", sep, names[k].name);
            sep = "+";
        }
    fputc('\n', w->f);
}

void replay_rec_frame(replay_rec *w, long frame, uint16_t buttons)
{
    if (!w->f)
        return;
    buttons &= 0xFFF0;   /* the 12 buttons; the low bits are the pad ID */
    if (buttons != w->buttons || frame != w->last + 1) {
        flush_span(w);
        w->buttons = buttons;
        w->from = frame;
    }
    w->last = frame;
}

void replay_rec_close(replay_rec *w)
{
    if (!w->f)
        return;
    flush_span(w);
    fprintf(w->f, "# frames %ld\n", w->last);
    fclose(w->f);
    w->f = NULL;
}
