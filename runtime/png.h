/* Minimal PNG writer: 8-bit RGB, uncompressed deflate. No dependencies. */
#ifndef CT_PNG_H
#define CT_PNG_H

#include <stdint.h>

/* bgrx: w x h pixels as bytes B G R x (sched_frame's layout). Returns 0 on
   success, -1 on I/O error. */
int png_write_bgrx(const char *path, const uint8_t *bgrx, int w, int h);

/* --dump: write DIR/frame_NNNNN.png if the frame has any nonblack pixel and
   differs from the last one written. Returns 1 if written, 0 if skipped,
   -1 on error. */
int png_dump_frame(const char *dir, long frame, const uint8_t *bgrx, int w, int h);

#endif
