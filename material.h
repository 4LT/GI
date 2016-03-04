#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "types.h"

struct material;

typedef struct
{
    struct vec3 position;
    struct vec3 normal;
    vfloat_t distance;
    struct material *material;
} intersect_result_t;

typedef color_t (*shader_fp)(intersect_result_t);

typedef struct material
{
    shader_fp shade;
    color_t diffuse_color;
    color_t specular_color;
    float specular_exp;
} Material_t;

#endif
