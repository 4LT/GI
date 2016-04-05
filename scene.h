#ifndef SCENE_H_
#define SCENE_H_

#include <stdbool.h>
#include "vecmatops.h"
#include "shapes.h"
#include "util/linkedlist.h"
#include "materials.h"
#include "light.h"
#include "types.h"
#include "color.h"

static const vfloat_t PLANE_WIDTH = 1.2;
static const vfloat_t PLANE_DIST = 1.0;
static const vfloat_t MAX_DIST = 5000;

typedef struct
{
    struct vec3 pos;
    struct vec3 up;
    struct vec3 lookAt;
} camera_t;

typedef struct scene
{
    Material_t *_sky;
    camera_t camera;
    llist_t *shapes;
    llist_t *lights;
} scene_t;

bool shadow_test(intersect_result_t res, light_t *light);

scene_t scene_empty_scene(color_t sky_color, camera_t camera);

void scene_add_shape(scene_t scene, Shape_t *shape);

void scene_add_light(scene_t scene, light_t *light);

void scene_teardown(scene_t scene);

color_t color_at(scene_t scene, ray_t ray);

pixel_t pixel_at(scene_t scene, ray_t ray);

void scene_render(scene_t scene, unsigned int w, unsigned int h,
        pixel_t *pixmap);
#endif
