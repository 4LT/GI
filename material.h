#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <stdlib.h>
#include "types.h"
#include "light.h"
#include "util/linked_list.h"

typedef struct intersect_result intersect_result_t;
typedef struct material Material_t;

typedef color_t (*shade_per_light_fp) (intersect_result_t, light_t *);
typedef color_t (*shade_once_fp) (intersect_result_t);
typedef color_t (*sample_fp) (Material_t *, vfloat_t, vfloat_t);

struct material
{
    struct scene *scene;
    shade_per_light_fp shade_per_light;
    shade_once_fp shade_once;
    sample_fp diffuse_sample;
    color_t diffuse_color;
    color_t specular_color;
    float specular_exp;
    float reflect_scale;
    float transmit_scale;
    float roughness;
    int reflect_ray_count;
    float ior;
};

struct intersect_result
{
    struct vec3 position;
    struct vec3 normal;
    struct vec3 incoming;
    vfloat_t distance;
    int depth;
    bool exit;
    Material_t *material;
};

color_t shade_nop(intersect_result_t res);
color_t shade_nop_light(intersect_result_t r, light_t *l);
color_t solid_sample(Material_t *mtrl, vfloat_t x, vfloat_t y);

static const Material_t DEF_MTRL = (Material_t)
{
    .scene = NULL,
    .shade_per_light = shade_nop_light,
    .shade_once = shade_nop,
    .diffuse_sample = solid_sample,
    .diffuse_color = CLR_WHITE,
    .specular_color = CLR_BLACK,
    .specular_exp = 1,
    .reflect_scale = 0,
    .transmit_scale = 0,
    .roughness = 0,
    .reflect_ray_count = 1,
    .ior = 1
};

color_t shade_light(intersect_result_t res, light_t *light);
color_t shade_once(intersect_result_t res);

#endif
