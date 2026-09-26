#include "png.h"

#include <stdio.h>
#include <stdlib.h>

static uint32_t crc_table[256];

static uint32_t crc(uint32_t c, const uint8_t *p, size_t n)
{
    if (!crc_table[1])
        for (uint32_t k = 0; k < 256; k++) {
            uint32_t v = k;
            for (int b = 0; b < 8; b++)
                v = v & 1 ? 0xEDB88320u ^ (v >> 1) : v >> 1;
            crc_table[k] = v;
        }
    for (size_t k = 0; k < n; k++)
        c = crc_table[(c ^ p[k]) & 0xFF] ^ (c >> 8);
    return c;
}

static void be32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v >> 24);
    p[1] = (uint8_t)(v >> 16);
    p[2] = (uint8_t)(v >> 8);
    p[3] = (uint8_t)v;
}

static int chunk(FILE *f, const char *type, const uint8_t *data, uint32_t n)
{
    uint8_t hdr[8];
    be32(hdr, n);
    for (int k = 0; k < 4; k++)
        hdr[4 + k] = (uint8_t)type[k];
    uint32_t c = crc(0xFFFFFFFFu, hdr + 4, 4);
    c = crc(c, data, n) ^ 0xFFFFFFFFu;
    uint8_t tail[4];
    be32(tail, c);
    return fwrite(hdr, 1, 8, f) == 8 && (!n || fwrite(data, 1, n, f) == n) &&
           fwrite(tail, 1, 4, f) == 4 ? 0 : -1;
}

int png_write_bgrx(const char *path, const uint8_t *bgrx, int w, int h)
{
    /* Raw scanlines: filter byte 0, then RGB. */
    size_t row = (size_t)w * 3 + 1, raw_n = row * (size_t)h;
    uint8_t *raw = malloc(raw_n);
    size_t blocks = (raw_n + 65534) / 65535;
    uint8_t *z = malloc(2 + raw_n + blocks * 5 + 4);
    if (!raw || !z) {
        free(raw);
        free(z);
        return -1;
    }
    for (int y = 0; y < h; y++) {
        uint8_t *d = raw + (size_t)y * row;
        const uint8_t *s = bgrx + (size_t)y * w * 4;
        *d++ = 0;
        for (int x = 0; x < w; x++, s += 4) {
            *d++ = s[2];
            *d++ = s[1];
            *d++ = s[0];
        }
    }
    /* zlib stream of stored deflate blocks */
    size_t zn = 0;
    z[zn++] = 0x78;
    z[zn++] = 0x01;
    uint32_t a = 1, b = 0;
    for (size_t off = 0; off < raw_n;) {
        size_t len = raw_n - off < 65535 ? raw_n - off : 65535;
        z[zn++] = off + len == raw_n;
        z[zn++] = (uint8_t)len;
        z[zn++] = (uint8_t)(len >> 8);
        z[zn++] = (uint8_t)~len;
        z[zn++] = (uint8_t)(~len >> 8);
        for (size_t k = 0; k < len; k++) {
            uint8_t v = raw[off + k];
            z[zn++] = v;
            a = (a + v) % 65521;
            b = (b + a) % 65521;
        }
        off += len;
    }
    be32(z + zn, b << 16 | a);
    zn += 4;

    static const uint8_t sig[8] = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1A, '\n'};
    uint8_t ihdr[13];
    be32(ihdr, (uint32_t)w);
    be32(ihdr + 4, (uint32_t)h);
    ihdr[8] = 8;    /* bit depth */
    ihdr[9] = 2;    /* RGB */
    ihdr[10] = ihdr[11] = ihdr[12] = 0;
    FILE *f = fopen(path, "wb");
    int rc = !f || fwrite(sig, 1, 8, f) != 8 || chunk(f, "IHDR", ihdr, 13) ||
             chunk(f, "IDAT", z, (uint32_t)zn) || chunk(f, "IEND", NULL, 0) ? -1 : 0;
    if (f && fclose(f))
        rc = -1;
    free(raw);
    free(z);
    return rc;
}

static uint32_t fnv(const uint8_t *p, size_t n)
{
    uint32_t h = 2166136261u;
    for (size_t k = 0; k < n; k++)
        h = (h ^ p[k]) * 16777619u;
    return h;
}

int png_dump_frame(const char *dir, long frame, const uint8_t *bgrx, int w, int h)
{
    static uint32_t last;
    size_t n = (size_t)w * h * 4;
    int lit = 0;
    for (size_t k = 0; k < n && !lit; k += 4)
        lit = bgrx[k] | bgrx[k + 1] | bgrx[k + 2];
    uint32_t hv = fnv(bgrx, n);
    if (!lit || hv == last)
        return 0;
    last = hv;
    char path[4096];
    snprintf(path, sizeof path, "%s/frame_%05ld.png", dir, frame);
    if (png_write_bgrx(path, bgrx, w, h)) {
        fprintf(stderr, "cannot write %s\n", path);
        return -1;
    }
    return 1;
}
