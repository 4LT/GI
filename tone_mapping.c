#include "tone_mapping.h"

#include <tgmath.h>
#include "util/ops.h"

static const float WARD_A = 1.219;
static const float WARD_B = 0.4;
static const float WARD_C = 2.5;
static const float LD_MAX = 500; /* nits */
#define LUM_SCALE ((color_t){{ 0.27, 0.67, 0.06 }})

static clrfloat_t luminance(color_t in)
{
    clrfloat_t out = 0;
    for (size_t i = 0; i < COLOR_COUNT; i++) {
        out+= in.c[i] * LUM_SCALE.c[i];
    }
    return out;
}

static double ward_op(double lwa)
{
    return pow(
            (WARD_A + pow((LD_MAX/2), WARD_B)) /
            (WARD_A + pow(lwa, WARD_B)),
            WARD_C);
}

static double log_avg_lum(size_t pix_count, color_t *img)
{
    double total_log = 0;
    for (size_t i = 0; i < pix_count; i++)
        total_log+= log(0.0001 + (double)luminance(img[i]));
    return exp(total_log / pix_count);
}

static pixel_t clr2pixel(color_t color)
{
    color.c[0] = color.c[0] > 1 ? 1 : color.c[0];
    color.c[1] = color.c[1] > 1 ? 1 : color.c[1];
    color.c[2] = color.c[2] > 1 ? 1 : color.c[2];
    return (int)(255 * color.c[0]) << 24 |
           (int)(255 * color.c[1]) << 16 |
           (int)(255 * color.c[2]) << 8  | 0xFF;
}

void tonemap_ward(color_t *img, size_t pix_count, pixel_t *pixmap, double l_max)
{
    if (l_max == 0) {
        for (size_t i = 0; i < pix_count; i++) {
            clrfloat_t lum = luminance(img[i]);
            l_max = lum > l_max ? lum : l_max;
        }
    }
    color_t *scaled_img = malloc(pix_count * sizeof(color_t));
    for (size_t i = 0; i < pix_count; i++) {
        scaled_img[i] = clr_scale(img[i], 1/l_max);
    }
    double sf = ward_op(log_avg_lum(pix_count, scaled_img));
    for (size_t i = 0; i < pix_count; i++) {
        pixmap[i] = clr2pixel(
                clr_scale(img[i], (clrfloat_t)(sf * l_max / LD_MAX)));
    }
}

void tonemap_linear(
        color_t *img, size_t pix_count, pixel_t *pixmap, vfloat_t scale)
{
    for (size_t i = 0; i < pix_count; i++)
        pixmap[i] = clr2pixel(clr_scale(img[i], scale));
}

void tonemap_nop(color_t *img, size_t pix_count, pixel_t *pixmap)
{
    tonemap_linear(img, pix_count, pixmap, 1);
}

void tonemap_max(color_t *img, size_t pix_count, pixel_t *pixmap)
{
    vfloat_t max = 0;
    for (size_t i = 0; i < pix_count; i++)
    for (int j = 0; j < COLOR_COUNT; j++) {
        max = ME_MAX(max, img[i].c[j]);
    }

    tonemap_linear(img, pix_count, pixmap, 1/max);
}

