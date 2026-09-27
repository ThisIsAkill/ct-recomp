/* Input replay scripts (runtime/replay.c): span parsing, overlapping spans,
 * malformed specs, and the recorder's output reading back as the same
 * buttons on every frame. */
#include <unistd.h>

#include "harness.h"
#include "replay.h"

static uint16_t pattern(long f)
{
    if (f >= 10 && f <= 12)
        return 0x1000;   /* start */
    if (f >= 20 && f <= 40)
        return (uint16_t)(0x0800 | (f >= 30 ? 0x0080 : 0));   /* up, then up+a */
    if (f == 41)
        return 0x0080;
    return 0;
}

int main(int argc, char **argv)
{
    replay r = {0};
    CHECK(!replay_add(&r, "5-7:a+b"), "names");
    CHECK(!replay_add(&r, "6-6:0x0010"), "hex mask");
    CHECK(replay_buttons(&r, 4) == 0 && replay_buttons(&r, 5) == 0x8080 &&
              replay_buttons(&r, 6) == 0x8090 && replay_buttons(&r, 8) == 0,
          "overlapping spans combine: %04X", replay_buttons(&r, 6));
    CHECK(replay_add(&r, "0-3:a") && replay_add(&r, "7-5:a") && replay_add(&r, "1-2:jump") &&
              replay_add(&r, "1-2:0x1FFFF") && replay_add(&r, "12:a"),
          "malformed specs rejected");
    replay_free(&r);

    char path[] = "/tmp/ct_replay_XXXXXX";
    int fd = mkstemp(path);
    CHECK(fd >= 0, "temp file");
    close(fd);
    replay_rec w;
    CHECK(!replay_rec_open(&w, path, "test_replay"), "recorder opens");
    for (long f = 1; f <= 50; f++)
        replay_rec_frame(&w, f, (uint16_t)(pattern(f) | 0x000F));   /* pad ID bits dropped */
    replay_rec_close(&w);
    replay_rec_close(&w);   /* twice is harmless */
    CHECK(!replay_load(&r, path), "recording loads");
    int same = 1;
    for (long f = 1; f <= 50; f++)
        same &= replay_buttons(&r, f) == pattern(f);
    CHECK(same, "recording reads back frame for frame");
    CHECK(r.n == 4, "one span per run: %d", r.n);
    replay_free(&r);
    remove(path);
    (void)argc;
    (void)argv;
    return th_report("replay");
}
