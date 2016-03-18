#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "types.h"
#include "color.h"
#include "util/linkedlist.h"

struct material;

typedef struct
{
    ray_t ray;
    struct vec3 position;
    struct vec3 normal;
    vfloat_t distance;
    struct material *material;
} intersect_result_t;

typedef color_t (*shader_fp)(intersect_result_t, llist_t *, llist_t *);

typedef struct material
{
    shader_fp shade;
    color_t diffuse_color;
    color_t specular_color;
    float specular_exp;
} Material_t;

color_t shade(intersect_result_t res, llist_t *shapes, llist_t *lights);

#endif
