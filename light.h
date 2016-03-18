#ifndef LIGHT_H_
#define LIGHT_H_

#include "color.h"

typedef struct
{
    struct vec3 position;
    color_t color;
    vfloat_t radius;
} light_t;

#endif
