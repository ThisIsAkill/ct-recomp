#include "wav.h"

static void le32(uint8_t *p, uint32_t v)
{
    for (int k = 0; k < 4; k++)
        p[k] = (uint8_t)(v >> 8 * k);
}

FILE *wav_open(const char *path, int rate)
{
    FILE *f = fopen(path, "wb");
    if (!f)
        return NULL;
    uint8_t h[44] = {'R', 'I', 'F', 'F', 0, 0, 0, 0, 'W', 'A', 'V', 'E',
                     'f', 'm', 't', ' ', 16, 0, 0, 0, 1, 0, 2, 0};
    le32(h + 24, (uint32_t)rate);
    le32(h + 28, (uint32_t)rate * 4);   /* bytes per second */
    h[32] = 4;                          /* block align */
    h[34] = 16;                         /* bits per sample */
    h[36] = 'd'; h[37] = 'a'; h[38] = 't'; h[39] = 'a';
    fwrite(h, 1, sizeof h, f);
    return f;
}

void wav_write(FILE *f, const int16_t *stereo, int samples)
{
    for (int k = 0; k < samples * 2; k++) {
        uint8_t b[2] = {(uint8_t)stereo[k], (uint8_t)((uint16_t)stereo[k] >> 8)};
        fwrite(b, 1, 2, f);
    }
}

void wav_close(FILE *f)
{
    long end = ftell(f);
    uint8_t v[4];
    if (end >= 44) {
        le32(v, (uint32_t)(end - 8));
        fseek(f, 4, SEEK_SET);
        fwrite(v, 1, 4, f);
        le32(v, (uint32_t)(end - 44));
        fseek(f, 40, SEEK_SET);
        fwrite(v, 1, 4, f);
    }
    fclose(f);
}
