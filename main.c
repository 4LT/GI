#include <stdlib.h>
#include "canvas.h"
#include "vecmatops.h"
#include "scene.h"

static const color_t BG_COLOR = {{ 0.4, 0.7, 1.0 }};
static const int SCREEN_W = 640;
static const int SCREEN_H = 480;

#define CAM_POS {{ 55, -180, 40 }}
#define CAM_UP {{ 0, 0, 1 }}
#define CAM_LOOK {{ 63, 100, 30 }}

static const camera_t CAM = { CAM_POS, CAM_UP, CAM_LOOK };

/*
 * Model[] -> Scene
 * Scene -> Pixels
 */
int main(int argc, char *argv[])
{
    scene_t scene = scene_empty_scene(BG_COLOR, CAM);

    color_t spec1 = (color_t) {{ .7, .7, .7 }};
    color_t spec2 = (color_t) {{ .5, .5, .5 }};
    Material_t *phong_green = phong_new((color_t) {{ 0, 0.7, 0 }}, spec1, 64);
    Material_t *phong_blue = phong_new((color_t) {{ 0, 0, 0.85 }}, spec2, 16);
#if 0
    Material_t *tiled = lambert_new((color_t) {{ .7, .3, 0 }});
#else
    Material_t *tiled = tile_new();
#endif
    light_t light1 = (light_t) { ORTHO, (struct vec3) {{ 65, -80, 128 }},
            v3_normalize((struct vec3) {{ -0.2, 1, -1.2 }} ),
            (color_t) {{ 0.5, 0.5, 0.5 }}, 2 };

    scene_add_light(scene, &light1);

    scene_add_shape(scene, 
            (Shape_t *)sphere_new(phong_blue, 24,
            (struct vec3){{ 55, -20, 30 }}));
    scene_add_shape(scene,
            (Shape_t *) sphere_new(phong_green, 18,
            (struct vec3){{ 20, 30, 22 }}));
    scene_add_shape(scene,
            (Shape_t *)triangle_new(tiled,
                (struct vec3){{ -100, -220, 0 }},
                (struct vec3){{  100, -220, 0 }},
                (struct vec3){{ -100,  220, 0 }} ));
    scene_add_shape(scene,
            (Shape_t *)triangle_new(tiled,
                (struct vec3){{  100, -220, 0 }},
                (struct vec3){{  100,  220, 0 }},
                (struct vec3){{ -100,  220, 0 }} ));

    pixel_t *img = malloc(SCREEN_W * SCREEN_H * sizeof(pixel_t));
    scene_render(scene, SCREEN_W, SCREEN_H, img);

    int exit_status = draw(SCREEN_W, SCREEN_H, img);
    free(img);
    return exit_status;
}
