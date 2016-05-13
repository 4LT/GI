#ifndef TONE_MAPPING_H_
#define TONE_MAPPING_H_

#include <stdlib.h>
#include "color.h"

void tonemap_ward(color_t *img, size_t pix_count, pixel_t *pixmap, double l_max);

void tonemap_nop(color_t *img, size_t pix_count, pixel_t *pixmap);

#endif
