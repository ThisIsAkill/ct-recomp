#include "input.h"

uint16_t input_keyboard(const uint8_t *keys)
{
    static const struct { SDL_Scancode key; uint16_t bit; } map[] = {
        {SDL_SCANCODE_UP, PAD_UP},       {SDL_SCANCODE_DOWN, PAD_DOWN},
        {SDL_SCANCODE_LEFT, PAD_LEFT},   {SDL_SCANCODE_RIGHT, PAD_RIGHT},
        {SDL_SCANCODE_Z, PAD_B},         {SDL_SCANCODE_X, PAD_A},
        {SDL_SCANCODE_A, PAD_Y},         {SDL_SCANCODE_S, PAD_X},
        {SDL_SCANCODE_Q, PAD_L},         {SDL_SCANCODE_W, PAD_R},
        {SDL_SCANCODE_RETURN, PAD_START}, {SDL_SCANCODE_RSHIFT, PAD_SELECT},
    };
    uint16_t pad = 0;
    for (unsigned k = 0; k < sizeof map / sizeof map[0]; k++)
        if (keys[map[k].key])
            pad |= map[k].bit;
    return pad;
}

uint16_t input_controller(int (*button)(void *ctx, SDL_GameControllerButton b),
                          int (*axis)(void *ctx, SDL_GameControllerAxis a), void *ctx)
{
    static const struct { SDL_GameControllerButton b; uint16_t bit; } map[] = {
        {SDL_CONTROLLER_BUTTON_A, PAD_B},             /* south */
        {SDL_CONTROLLER_BUTTON_B, PAD_A},             /* east */
        {SDL_CONTROLLER_BUTTON_X, PAD_Y},             /* west */
        {SDL_CONTROLLER_BUTTON_Y, PAD_X},             /* north */
        {SDL_CONTROLLER_BUTTON_LEFTSHOULDER, PAD_L},
        {SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, PAD_R},
        {SDL_CONTROLLER_BUTTON_START, PAD_START},
        {SDL_CONTROLLER_BUTTON_BACK, PAD_SELECT},
        {SDL_CONTROLLER_BUTTON_DPAD_UP, PAD_UP},
        {SDL_CONTROLLER_BUTTON_DPAD_DOWN, PAD_DOWN},
        {SDL_CONTROLLER_BUTTON_DPAD_LEFT, PAD_LEFT},
        {SDL_CONTROLLER_BUTTON_DPAD_RIGHT, PAD_RIGHT},
    };
    uint16_t pad = 0;
    for (unsigned k = 0; k < sizeof map / sizeof map[0]; k++)
        if (button(ctx, map[k].b))
            pad |= map[k].bit;
    const int dead = 16000;   /* of 32767 */
    int x = axis(ctx, SDL_CONTROLLER_AXIS_LEFTX), y = axis(ctx, SDL_CONTROLLER_AXIS_LEFTY);
    if (x < -dead)
        pad |= PAD_LEFT;
    if (x > dead)
        pad |= PAD_RIGHT;
    if (y < -dead)
        pad |= PAD_UP;
    if (y > dead)
        pad |= PAD_DOWN;
    return pad;
}
