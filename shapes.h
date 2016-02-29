#ifndef SHAPES_H_
#define SHAPES_H_

#include <tgmath.h>
#include "vecmatops.h"
#include <stdio.h>

typedef struct
{
    vfloat_t distance;
    color_t color;
} intersectResult_t;

struct shape;

typedef intersectResult_t (*intersect_fp)(struct shape *, ray_t);

typedef struct shape * (*transform_fp)(struct shape *, struct mat4);

typedef struct shape
{
    intersect_fp intersect;
    transform_fp transform;
    color_t color;
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
    
#define MISS ((intersectResult_t) { 0, (color_t) {0, 0, 0} })

intersectResult_t intersect(Shape_t *shape, ray_t ray)
{
    return shape->intersect(shape, ray);
}

Shape_t *transform(Shape_t *shape, struct mat4 transMat)
{
    return shape->transform(shape, transMat);
}

intersectResult_t sphere_intersect(Shape_t *shape, ray_t ray)
{
    Sphere_t *sphere = (Sphere_t *)shape;
    color_t color = shape->color;
    struct vec3 displacedPos = v3_sub(sphere->position, ray.position);
    vfloat_t b = 2 * v3_dot(displacedPos, ray.direction);
    vfloat_t c = v3_dot(displacedPos, displacedPos) -
        sphere->radius * sphere->radius;
    vfloat_t p = b*b - 4*c;
    if (p < 0) {
        return MISS;
    }
    p = sqrt(p);
#if 0
    vfloat_t q = (-b + p)/2;
    vfloat_t r = (-b - p)/2;
#endif
    /*TODO stop-gap fix (why is this backwards?) */
    vfloat_t q = (b - p)/2;
    vfloat_t r = (b + p)/2;

    if (q <= 0 && r <= 0)
        return MISS;
    else if (q <= 0)
        return (intersectResult_t) { r, color };
    else if (r <= 0)
        return (intersectResult_t) { q, color };
    else
        return (intersectResult_t) { q < r ? q : r, color };
}

Shape_t *sphere_transform(Shape_t *shape, struct mat4 trans_mat)
{
    Sphere_t *sphere = (Sphere_t *)shape;
    sphere->position = m4v3_transform(trans_mat, sphere->position);
    return (Shape_t *)sphere;
}

intersectResult_t triangle_intersect(Shape_t *shape, ray_t ray)
{
    Triangle_t *tri = (Triangle_t *)shape;
    struct vec3 e1 = v3_sub(tri->verts[1], tri->verts[0]);
    struct vec3 e2 = v3_sub(tri->verts[2], tri->verts[0]);
    struct vec3 T = v3_sub(ray.position, tri->verts[0]);
    struct vec3 P = v3_cross(ray.direction, e2);
    struct vec3 Q = v3_cross(T, e1);
    struct vec3 barycoords = v3_divide((struct vec3) {{
            v3_dot(Q, e2), v3_dot(P, T), v3_dot(Q, ray.direction) }},
            v3_dot(P, e1));

    vfloat_t u = barycoords.v[1], v = barycoords.v[2];
    if (u < 0 || v < 0 || u+v > 1)
        return MISS;
    else
        return (intersectResult_t) { barycoords.v[0], shape->color };
}

Shape_t *triangle_transform(Shape_t *shape, struct mat4 trans_mat)
{
    Triangle_t *triangle = (Triangle_t *)shape;
    for (int i = 0; i < 3; i++) {
        triangle->verts[i] = m4v3_transform(trans_mat, triangle->verts[i]);
    }
    return (Shape_t *)triangle;
}

Sphere_t *sphere_new(color_t color, unsigned int radius, struct vec3 position)
{
    Sphere_t *sphere = malloc(sizeof(Sphere_t));
    sphere->base = (Shape_t) { sphere_intersect, sphere_transform, color };
    sphere->radius = radius;
    sphere->position = position;
    return sphere;
}

Triangle_t *triangle_new(color_t color, struct vec3 vert0, struct vec3 vert1,
        struct vec3 vert2)
{
    Triangle_t *tri = malloc(sizeof(Triangle_t));
    tri->base = (Shape_t) { triangle_intersect, triangle_transform, color };
    tri->verts[0] = vert0;
    tri->verts[1] = vert1;
    tri->verts[2] = vert2;
    return tri;
}

#endif
