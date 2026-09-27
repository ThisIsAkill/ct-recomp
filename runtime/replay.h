/* Input replay scripts: pad 1 buttons per frame, as text.
 *
 * One span per line, "F1-F2:BUTTONS": hold BUTTONS for frames F1..F2
 * (1-based, inclusive). BUTTONS is names joined by '+' (b y select start
 * up down left right a x l r) or a hex mask ("0x1080", bits as in
 * sched_set_joypad). Spans may overlap; their buttons combine. '#' starts
 * a comment. Frame F's buttons are the ones its auto-joypad read sees:
 * the scheduler's frame hook sets them at the edge of frame F-1.
 *
 * The recorder writes the same format, one span per run of identical
 * nonzero buttons, so a recording replays through replay_load. */
#ifndef CT_REPLAY_H
#define CT_REPLAY_H

#include <stdint.h>
#include <stdio.h>

typedef struct {
    long from, to;
    uint16_t buttons;
} replay_span;

typedef struct {
    replay_span *spans;
    int n, cap;
} replay;

/* Add one "F1-F2:BUTTONS" span; 0 on success, -1 if malformed. */
int replay_add(replay *r, const char *spec);
/* Add every span in the file at path; 0 on success, -1 if unreadable or
   malformed (the first bad line goes to stderr). */
int replay_load(replay *r, const char *path);
uint16_t replay_buttons(const replay *r, long frame);
void replay_free(replay *r);

typedef struct {
    FILE *f;
    long from, last;
    uint16_t buttons;
} replay_rec;

/* 0 on success, -1 if path can't be written. */
int replay_rec_open(replay_rec *w, const char *path, const char *source);
/* The buttons of frame `frame`; call once per frame in order. */
void replay_rec_frame(replay_rec *w, long frame, uint16_t buttons);
/* Flush the open span and note the frame count; safe to call twice. */
void replay_rec_close(replay_rec *w);

#endif
