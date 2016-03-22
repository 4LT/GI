#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "types.h"
#include "light.h"
#include "util/linkedlist.h"

typedef struct intersect_result intersect_result_t;

typedef color_t (*shader_fp) (struct intersect_result, Light_t *);

typedef struct
{
    shader_fp shade;
    color_t diffuse_color;
    color_t specular_color;
    float specular_exp;
} Material_t;

struct intersect_result
{
    struct vec3 position;
    struct vec3 normal;
    struct vec3 incoming;
    vfloat_t distance;
    Material_t *material;
};

color_t shade(intersect_result_t res, Light_t *light);

#endif
