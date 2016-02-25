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
    struct vec3 vertices[3];
} Triangle_t;

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
    color_t color = sphere->base.color;
    intersectResult_t miss = (intersectResult_t) { 0, color };
    struct vec3 displacedPos = v3_sub(sphere->position, ray.position);
    vfloat_t a = v3_dot(ray.direction, ray.direction);
    vfloat_t b = 2 * v3_dot(displacedPos, ray.direction);
    vfloat_t c = v3_dot(displacedPos, displacedPos) -
        sphere->radius * sphere->radius;

    vfloat_t p = b*b - 4*a*c;
    if (p < 0) {
        printf("total miss\n");
        return miss;
    }

    p = sqrt(p);
    vfloat_t q = (-b + p)/2;
    vfloat_t r = (-b - p)/2;

    if (q <= 0 && r <= 0) {
        printf("back miss\n");
        return miss;
    }
    else if (q <= 0) {
        printf("1 hit\n");
        return (intersectResult_t) { r, color };
    }
    else if (r <= 0) {
        printf("1 hit\n");
        return (intersectResult_t) { q, color };
    }
    else {
        printf("2 hits\n");
        return (intersectResult_t) { q < r ? q : r, color };
    }
}

Shape_t *sphere_transform(Shape_t *shape, struct mat4 transMat)
{
    Sphere_t *sphere = (Sphere_t *)shape;
    sphere->position = m4v3_transform(transMat, sphere->position);
    return (Shape_t *)sphere;
}

/*intersectResult_t triangle_intersect(Triangle_t *triangles, ray_t ray)
{
    vfloat_v origin = triangles->vertices[0];
    struct vec3 u = v3_sub(triangles->vertices[1], origin);
    struct vec3 v = v3_sub(triangles->vertices[2], origin);
    vfloat_t uMag = v3_magnitude(u);
    vfloat_t vMag = v3_magnitude(v);
    u = v3_div(u, uMag);
    v = v3_div(v, vMag);
    struct vec3 normal = v3_cross(u, v);

    vfloat_t k = v3_dot(triangles->vertices[0], normal);
}*/

Sphere_t *sphere_new(color_t color, unsigned int radius, struct vec3 position)
{
    Sphere_t *sphere = malloc(sizeof(Sphere_t));
    sphere->base = (Shape_t) { sphere_intersect, sphere_transform, color };
    sphere->radius = radius;
    sphere->position = position;
    return sphere;
}

#endif
