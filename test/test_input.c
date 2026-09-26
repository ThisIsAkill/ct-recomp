/* frontend/input.c: keyboard and controller mapping to SNES pad bits. No
 * ROM, no window. */
#include <stdio.h>
#include <string.h>

#include "input.h"

static long checks, fails;
#define CHECK(c, ...)                                                    \
    do {                                                                 \
        checks++;                                                        \
        if (!(c)) {                                                      \
            fails++;                                                     \
            fprintf(stderr, "%s:%d: FAIL: ", __FILE__, __LINE__);        \
            fprintf(stderr, __VA_ARGS__);                                \
            fputc('\n', stderr);                                         \
        }                                                                \
    } while (0)

static int held[SDL_CONTROLLER_BUTTON_MAX];
static int axes[SDL_CONTROLLER_AXIS_MAX];
static int button(void *ctx, SDL_GameControllerButton b) { (void)ctx; return held[b]; }
static int axis(void *ctx, SDL_GameControllerAxis a) { (void)ctx; return axes[a]; }

int main(void)
{
    uint8_t keys[SDL_NUM_SCANCODES] = {0};
    CHECK(input_keyboard(keys) == 0, "no keys, no buttons");
    keys[SDL_SCANCODE_Z] = keys[SDL_SCANCODE_RETURN] = keys[SDL_SCANCODE_LEFT] = 1;
    CHECK(input_keyboard(keys) == (PAD_B | PAD_START | PAD_LEFT), "Z Enter Left: $%04X",
          input_keyboard(keys));
    memset(keys, 0, sizeof keys);
    keys[SDL_SCANCODE_X] = keys[SDL_SCANCODE_A] = keys[SDL_SCANCODE_S] = 1;
    keys[SDL_SCANCODE_Q] = keys[SDL_SCANCODE_W] = keys[SDL_SCANCODE_RSHIFT] = 1;
    CHECK(input_keyboard(keys) == (PAD_A | PAD_Y | PAD_X | PAD_L | PAD_R | PAD_SELECT),
          "X A S Q W RShift: $%04X", input_keyboard(keys));

    CHECK(input_controller(button, axis, NULL) == 0, "idle controller");
    held[SDL_CONTROLLER_BUTTON_A] = held[SDL_CONTROLLER_BUTTON_Y] = 1;
    held[SDL_CONTROLLER_BUTTON_DPAD_UP] = held[SDL_CONTROLLER_BUTTON_BACK] = 1;
    CHECK(input_controller(button, axis, NULL) == (PAD_B | PAD_X | PAD_UP | PAD_SELECT),
          "south north up back: $%04X", input_controller(button, axis, NULL));
    memset(held, 0, sizeof held);
    axes[SDL_CONTROLLER_AXIS_LEFTX] = 30000;
    axes[SDL_CONTROLLER_AXIS_LEFTY] = -30000;
    CHECK(input_controller(button, axis, NULL) == (PAD_RIGHT | PAD_UP), "stick up-right");
    axes[SDL_CONTROLLER_AXIS_LEFTX] = 8000;
    axes[SDL_CONTROLLER_AXIS_LEFTY] = 0;
    CHECK(input_controller(button, axis, NULL) == 0, "inside the dead zone");

    /* key_filter: Steam Input sends Return with A and Escape with B. */
    key_filter kf;
    key_filter_init(&kf);
    kf.controller = 1;
    key_filter_key(&kf, SDL_SCANCODE_Z, 1, 10);
    CHECK(input_keyboard(kf.held) == PAD_B, "plain key press with a controller attached");
    key_filter_button(&kf, 240);                              /* controller A */
    key_filter_key(&kf, SDL_SCANCODE_RETURN, 1, 241);         /* Steam's Return */
    CHECK(!(input_keyboard(kf.held) & PAD_START), "key 1 frame after a button: ignored");
    key_filter_key(&kf, SDL_SCANCODE_RETURN, 1, 250);         /* still the same press */
    CHECK(!(input_keyboard(kf.held) & PAD_START), "stays ignored until released");
    key_filter_key(&kf, SDL_SCANCODE_RETURN, 0, 251);
    key_filter_key(&kf, SDL_SCANCODE_RETURN, 1, 260);
    CHECK(input_keyboard(kf.held) & PAD_START, "a later, separate press counts");
    key_filter_key(&kf, SDL_SCANCODE_RETURN, 0, 261);

    key_filter_key(&kf, SDL_SCANCODE_X, 1, 454);              /* key first ... */
    CHECK(input_keyboard(kf.held) & PAD_A, "key alone counts at first");
    key_filter_button(&kf, 456);                              /* ... button 2 frames later */
    CHECK(!(input_keyboard(kf.held) & PAD_A), "key just before a button press: dropped");
    key_filter_key(&kf, SDL_SCANCODE_X, 0, 460);

    key_filter_button(&kf, 500);
    key_filter_key(&kf, SDL_SCANCODE_X, 1, 503);
    CHECK(input_keyboard(kf.held) & PAD_A, "3 frames after a button press: counts");
    CHECK(input_keyboard(kf.held) & PAD_B, "Z still held from frame 10");

    key_filter_init(&kf);                                     /* no controller */
    key_filter_button(&kf, 600);
    key_filter_key(&kf, SDL_SCANCODE_RETURN, 1, 600);
    CHECK(input_keyboard(kf.held) & PAD_START, "no controller active: no filtering");

    printf("input: %ld checks, %ld failed\n", checks, fails);
    return fails ? 1 : 0;
}
