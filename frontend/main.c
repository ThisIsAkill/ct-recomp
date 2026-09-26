/* SDL2 frontend: Chrono Trigger from reset in the system-mode interpreter
 * under the frame scheduler, in a window.
 *
 * usage: ct_sdl [--scale N] [--frames N] [--dump DIR] [--fast] [--require-render]
 *
 * 256x224, integer scaled (--scale, default 3); paced to 60.0988 Hz
 * (NTSC) unless --fast; 48 kHz stereo audio; keyboard (arrows, Z=B X=A
 * A=Y S=X Q=L W=R, Enter=Start, Right Shift=Select) and the first game
 * controller on pad 1. Esc or closing the window quits. --frames and
 * --dump work as in ct_boot. */
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL.h>

#include "bus.h"
#include "input.h"
#include "interp.h"
#include "png.h"
#include "sched.h"

#define RATE 48000
#define SAMPLES_PER_FRAME 800
#define FRAME_NS 16639267ull   /* 1 / 60.0988 Hz */

static jmp_buf fatal_jmp;
static char fatal_msg[256];

static void on_fatal(const char *msg)
{
    snprintf(fatal_msg, sizeof fatal_msg, "%s", msg);
    longjmp(fatal_jmp, 1);
}

static int pad_button(void *ctx, SDL_GameControllerButton b)
{
    return SDL_GameControllerGetButton(ctx, b);
}

static int pad_axis(void *ctx, SDL_GameControllerAxis a)
{
    return SDL_GameControllerGetAxis(ctx, a);
}

static SDL_GameController *open_controller(void)
{
    for (int k = 0; k < SDL_NumJoysticks(); k++)
        if (SDL_IsGameController(k))
            return SDL_GameControllerOpen(k);
    return NULL;
}

int main(int argc, char **argv)
{
    static long frames = -1;           /* static: survive the longjmp */
    static int scale = 3, fast, require_render;
    static const char *dump;
    for (int k = 1; k < argc; k++) {
        if (!strcmp(argv[k], "--scale") && k + 1 < argc)
            scale = atoi(argv[++k]);
        else if (!strcmp(argv[k], "--frames") && k + 1 < argc)
            frames = atol(argv[++k]);
        else if (!strcmp(argv[k], "--dump") && k + 1 < argc)
            dump = argv[++k];
        else if (!strcmp(argv[k], "--fast"))
            fast = 1;
        else if (!strcmp(argv[k], "--require-render"))
            require_render = 1;
        else {
            fprintf(stderr, "usage: ct_sdl [--scale N] [--frames N] [--dump DIR] [--fast] "
                            "[--require-render]\n");
            return 2;
        }
    }
    if (scale < 1)
        scale = 1;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER)) {
        fprintf(stderr, "ct_sdl: SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window *win = SDL_CreateWindow("Chrono Trigger", SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED, SCHED_WIDTH * scale,
                                       SCHED_HEIGHT * scale, SDL_WINDOW_RESIZABLE);
    SDL_Renderer *ren = win ? SDL_CreateRenderer(win, -1, 0) : NULL;
    if (win && !ren)
        ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
    SDL_Texture *tex = ren ? SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888,
                                               SDL_TEXTUREACCESS_STREAMING, SCHED_WIDTH,
                                               SCHED_HEIGHT) : NULL;
    if (!tex) {
        fprintf(stderr, "ct_sdl: window: %s\n", SDL_GetError());
        return 1;
    }
    SDL_RenderSetLogicalSize(ren, SCHED_WIDTH, SCHED_HEIGHT);
    SDL_RenderSetIntegerScale(ren, SDL_TRUE);

    SDL_AudioSpec want = {0}, have;
    want.freq = RATE;
    want.format = AUDIO_S16SYS;
    want.channels = 2;
    want.samples = 1024;
    SDL_AudioDeviceID audio = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (!audio)
        fprintf(stderr, "ct_sdl: no audio: %s\n", SDL_GetError());
    else
        SDL_PauseAudioDevice(audio, 0);
    SDL_GameController *pad = open_controller();

    static CPU cpu;
    bus_init(NULL);
    interp_reset(&cpu);
    sched_init(&cpu);
    ct_fatal_hook = on_fatal;
    if (setjmp(fatal_jmp)) {
        fprintf(stderr, "ct_sdl: stopped in frame %ld, line %d, PC $%02X%04X: %s\n",
                sched_frame_count(), sched_line(), cpu.PB, cpu.PC, fatal_msg);
        SDL_Quit();
        return 1;
    }

    uint64_t freq = SDL_GetPerformanceFrequency(), next = SDL_GetPerformanceCounter();
    int running = 1;
    for (long f = 0; running && (frames < 0 || f < frames); f++) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT ||
                (ev.type == SDL_KEYDOWN && ev.key.keysym.scancode == SDL_SCANCODE_ESCAPE))
                running = 0;
            else if (ev.type == SDL_CONTROLLERDEVICEADDED && !pad)
                pad = open_controller();
            else if (ev.type == SDL_CONTROLLERDEVICEREMOVED && pad &&
                     !SDL_GameControllerGetAttached(pad)) {
                SDL_GameControllerClose(pad);
                pad = open_controller();
            }
        }
        uint16_t buttons = input_keyboard(SDL_GetKeyboardState(NULL));
        if (pad)
            buttons |= input_controller(pad_button, pad_axis, pad);
        sched_set_joypad(0, buttons);

        sched_run_frame();

        static int16_t samples[SAMPLES_PER_FRAME * 2];
        sched_audio(samples, SAMPLES_PER_FRAME);
        /* Keep at most ~3 frames queued: drop rather than drift behind. */
        if (audio && SDL_GetQueuedAudioSize(audio) < 3 * sizeof samples)
            SDL_QueueAudio(audio, samples, sizeof samples);

        const uint8_t *fb = sched_frame();   /* B G R x = ARGB8888 little-endian */
        SDL_UpdateTexture(tex, NULL, fb, SCHED_WIDTH * 4);
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);
        if (dump)
            png_dump_frame(dump, f + 1, fb, SCHED_WIDTH, SCHED_HEIGHT);

        if (!fast) {
            next += freq * FRAME_NS / 1000000000ull;
            uint64_t now = SDL_GetPerformanceCounter();
            if (now < next)
                SDL_Delay((uint32_t)((next - now) * 1000 / freq));
            else if (now - next > freq / 10)
                next = now;   /* more than 100 ms behind: resync, don't sprint */
        }
    }

    int rendered = 0;
    const uint8_t *fb = sched_frame();
    for (size_t k = 0; k < (size_t)SCHED_WIDTH * SCHED_HEIGHT * 4 && !rendered; k += 4)
        rendered = fb[k] | fb[k + 1] | fb[k + 2];
    printf("ct_sdl: %ld frames, %ld NMIs, PC $%02X%04X\n", sched_frame_count(), sched_nmi_count(),
           cpu.PB, cpu.PC);
    if (pad)
        SDL_GameControllerClose(pad);
    if (audio)
        SDL_CloseAudioDevice(audio);
    SDL_Quit();
    return require_render && !rendered ? 1 : 0;
}
