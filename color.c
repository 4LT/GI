#include "color.h"

color_t clr_add(color_t clr1, color_t clr2)
{
    color_t out_color;
    for (unsigned int i = 0; i < COLOR_COUNT; i++)
        out_color.c[i] = clr1.c[i] + clr2.c[i];
    return out_color;
}

color_t clr_mul(color_t clr1, color_t clr2)
{
    color_t out_color;
    for (unsigned int i = 0; i < COLOR_COUNT; i++)
        out_color.c[i] = clr1.c[i] * clr2.c[i];
    return out_color;
}

color_t clr_clamp(color_t clr)
{
    for (unsigned int i = 0; i < COLOR_COUNT; i++)
        clr.c[i] = clr.c[i] > 1 ? 1 : clr.c[i];
    return clr;
}

color_t clr_scale(color_t clr, vfloat_t scale)
{
    color_t out_color;
    for (unsigned int i = 0; i < COLOR_COUNT; i++)
        out_color.c[i] = clr.c[i] * (clrfloat_t)scale;
    return out_color;
}

bool clr_eq(color_t clr1, color_t clr2)
{
    for (unsigned int i = 0; i < COLOR_COUNT; i++) {
        if (clr1.c[i] != clr2.c[i])
            return false;
    }
    return true;
}
