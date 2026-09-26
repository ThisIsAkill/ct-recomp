/* Keyboard and game controller state -> SNES pad bits (sched_set_joypad
 * layout). Pure functions over SDL state, so they can be tested without a
 * window. */
#ifndef CT_INPUT_H
#define CT_INPUT_H

#include <stdint.h>

#include <SDL.h>

#define PAD_B      0x8000
#define PAD_Y      0x4000
#define PAD_SELECT 0x2000
#define PAD_START  0x1000
#define PAD_UP     0x0800
#define PAD_DOWN   0x0400
#define PAD_LEFT   0x0200
#define PAD_RIGHT  0x0100
#define PAD_A      0x0080
#define PAD_X      0x0040
#define PAD_L      0x0020
#define PAD_R      0x0010

/* keys: held state per scancode (key_filter.held). Arrows, Z=B X=A A=Y S=X Q=L W=R,
   Enter=Start, Right Shift=Select. */
uint16_t input_keyboard(const uint8_t *keys);

/* button(b) / axis(a): SDL_GameControllerGetButton/GetAxis for one pad.
   Face buttons by position (south=B, east=A, west=Y, north=X, as on the
   SNES), shoulders L/R, Start, Back=Select, d-pad and left stick. */
uint16_t input_controller(int (*button)(void *ctx, SDL_GameControllerButton b),
                          int (*axis)(void *ctx, SDL_GameControllerAxis a), void *ctx);

/* Held keys, built from key events. While a game controller is active, a
   key press within KEY_FILTER_FRAMES of a controller button press (either
   order) is ignored until that key is released: Steam Input's desktop
   layout sends a keyboard key along with some controller buttons (B ->
   Escape, A -> Return), which would otherwise press a second SNES button. */
#define KEY_FILTER_FRAMES 2

typedef struct {
    uint8_t held[SDL_NUM_SCANCODES];      /* for input_keyboard() */
    uint8_t ignored[SDL_NUM_SCANCODES];   /* pressed, filtered until released */
    long pressed_at[SDL_NUM_SCANCODES];
    long last_button;                     /* frame of the last controller press */
    int controller;                       /* a game controller is active */
} key_filter;

void key_filter_init(key_filter *kf);
void key_filter_key(key_filter *kf, SDL_Scancode sc, int down, long frame);
void key_filter_button(key_filter *kf, long frame);

#endif
