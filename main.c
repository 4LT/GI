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

    color_t spec1 = (color_t) {{ .002, .002, .002 }};
    color_t spec2 = (color_t) {{ .002, .002, .002 }};
    Material_t *shiny = shiny_new(&scene, (color_t) {{ 0.3, 0.3, 0.3 }},
            spec1, 64, 1);
    Material_t *phong_blue = phong_new(&scene, (color_t) {{ 0, 0, 0.5 }},
            spec2, 8);

    Material_t *tiled = tile_new(&scene);
    light_t light1 = (light_t) { SPHERE, (struct vec3) {{ 65, -80, 128 }},
            v3_normalize((struct vec3) {{ 0, 0, -1 }} ),
            (color_t) {{ 200, 200, 200 }}, 6 };

    scene_add_light(scene, &light1);

    scene_add_shape(scene, 
            (Shape_t *)sphere_new(phong_blue, 24,
            (struct vec3){{ 55, -20, 30 }}));
    scene_add_shape(scene,
            (Shape_t *) sphere_new(shiny, 18,
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
