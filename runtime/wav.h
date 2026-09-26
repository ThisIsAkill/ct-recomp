/* Minimal WAV writer: 16-bit stereo PCM, streamed. */
#ifndef CT_WAV_H
#define CT_WAV_H

#include <stdint.h>
#include <stdio.h>

FILE *wav_open(const char *path, int rate);
void wav_write(FILE *f, const int16_t *stereo, int samples);
/* Patches the header sizes and closes. */
void wav_close(FILE *f);

#endif
