#include <stdio.h>
#include <tgmath.h>
#include <stdlib.h>
#include "shapes.h"

#define MISS ((intersect_result_t) {\
        .position = {{ 0, 0, 0 }},\
        .normal = {{ 0, 0, 0, }},\
        .incoming = {{ 0, 0, 0 }}, \
        .distance = 0,\
        .material = NULL })

intersect_result_t intersect_shape(Shape_t *shape, ray_t ray)
{
    return shape->intersect(shape, ray);
}

intersect_result_t sphere_intersect(Shape_t *shape, ray_t ray)
{
    Sphere_t *sphere = (Sphere_t *)shape;
    Material_t *mtrl = shape->material;
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

    vfloat_t dist;
    if (q <= 0 && r <= 0)
        return MISS;
    else if (q <= 0)
        dist = r;
    else if (r <= 0)
        dist = q;
    else {
        dist = q < r ? q : r;
    }

    struct vec3 intersect_point = v3_add(
            v3_scale(ray.direction, dist), ray.position);
    struct vec3 normal = v3_normalize(
            v3_sub(intersect_point, sphere->position));
    
    return (intersect_result_t) { intersect_point, normal, ray.direction,
            dist, mtrl };
}

Shape_t *sphere_transform(Shape_t *shape, struct mat4 trans_mat)
{
    Sphere_t *sphere = (Sphere_t *)shape;
    sphere->position = m4v3_transform(trans_mat, sphere->position);
    return (Shape_t *)sphere;
}

intersect_result_t triangle_intersect(Shape_t *shape, ray_t ray)
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
    if (u < 0 || v < 0 || u+v > 1) {
        return MISS;
    }
    else {
/*        struct vec3 intersect = v3_add(
            v3_add(v3_scale(e1, u), tri->verts[0]),
            v3_add(v3_scale(e2, v), tri->verts[0])); */
        struct vec3 intersect = v3_add(v3_scale(tri->verts[0], 1-u-v),
                v3_add(v3_scale(tri->verts[1], u), v3_scale(tri->verts[2], v)));
        struct vec3 normal = v3_normalize(v3_cross(e1, e2));
        return (intersect_result_t) { intersect, normal, ray.direction,
                barycoords.v[0], shape->material };
    }
}

Shape_t *triangle_transform(Shape_t *shape, struct mat4 trans_mat)
{
    Triangle_t *triangle = (Triangle_t *)shape;
    for (int i = 0; i < 3; i++) {
        triangle->verts[i] = m4v3_transform(trans_mat, triangle->verts[i]);
    }
    return (Shape_t *)triangle;
}

Sphere_t *sphere_new(Material_t *mtrl, unsigned int radius, struct vec3 position)
{
    Sphere_t *sphere = malloc(sizeof(Sphere_t));
    sphere->base = (Shape_t) { sphere_intersect, sphere_transform, mtrl };
    sphere->radius = radius;
    sphere->position = position;
    return sphere;
}

Triangle_t *triangle_new(Material_t *mtrl, struct vec3 vert0, struct vec3 vert1,
        struct vec3 vert2)
{
    Triangle_t *tri = malloc(sizeof(Triangle_t));
    tri->base = (Shape_t) { triangle_intersect, triangle_transform, mtrl };
    tri->verts[0] = vert0;
    tri->verts[1] = vert1;
    tri->verts[2] = vert2;
    return tri;
}
