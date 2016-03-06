#ifndef TYPES_H_
#define TYPES_H_
#include <stdint.h>

typedef double vfloat_t;
typedef float clrfloat_t;
typedef int32_t pixel_t;

struct vec3
{
    vfloat_t v[3];
};

struct mat4
{
    vfloat_t m[4][4];
};

typedef struct
{
    clrfloat_t red;
    clrfloat_t green;
    clrfloat_t blue;
} color_t;

typedef struct
{
    struct vec3 position;
    struct vec3 direction;
} ray_t;

#endif
