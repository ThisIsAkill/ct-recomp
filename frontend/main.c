/* SDL2 frontend: Chrono Trigger from reset in the system-mode interpreter
 * under the frame scheduler, in a window.
 *
 * usage: ct_sdl [--scale N] [--frames N] [--dump DIR] [--needed-hw FILE] [--fast]
 *               [--require-render] [--log-input]
 *
 * 256x224, integer scaled (--scale, default 3); paced to 60.0988 Hz
 * (NTSC) unless --fast; 48 kHz stereo audio; keyboard (arrows, Z=B X=A
 * A=Y S=X Q=L W=R, Enter=Start, Right Shift=Select) and the first game
 * controller on pad 1. Ctrl+Q or closing the window quits (not Esc: Steam
 * Input's desktop layout sends Esc for controller B). While a controller
 * is active, key presses within 2 frames of a controller button press are
 * ignored (key_filter in input.h). --frames, --dump
 * and --needed-hw work as in ct_boot.
 *
 * Every exit prints its reason ("ct_sdl: exit: ..."); startup prints each
 * joystick and the button mapping. --log-input prints every key and
 * controller button event. */
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL.h>

#include "bus.h"
#include "hwlog.h"
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

/* Why the process is ending; printed by an atexit handler so no exit path
   goes unexplained. */
static char exit_reason[256] =
    "exit() before the frame loop: see the message above (a ct: line is a fatal error)";
static long exit_frame = -1;

static void set_exit_reason(const char *fmt, ...) CT_PRINTF(1, 2);
static void set_exit_reason(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(exit_reason, sizeof exit_reason, fmt, ap);
    va_end(ap);
}

static void report_exit(void)
{
    fprintf(stderr, "ct_sdl: exit: %s (frame %ld)\n", exit_reason, exit_frame);
}

