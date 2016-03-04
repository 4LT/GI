#ifndef SCENE_H_
#define SCENE_H_

#include <stdbool.h>
#include "vecmatops.h"
#include "shapes.h"
#include "util/linkedlist.h"
#include "material.h"
#include "materials.h"

typedef struct
{
    struct vec3 pos;
    struct vec3 up;
    struct vec3 lookAt;
} camera_t;

typedef struct
{
    Material_t _sky;
    camera_t camera;
    llist_t *shapes;
    llist_t *lights;
} scene_t;

const vfloat_t PLANE_WIDTH = 1.2;
const vfloat_t PLANE_DIST = 1.0;
const vfloat_t FAR_PLANE = 5000;

color_t default_shader(Material_t *mtrl, intersect_result_t res)
{
    return mtrl->diffuse_color;
}

scene_t scene_emptyScene(color_t sky_color, camera_t camera)
{
    Material_t sky_mtrl = (Material_t) {
        .shade = flat_shade,
        .diffuse_color = sky_color,
        .specular_color = (color_t) { 0, 0, 0 },
        .specular_exp = 1
    };

    return (scene_t){ sky_mtrl, camera, llist_new(), llist_new() };
}

void scene_addShape(scene_t scene, Shape_t *shape)
{
    llist_append(scene.shapes, (void *)shape);
}

void scene_teardown(scene_t scene)
{
    /* TODO: apply free function */
    llist_free_list(scene.shapes);
    llist_free_list(scene.lights);
}

pixel_t color2pixel(color_t color)
{
    return (int)(255 * color.red) << 24 |
           (int)(255 * color.green) << 16 |
           (int)(255 * color.blue) << 8;
}

pixel_t pixel_at(scene_t scene, ray_t ray)
{
    vfloat_t cur_dist = FAR_PLANE;
    intersect_result_t cur_result;
    cur_result.material = &scene._sky;

    for (llist_node_t *node = scene.shapes->first; node != NULL;
            node = node->next)
    {
        Shape_t *shape = (Shape_t *)(node->datum);
        intersect_result_t r = intersect(shape, ray);
        
        if (r.distance > 0 && r.distance < cur_dist) {
            cur_result = r;
            cur_dist = r.distance;
        }
    }
    return color2pixel(shade(cur_result));
}

void scene_render(scene_t scene, unsigned int w, unsigned int h,
        pixel_t *pixmap)
{
    camera_t cam = scene.camera;
    struct vec3 lookVec = v3_sub(cam.lookAt, cam.pos);
    struct vec3 plane_right = v3_cross(lookVec, cam.up);
    struct vec3 plane_up = v3_cross(plane_right, lookVec);
    plane_right = v3_normalize(plane_right);
    plane_up = v3_normalize(plane_up);
    lookVec = v3_normalize(lookVec);

    vfloat_t plane_height = PLANE_WIDTH * (vfloat_t)h / w;
    vfloat_t dx = PLANE_WIDTH / w;
    vfloat_t dy = plane_height / h;

    for (int r = 0; r < h; r++) {
        vfloat_t plane_y = ((signed)h/2 - r) * dy - dy/2;
        for (int c = 0; c < w; c++) {
            vfloat_t plane_x = (c - (signed)w/2) * dx - dx/2;
            struct vec3 plane_world = v3_add(
                    v3_add(
                        v3_scale(plane_right, plane_x),
                        v3_scale(plane_up, plane_y)),
                    v3_scale(lookVec, PLANE_DIST));
            ray_t ray;
            ray.position = cam.pos;
            ray.direction = v3_normalize(plane_world);

            pixel_t pix = pixel_at(scene, ray);
            pixmap[r*w + c] = pix;
#if 0
            color_t cl = (color_t){ (ray.direction.v[0]+1)/2,
                    (ray.direction.v[1]+1)/2,
                    (ray.direction.v[2]+1)/2 };
            pixmap[r*w +c] = color2pixel(cl);
#endif
        }
    }
}
#endif
