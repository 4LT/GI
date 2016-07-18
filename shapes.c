#include <stdio.h>
#include <tgmath.h>
#include <stdlib.h>
#include "shapes.h"
#include "aabb.h"
#include "util/ops.h"

intersect_result_t intersect_shape(Shape_t *shape, ray_t ray)
{
    return shape->intersect(shape, ray);
}

intersect_result_t sphere_intersect(Shape_t *shape, ray_t ray)
{
    Sphere_t *sphere = (Sphere_t *)shape;
    Material_t *mtrl = shape->material;
    vec3_t displacedPos = v3_sub(shape->position, ray.position);
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

    vec3_t intersect_point = v3_add(
            v3_scale(ray.direction, dist), ray.position);
    vec3_t normal = v3_normalize(
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

Shape_t *sphere_transform(Shape_t *shape, mat4_t trans_mat)
{
    Sphere_t *sphere = (Sphere_t *)shape;
    shape->position = m4v3_transform(trans_mat, shape->position);
    return (Shape_t *)sphere;
}

intersect_result_t triangle_intersect(Shape_t *shape, ray_t ray)
{
    Triangle_t *tri = (Triangle_t *)shape;
    vec3_t e1 = v3_sub(tri->verts[1], tri->verts[0]);
    vec3_t e2 = v3_sub(tri->verts[2], tri->verts[0]);
    vec3_t normal = v3_normalize(v3_cross(e1, e2));

    if (!shape->draw_backface && v3_dot(normal, ray.direction) > 0)
        return MISS;

    vec3_t T = v3_sub(ray.position, tri->verts[0]);
    vec3_t P = v3_cross(ray.direction, e2);
    vec3_t Q = v3_cross(T, e1);
    vec3_t barycoords = v3_divide((vec3_t) {{
            v3_dot(Q, e2), v3_dot(P, T), v3_dot(Q, ray.direction) }},
            v3_dot(P, e1));

    vfloat_t u = barycoords.v[1], v = barycoords.v[2];
    if (u < 0 || v < 0 || u+v > 1) {
        return MISS;
    }
    else {
        vec3_t intersect = v3_add(v3_scale(tri->verts[0], 1-u-v),
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

Shape_t *triangle_transform(Shape_t *shape, mat4_t trans_mat)
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

Shape_t *quad_transform(Shape_t *shape, mat4_t trans_mat)
{
    Quad_t *quad = (Quad_t *)shape;
    triangle_transform((Shape_t *)quad->t1, trans_mat);
    triangle_transform((Shape_t *)quad->t2, trans_mat);
    return shape;
}

Sphere_t *sphere_new(Material_t *mtrl, vfloat_t radius,
            vec3_t position)
{
    Sphere_t *sphere = malloc(sizeof(Sphere_t));
    sphere->base = (Shape_t) {
        .intersect = sphere_intersect,
        .transform = sphere_transform,
        .bbox = init_aabb(
            v3_sub(position, (vec3_t) {{ radius, radius, radius }}),
            v3_add(position, (vec3_t) {{ radius, radius, radius }})),
        .position = position,
        .material = mtrl,
        .draw_backface = true };
    sphere->radius = radius;
    return sphere;
}

Triangle_t *triangle_new(Material_t *mtrl, vec3_t vert0, vec3_t vert1,
        vec3_t vert2, bool draw_backface)
{
    Triangle_t *tri = malloc(sizeof(Triangle_t));
    tri->verts[0] = vert0;
    tri->verts[1] = vert1;
    tri->verts[2] = vert2;

#if 0
    vfloat_t bounds[3][2];
    for (int i = 0; i < 3; i++) {
        bounds[i][0] = tri->verts[0].v[i];
        bounds[i][1] = tri->verts[0].v[i];

        for (int vert_i = 1; vert_i < 3; vert_i++) {
            bounds[i][0] = ME_MIN(bounds[i][0], tri->verts[vert_i].v[i]);
            bounds[i][1] = ME_MAX(bounds[i][1], tri->verts[vert_i].v[i]);
        }
    }
#endif

    Shape_t shape = (Shape_t) {
        .intersect = triangle_intersect,
        .transform = triangle_transform,
        .bbox = init_aabb(vert0, vert1),
        .position = v3_divide(v3_add(v3_add(vert0, vert1), vert2), 3.0),
        .material = mtrl,
        .draw_backface = draw_backface };

    grow_aabb_by_vertex(&shape.bbox, vert2);
    
    tri->base = shape;
    return tri;
}

Quad_t *quad_new(Material_t *mtrl, vec3_t vert0, vec3_t vert1,
        vec3_t vert2, vec3_t vert3, bool draw_backface)
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
        .bbox = t1s->bbox,
        .position = v3_divide(
                v3_add(t1s->position, t2s->position), 2),
        .material = mtrl,
        .draw_backface = draw_backface };

    grow_aabb_by_aabb(&qshape.bbox, &t2s->bbox);

    quad->base = qshape;
    return quad;
}

