/* color.h
 *
 * RGB [0, inf] data structure and operations to manipulate colors.
 */
#ifndef COLOR_H_
#define COLOR_H_

#include <stdbool.h>
#include "types.h"

/* 3 colors (RGB), extensible to more wavelength bins for simulating more
 * complex light interactions */
#define COLOR_COUNT 3

#define CLR_BLACK ((color_t){{0, 0, 0}})
#define CLR_WHITE ((color_t){{1, 1, 1}})

/* floating point type used in colors */
typedef float clrfloat_t;

/* value-type, for non-destructive manipulation */
typedef struct
{
    clrfloat_t c[COLOR_COUNT];
} color_t;

/* add corresponding bins of the colors */
color_t clr_add(color_t clr1, color_t clr2);

/* multiply corresponding bins */
color_t clr_mul(color_t clr1, color_t clr2);

/* clamp color to the range [0, 1] */
color_t clr_clamp(color_t clr);

/* scale brightness by "scale" */
color_t clr_scale(color_t clr, vfloat_t scale);

/* equale if all corresponding bins are equal, otherwise false */
bool clr_eq(color_t clr1, color_t clr2);
#endif