static void print_controllers(void)
{
    int n = SDL_NumJoysticks();
    fprintf(stderr, "ct_sdl: %d joystick(s)\n", n);
    for (int k = 0; k < n; k++) {
        const char *name = SDL_JoystickNameForIndex(k);
        fprintf(stderr, "ct_sdl:   #%d \"%s\" %s\n", k, name ? name : "?",
                SDL_IsGameController(k) ? "(game controller)" : "(joystick only: not used)");
        if (SDL_IsGameController(k)) {
            char *m = SDL_GameControllerMappingForDeviceIndex(k);
            fprintf(stderr, "ct_sdl:     SDL mapping: %s\n", m ? m : "?");
            SDL_free(m);
        }
    }
    fprintf(stderr, "ct_sdl: pad 1: keyboard arrows, Z=B X=A A=Y S=X Q=L W=R, Enter=Start, "
                    "Right Shift=Select; controller south=B east=A west=Y north=X, "
                    "shoulders=L/R, Start, Back=Select, d-pad/left stick. Ctrl+Q or closing "
                    "the window quits.\n");
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
    static int scale = 3, fast, require_render, log_input;
    static const char *dump, *needed;
    for (int k = 1; k < argc; k++) {
        if (!strcmp(argv[k], "--scale") && k + 1 < argc)
            scale = atoi(argv[++k]);
        else if (!strcmp(argv[k], "--frames") && k + 1 < argc)
            frames = atol(argv[++k]);
        else if (!strcmp(argv[k], "--dump") && k + 1 < argc)
            dump = argv[++k];
        else if (!strcmp(argv[k], "--needed-hw") && k + 1 < argc)
            needed = argv[++k];
        else if (!strcmp(argv[k], "--fast"))
            fast = 1;
        else if (!strcmp(argv[k], "--require-render"))
            require_render = 1;
        else if (!strcmp(argv[k], "--log-input"))
            log_input = 1;
        else {
            fprintf(stderr, "usage: ct_sdl [--scale N] [--frames N] [--dump DIR] "
                            "[--needed-hw FILE] [--fast] [--require-render] [--log-input]\n");
            return 2;
        }
    }
    if (scale < 1)
        scale = 1;
    atexit(report_exit);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER)) {
        fprintf(stderr, "ct_sdl: SDL_Init: %s\n", SDL_GetError());
        set_exit_reason("SDL_Init failed");
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
        set_exit_reason("window or renderer creation failed");
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
    print_controllers();
    SDL_GameController *pad = open_controller();
    if (pad)
        fprintf(stderr, "ct_sdl: using \"%s\" on pad 1\n", SDL_GameControllerName(pad));

    static CPU cpu;
    bus_init(NULL);
    interp_reset(&cpu);
    sched_init(&cpu);
    ct_fatal_hook = on_fatal;
    if (setjmp(fatal_jmp)) {
        static char stop[512];
        snprintf(stop, sizeof stop, "frame %ld, line %d, PC $%02X%04X: %s", sched_frame_count(),
                 sched_line(), cpu.PB, cpu.PC, fatal_msg);
        fprintf(stderr, "ct_sdl: stopped in %s\n", stop);
        set_exit_reason("fatal error: %s", stop);
        if (needed && hw_needed_write(needed, "ct_sdl", stop))
            fprintf(stderr, "ct_sdl: cannot write %s\n", needed);
        SDL_Quit();
        return 1;
    }

    static key_filter keys;
    key_filter_init(&keys);
    uint64_t freq = SDL_GetPerformanceFrequency(), next = SDL_GetPerformanceCounter();
    int running = 1;
    for (long f = 0; running && (frames < 0 || f < frames); f++) {
        exit_frame = f;
        keys.controller = pad != NULL;
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (log_input && (ev.type == SDL_KEYDOWN || ev.type == SDL_KEYUP) && !ev.key.repeat)
                fprintf(stderr, "ct_sdl: frame %ld key %s %s (scancode %d)\n", f,
                        SDL_GetScancodeName(ev.key.keysym.scancode),
                        ev.type == SDL_KEYDOWN ? "down" : "up", ev.key.keysym.scancode);
            if (log_input && (ev.type == SDL_CONTROLLERBUTTONDOWN || ev.type == SDL_CONTROLLERBUTTONUP))
                fprintf(stderr, "ct_sdl: frame %ld controller button %s %s\n", f,
                        SDL_GameControllerGetStringForButton(ev.cbutton.button),
                        ev.type == SDL_CONTROLLERBUTTONDOWN ? "down" : "up");
            if (ev.type == SDL_QUIT) {
                set_exit_reason("SDL_QUIT event (window closed, or quit requested by the "
                                "desktop or another program)");
                running = 0;
            } else if (ev.type == SDL_KEYDOWN && ev.key.keysym.scancode == SDL_SCANCODE_Q &&
                       (ev.key.keysym.mod & KMOD_CTRL)) {
                set_exit_reason("Ctrl+Q");
                running = 0;
            } else if ((ev.type == SDL_KEYDOWN || ev.type == SDL_KEYUP) && !ev.key.repeat) {
                key_filter_key(&keys, ev.key.keysym.scancode, ev.type == SDL_KEYDOWN, f);
            } else if (ev.type == SDL_CONTROLLERBUTTONDOWN) {
                key_filter_button(&keys, f);
            } else if (ev.type == SDL_CONTROLLERDEVICEADDED && !pad)
                pad = open_controller();
            else if (ev.type == SDL_CONTROLLERDEVICEREMOVED && pad &&
                     !SDL_GameControllerGetAttached(pad)) {
                SDL_GameControllerClose(pad);
                pad = open_controller();
            }
        }
        uint16_t buttons = input_keyboard(keys.held);
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

    if (running)
        set_exit_reason("--frames %ld reached", frames);
    exit_frame = sched_frame_count();
    int rendered = 0;
    const uint8_t *fb = sched_frame();
    for (size_t k = 0; k < (size_t)SCHED_WIDTH * SCHED_HEIGHT * 4 && !rendered; k += 4)
        rendered = fb[k] | fb[k + 1] | fb[k + 2];
    printf("ct_sdl: %ld frames, %ld NMIs, PC $%02X%04X\n", sched_frame_count(), sched_nmi_count(),
           cpu.PB, cpu.PC);
    for (unsigned k = 0; k < hw_note_count(); k++)
        printf("ct_sdl: stubbed: %s\n", hw_note_text(k));
    if (needed && hw_needed_write(needed, "ct_sdl", NULL))
        fprintf(stderr, "ct_sdl: cannot write %s\n", needed);
    if (pad)
        SDL_GameControllerClose(pad);
    if (audio)
        SDL_CloseAudioDevice(audio);
    SDL_Quit();
    return require_render && !rendered ? 1 : 0;
}
