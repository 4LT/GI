#ifndef LIGHT_H_
#define LIGHT_H_

#include "color.h"

typedef struct
{
    enum
    {
        SPHERE,
        ORTHO,
        AMBIENT
    } type;
    struct vec3 position;
    struct vec3 direction;
    color_t color;
    vfloat_t radius;
} light_t;

#endif
