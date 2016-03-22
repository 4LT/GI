#ifndef SHAPES_H_
#define SHAPES_H_

#include "vecmatops.h"
#include "material.h"

struct shape;

typedef intersect_result_t (*intersect_fp)(struct shape *, ray_t);

typedef struct shape * (*transform_fp)(struct shape *, struct mat4);

typedef struct shape
{
    intersect_fp intersect;
    transform_fp transform;
    Material_t *material;
} Shape_t;

typedef struct
{
    Shape_t base;
    vfloat_t radius;
    struct vec3 position;
} Sphere_t;

typedef struct
{
    Shape_t base;
    struct vec3 verts[3];
} Triangle_t;
    
intersect_result_t intersect_shape(Shape_t *shape, ray_t ray);

Shape_t *transform(Shape_t *shape, struct mat4 transMat);

color_t shade(intersect_result_t intrs_result, Light_t *light);

Sphere_t *sphere_new(Material_t *mtrl,
        unsigned int radius, struct vec3 position);

Triangle_t *triangle_new(Material_t *mtrl, struct vec3 vert0, struct vec3 vert1,
        struct vec3 vert2);
#endif
