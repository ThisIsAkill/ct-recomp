#include "resample.h"

#include <math.h>
#include <string.h>

#define PHASES 512
#define HALF   (RS_TAPS / 2)
#define BETA   8.0

/* Kernel table per filter: PHASES+1 rows of RS_TAPS weights, one row per
   fractional position (the last row lets phase interpolation run to 1). */
static float table[PHASES + 1][RS_TAPS];
static float table_cutoff = -1.0f;

static double bessel_i0(double x)
{
    double sum = 1.0, term = 1.0;
    for (int k = 1; k < 50; k++) {
        term *= (x / (2.0 * k)) * (x / (2.0 * k));
        sum += term;
        if (term < 1e-12 * sum)
            break;
    }
    return sum;
}

static void build_table(float cutoff)
{
    const double pi = 3.14159265358979323846;
    double norm = bessel_i0(BETA);
    for (int p = 0; p <= PHASES; p++) {
        double frac = (double)p / PHASES;
        double sum = 0.0;
        for (int t = 0; t < RS_TAPS; t++) {
            double x = (t - (HALF - 1)) - frac;   /* distance from the output point */
            double s = x == 0.0 ? 1.0 : sin(pi * 2.0 * cutoff * x) / (pi * 2.0 * cutoff * x);
            double w = x / HALF;
            double win = fabs(w) >= 1.0 ? 0.0 : bessel_i0(BETA * sqrt(1.0 - w * w)) / norm;
            table[p][t] = (float)(s * win);
            sum += table[p][t];
        }
        for (int t = 0; t < RS_TAPS; t++)   /* unity gain at DC */
            table[p][t] = (float)(table[p][t] / sum);
    }
    table_cutoff = cutoff;
}

void rs_init(resampler *r, double in_hz, double out_hz)
{
    memset(r, 0, sizeof *r);
    r->in_hz = in_hz;
    r->out_hz = out_hz;
    r->adjust = 1.0;
    double lower = in_hz < out_hz ? in_hz : out_hz;
    r->cutoff = (float)(0.95 * 0.5 * lower / in_hz);
    r->len = RS_TAPS;   /* silence as history */
    r->pos = HALF - 1;
    if (table_cutoff != r->cutoff)
        build_table(r->cutoff);
}

void rs_set_adjust(resampler *r, double factor)
{
    r->adjust = factor < 0.99 ? 0.99 : factor > 1.01 ? 1.01 : factor;
}

int rs_process(resampler *r, const int16_t *in, int n, int16_t *out, int max)
{
    if (n > 4096 || r->len + n > 4096 + RS_TAPS)
        return -1;
    if (table_cutoff != r->cutoff)
        build_table(r->cutoff);
    for (int k = 0; k < n; k++) {
        r->buf[0][r->len + k] = in[2 * k];
        r->buf[1][r->len + k] = in[2 * k + 1];
    }
    r->len += n;
    double step = r->in_hz / (r->out_hz * r->adjust);
    int produced = 0;
    /* An output at pos uses inputs floor(pos)-(HALF-1) .. floor(pos)+HALF. */
    while (produced < max && r->pos + HALF < r->len) {
        int base = (int)r->pos;
        double frac = r->pos - base;
        double fp = frac * PHASES;
        int p = (int)fp;
        float mix = (float)(fp - p);
        const float *k0 = table[p], *k1 = table[p + 1];
        const float *l = &r->buf[0][base - (HALF - 1)], *rr = &r->buf[1][base - (HALF - 1)];
        float accl = 0.0f, accr = 0.0f;
        for (int t = 0; t < RS_TAPS; t++) {
            float w = k0[t] + (k1[t] - k0[t]) * mix;
            accl += l[t] * w;
            accr += rr[t] * w;
        }
        long vl = lrintf(accl), vr = lrintf(accr);
        out[2 * produced] = (int16_t)(vl < -32768 ? -32768 : vl > 32767 ? 32767 : vl);
        out[2 * produced + 1] = (int16_t)(vr < -32768 ? -32768 : vr > 32767 ? 32767 : vr);
        produced++;
        r->pos += step;
    }
    /* Keep the history the next output needs. */
    int drop = (int)r->pos - (HALF - 1);
    if (drop > 0) {
        if (drop > r->len)
            drop = r->len;
        memmove(r->buf[0], r->buf[0] + drop, (size_t)(r->len - drop) * sizeof(float));
        memmove(r->buf[1], r->buf[1] + drop, (size_t)(r->len - drop) * sizeof(float));
        r->len -= drop;
        r->pos -= drop;
    }
    return produced;
}

double rs_rate_control(double level, double target, double max_dev)
{
    double d = max_dev * (target - level) / target;
    if (d > max_dev)
        d = max_dev;
    if (d < -max_dev)
        d = -max_dev;
    return 1.0 + d;
}
