/* tone_mapping.h
 *
 * Collection of tone mapping operations.
 * author: Seth Rader
 */
#ifndef TONE_MAPPING_H_
#define TONE_MAPPING_H_

#include <stdlib.h>
#include "color.h"

/* Apply Ward tone mapping operator to image, then convert to pixels */
void tonemap_ward(
        color_t *img, size_t pix_count, pixel_t *pixmap, double l_max);

/* just change colors to pixel values */
void tonemap_nop(color_t *img, size_t pix_count, pixel_t *pixmap);

/* Linearly scale all color values */
void tonemap_linear(
        color_t *img, size_t pix_count, pixel_t *pixmap, vfloat_t scale);

/* Scale color values such that the highest values = 1 */
void tonemap_max(color_t *img, size_t pix_count, pixel_t *pixmap);

#endif
