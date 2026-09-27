/* Streaming stereo resampler: band-limited (Kaiser-windowed sinc, 32 taps,
 * cutoff at 95% of the lower Nyquist frequency) interpolation from one
 * rate to another, with a fine rate adjustment for keeping an output
 * queue at a target level (dynamic rate control).
 *
 * Samples are int16 L R interleaved. Output is produced as input allows;
 * each call consumes all of its input. */
#ifndef CT_RESAMPLE_H
#define CT_RESAMPLE_H

#include <stdint.h>

#define RS_TAPS 32

typedef struct {
    double in_hz, out_hz;
    double adjust;   /* output rate multiplier, 1.0 = nominal */
    double pos;      /* next output position, in input samples from buf[0] */
    float cutoff;    /* normalized to the input rate */
    int len;         /* input samples buffered */
    float buf[2][4096 + RS_TAPS];
} resampler;

void rs_init(resampler *r, double in_hz, double out_hz);
/* Output rate x factor; clamped to [0.99, 1.01]. */
void rs_set_adjust(resampler *r, double factor);
/* Consume n input samples, write at most max output samples; returns the
   count written. Input beyond the buffer (4096 samples per call) is an
   error: returns -1. */
int rs_process(resampler *r, const int16_t *in, int n, int16_t *out, int max);

/* Dynamic rate control: the output rate factor that steers a queue
   holding `level` samples (smoothed) toward `target`, proportionally,
   within 1 +- max_dev (e.g. 0.005). Above target: produce less. */
double rs_rate_control(double level, double target, double max_dev);

#endif
