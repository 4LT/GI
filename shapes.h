#ifndef SHAPES_H_
#define SHAPES_H_

#include "vecmatops.h"
#include "material.h"
#include "aabb.h"

#define MISS ((intersect_result_t) {\
        .position = {{ 0, 0, 0 }},\
        .normal = {{ 0, 0, 0, }},\
        .incoming = {{ 0, 0, 0 }}, \
        .distance = 0,\
        .material = NULL })

struct shape;

typedef intersect_result_t (*intersect_fp)(struct shape *, ray_t);

typedef struct shape * (*transform_fp)(struct shape *, mat4_t);

typedef struct shape
{
    intersect_fp intersect;
    transform_fp transform;
    aabb_t bbox;
    vec3_t position;
    Material_t *material;
    bool draw_backface;
} Shape_t;

typedef struct
{
    Shape_t base;
    vfloat_t radius;
} Sphere_t;

typedef struct
{
    Shape_t base;
    vec3_t verts[3];
} Triangle_t;

typedef struct
{
    Shape_t base;
    /* TODO: possibly create quad intersect function, use 4 coords instead of
     * 2 triangles (non-planar) */
    Triangle_t *t1;
    Triangle_t *t2;
} Quad_t;

intersect_result_t intersect_shape(Shape_t *shape, ray_t ray);

Shape_t *transform(Shape_t *shape, mat4_t transMat);

color_t shade(intersect_result_t intrs_result, light_t *light);

Sphere_t *sphere_new(Material_t *mtrl, vfloat_t radius, vec3_t position);

Triangle_t *triangle_new(Material_t *mtrl, vec3_t vert0, vec3_t vert1,
        vec3_t vert2, bool draw_backface);

Quad_t *quad_new(Material_t *mtrl, vec3_t vert0, vec3_t vert1,
        vec3_t vert2, vec3_t vert3, bool draw_backface);

#endif
