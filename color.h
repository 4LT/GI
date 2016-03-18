#ifndef COLOR_H_
#define COLOR_H_

#include "types.h"

#define COLOR_COUNT 3

#define CLR_BLACK ((color_t){{0, 0, 0}})
#define CLR_WHITE ((color_t){{1, 1, 1}})

typedef float clrfloat_t;

typedef struct
{
    clrfloat_t c[COLOR_COUNT];
} color_t;

color_t clr_add(color_t clr1, color_t clr2);

color_t clr_mul(color_t clr1, color_t clr2);

color_t clr_clamp(color_t clr);

color_t clr_scale(color_t clr, vfloat_t scale);
#endif
