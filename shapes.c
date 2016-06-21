#include <stdio.h>
#include <tgmath.h>
#include <stdlib.h>
#include "shapes.h"
#include "util/ops.h"

intersect_result_t intersect_shape(Shape_t *shape, ray_t ray)
{
    return shape->intersect(shape, ray);
}

intersect_result_t sphere_intersect(Shape_t *shape, ray_t ray)
{
    Sphere_t *sphere = (Sphere_t *)shape;
    Material_t *mtrl = shape->material;
    struct vec3 displacedPos = v3_sub(shape->position, ray.position);
    vfloat_t b = 2 * v3_dot(displacedPos, ray.direction);
    vfloat_t c = v3_dot(displacedPos, displacedPos) -
        sphere->radius * sphere->radius;
    vfloat_t p = b*b - 4*c;
    if (p < 0) {
        return MISS;
    }
    p = sqrt(p);

    vfloat_t q = (b - p)/2;
    vfloat_t r = (b + p)/2;

    bool exit = false;
    vfloat_t dist;
    if (q <= 0 && r <= 0)
        return MISS;
    else if (q <= 0) {
        dist = r;
        exit = true;
    }
    else if (r <= 0) {
        dist = q;
        exit = true;
    }
    else {
        dist = q < r ? q : r;
    }

    struct vec3 intersect_point = v3_add(
            v3_scale(ray.direction, dist), ray.position);
    struct vec3 normal = v3_normalize(
            v3_sub(intersect_point, shape->position));
    if (exit)
        normal = v3_scale(normal, -1);
    
    intersect_result_t res;
    res.position = intersect_point;
    res.normal = normal;
    res.incoming = ray.direction;
    res.distance = dist;
    res.exit = exit;
    res.material = mtrl;
    return res;
}

Shape_t *sphere_transform(Shape_t *shape, struct mat4 trans_mat)
{
    Sphere_t *sphere = (Sphere_t *)shape;
    shape->position = m4v3_transform(trans_mat, shape->position);
    return (Shape_t *)sphere;
}

intersect_result_t triangle_intersect(Shape_t *shape, ray_t ray)
{
    Triangle_t *tri = (Triangle_t *)shape;
    struct vec3 e1 = v3_sub(tri->verts[1], tri->verts[0]);
    struct vec3 e2 = v3_sub(tri->verts[2], tri->verts[0]);
    struct vec3 normal = v3_normalize(v3_cross(e1, e2));

    if (!shape->draw_backface && v3_dot(normal, ray.direction) > 0)
        return MISS;

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
        struct vec3 intersect = v3_add(v3_scale(tri->verts[0], 1-u-v),
                v3_add(v3_scale(tri->verts[1], u), v3_scale(tri->verts[2], v)));
        intersect_result_t res;
        res.position = intersect;
        res.normal = normal;
        res.incoming = ray.direction;
        res.distance = barycoords.v[0];
        res.material = shape->material;
        return res;
    }
}

Shape_t *triangle_transform(Shape_t *shape, struct mat4 trans_mat)
{
    Triangle_t *triangle = (Triangle_t *)shape;
    for (int i = 0; i < 3; i++) {
        triangle->verts[i] = m4v3_transform(trans_mat, triangle->verts[i]);
    }
    return shape;
}

intersect_result_t quad_intersect(Shape_t *shape, ray_t ray)
{
    Quad_t *quad = (Quad_t *)shape;
    /* the lazy solution. . . */
    intersect_result_t res = triangle_intersect((Shape_t *)quad->t1, ray);
    if (res.distance <= 0)
        return triangle_intersect((Shape_t *)quad->t2, ray);
    else
        return res;
}

Shape_t *quad_transform(Shape_t *shape, struct mat4 trans_mat)
{
    Quad_t *quad = (Quad_t *)shape;
    triangle_transform((Shape_t *)quad->t1, trans_mat);
    triangle_transform((Shape_t *)quad->t2, trans_mat);
    return shape;
}

Sphere_t *sphere_new(Material_t *mtrl, vfloat_t radius,
            struct vec3 position)
{
    Sphere_t *sphere = malloc(sizeof(Sphere_t));
    sphere->base = (Shape_t) {
        .intersect = sphere_intersect,
        .transform = sphere_transform,
        .bounds = {{ position.v[0] - radius, position.v[0] + radius },
                   { position.v[1] - radius, position.v[1] + radius },
                   { position.v[2] - radius, position.v[2] + radius }},
        .position = position,
        .material = mtrl,
        .draw_backface = true };
    sphere->radius = radius;
    return sphere;
}

Triangle_t *triangle_new(Material_t *mtrl, struct vec3 vert0, struct vec3 vert1,
        struct vec3 vert2, bool draw_backface)
{
    Triangle_t *tri = malloc(sizeof(Triangle_t));
    tri->verts[0] = vert0;
    tri->verts[1] = vert1;
    tri->verts[2] = vert2;

    vfloat_t bounds[3][2];
    for (int i = 0; i < 3; i++) {
        bounds[i][0] = tri->verts[0].v[i];
        bounds[i][1] = tri->verts[0].v[i];

        for (int vert_i = 1; vert_i < 3; vert_i++) {
            bounds[i][0] = ME_MIN(bounds[i][0], tri->verts[vert_i].v[i]);
            bounds[i][1] = ME_MAX(bounds[i][1], tri->verts[vert_i].v[i]);
        }
    }

    Shape_t shape = (Shape_t) {
        .intersect = triangle_intersect,
        .transform = triangle_transform,
        .bounds = {{0}},
        .position = v3_divide(v3_add(v3_add(vert0, vert1), vert2), 3.0),
        .material = mtrl,
        .draw_backface = draw_backface };

    for (int i = 0; i < 3; i++) {
        shape.bounds[i][0] = tri->verts[0].v[i];
        shape.bounds[i][1] = tri->verts[0].v[i];

        for (int vert_i = 1; vert_i < 3; vert_i++) {
            shape.bounds[i][0] =
                ME_MIN(shape.bounds[i][0], tri->verts[vert_i].v[i]);
            shape.bounds[i][1] =
                ME_MAX(shape.bounds[i][1], tri->verts[vert_i].v[i]);
        }
    }
    
    tri->base = shape;
    return tri;
}

Quad_t *quad_new(Material_t *mtrl, struct vec3 vert0, struct vec3 vert1,
        struct vec3 vert2, struct vec3 vert3, bool draw_backface)
{
    Quad_t *quad = malloc(sizeof(Quad_t));
    Triangle_t *t1 = triangle_new(mtrl, vert0, vert1, vert2, draw_backface);
    Triangle_t *t2 = triangle_new(mtrl, vert2, vert3, vert0, draw_backface);
    quad->t1 = t1;
    quad->t2 = t2;
    Shape_t *t1s = (Shape_t *)t1;
    Shape_t *t2s = (Shape_t *)t2;

    Shape_t qshape = (Shape_t) {
        .intersect = quad_intersect,
        .transform = NULL,
        .bounds = {{0}},
        .position = v3_divide(
                v3_add(t1s->position, t2s->position), 2),
        .material = mtrl,
        .draw_backface = draw_backface };

    for (int r = 0; r < 3; r++) {
        qshape.bounds[r][0] = ME_MIN(t1s->bounds[r][0], t2s->bounds[r][0]);
        qshape.bounds[r][1] = ME_MAX(t1s->bounds[r][1], t2s->bounds[r][1]);
    }

    quad->base = qshape;
    return quad;
}

