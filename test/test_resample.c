/* Resampler (runtime/resample.c): output count follows the rate and its
 * adjustment, a tone below the input Nyquist comes out clean (SNR against
 * the ideal sine), and rate control stays within its bounds. */
#include <math.h>

#include "harness.h"
#include "resample.h"

#define PI 3.14159265358979323846

/* Resample a tone of `hz` from 32000 to 48000 Hz in blocks of 500;
   returns the SNR in dB of the output against a fitted ideal sine,
   skipping the filter's start-up. */
static double tone_snr(double hz, double adjust, long *n_out, long n_in)
{
    static resampler r;
    rs_init(&r, 32000, 48000);
    rs_set_adjust(&r, adjust);
    static int16_t in[500 * 2], out[2048 * 2];
    static double y[400000];
    long t = 0, m = 0;
    while (t < n_in) {
        int n = 500;
        for (int k = 0; k < n; k++, t++) {
            int16_t v = (int16_t)lrint(12000.0 * sin(2 * PI * hz * t / 32000.0));
            in[2 * k] = in[2 * k + 1] = v;
        }
        int got = rs_process(&r, in, n, out, 2048);
        for (int k = 0; k < got && m < 400000; k++)
            y[m++] = out[2 * k];
    }
    *n_out = m;
    /* Least-squares fit of a sin + b cos at the output rate, from 200 on. */
    double w = 2 * PI * hz / (48000.0 * adjust), ss = 0, cc = 0, sc = 0, ys = 0, yc = 0;
    for (long k = 200; k < m; k++) {
        double s = sin(w * k), c = cos(w * k);
        ss += s * s, cc += c * c, sc += s * c, ys += y[k] * s, yc += y[k] * c;
    }
    double det = ss * cc - sc * sc, a = (ys * cc - yc * sc) / det, b = (yc * ss - ys * sc) / det;
    double sig = 0, err = 0;
    for (long k = 200; k < m; k++) {
        double f = a * sin(w * k) + b * cos(w * k);
        sig += f * f;
        err += (y[k] - f) * (y[k] - f);
    }
    return 10 * log10(sig / (err + 1e-9));
}

int main(void)
{
    long n;
    double snr = tone_snr(1000, 1.0, &n, 32000 * 4);
    printf("resample: 1 kHz SNR %.1f dB\n", snr);
    CHECK(snr > 70, "1 kHz tone SNR %.1f dB", snr);
    CHECK(labs(n - 48000 * 4) < 40, "4 s in -> %ld samples out (48000 * 4)", n);
    snr = tone_snr(12000, 1.0, &n, 32000 * 4);
    printf("resample: 12 kHz SNR %.1f dB\n", snr);
    CHECK(snr > 60, "12 kHz tone SNR %.1f dB", snr);
    long n_up;
    tone_snr(1000, 1.005, &n_up, 32000 * 4);
    CHECK(labs(n_up - lround(48000 * 4 * 1.005)) < 40, "+0.5%%: %ld samples", n_up);

    CHECK(fabs(rs_rate_control(0, 2720, 0.005) - 1.005) < 1e-9, "empty queue: +0.5%%");
    CHECK(fabs(rs_rate_control(2720, 2720, 0.005) - 1.0) < 1e-9, "at target: nominal");
    CHECK(fabs(rs_rate_control(100000, 2720, 0.005) - 0.995) < 1e-9, "far over: -0.5%%");
    return th_report("resample");
}
