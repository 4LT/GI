#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "types.h"
#include "light.h"
#include "util/linkedlist.h"

typedef struct intersect_result intersect_result_t;
typedef struct material Material_t;

typedef color_t (*shader_fp) (intersect_result_t, light_t *);
typedef color_t (*sample_fp) (Material_t *, vfloat_t, vfloat_t);

struct material
{
    struct scene *scene;
    shader_fp shade;
    sample_fp diffuse_sample;
    color_t diffuse_color;
    color_t specular_color;
    vfloat_t specular_exp;
};

struct intersect_result
{
    struct vec3 position;
    struct vec3 normal;
    struct vec3 incoming;
    vfloat_t distance;
    Material_t *material;
};

color_t shade(intersect_result_t res, light_t *light);

#endif
