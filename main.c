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

static const camera_t CAM = { CAM_POS, CAM_UP, CAM_LOOK, 1.2, 1.0 };

/*
 * Model[] -> Scene
 * Scene -> Pixels
 */
int main(int argc, char *argv[])
{
    scene_t scene = scene_empty_scene(BG_COLOR, CAM);

    color_t spec1 = (color_t) {{ .002, .002, .002 }};
    color_t spec2 = (color_t) {{ .002, .002, .002 }};

    Material_t *shiny = shiny_new(&scene, (color_t) {{ 0.2, 0.2, 0.2 }},
            spec1, 64, 1, 0, 1);
#if 1
    Material_t *phong_blue = phong_new(&scene, (color_t) {{ 0, 0, 0.5 }},
            spec2, 8);
#else
    Material_t *phong_blue = shiny_new(&scene, (color_t) {{ 0, 0, 0.5 }},
            spec2, 8, 1, 0, 1);
#endif

#if 1
    Material_t *tiled = tile_new(&scene);
#elif 1
    Material_t *tiled = concentric_new(&scene);
#else
    Material_t *tiled = noisy_tile_new(&scene);
#endif

    light_t light1 = (light_t) {
        .type = SPHERE,
        .position = (struct vec3) {{ 65, -80, 128 }},
        .color = (color_t) {{ 200, 200, 200 }},
        .radius = 6
    };
    light_t ambient = (light_t) {
        .type = AMBIENT,
        .color = clr_scale(BG_COLOR, 0.4)
    };

    scene_add_light(scene, &light1);
    scene_add_light(scene, &ambient);

    scene_add_shape(scene, 
            (Shape_t *)sphere_new(phong_blue, 24,
            (struct vec3){{ 55, -20, 30 }}));
    scene_add_shape(scene,
            (Shape_t *) sphere_new(shiny, 18,
#if 1
            (struct vec3){{ 20, 30, 22 }}));
#else
            (struct vec3){{ 15, -30, 22 }}));
#endif
    scene_add_shape(scene,
            (Shape_t *)triangle_new(tiled,
                (struct vec3){{ -100, -220, 0 }},
                (struct vec3){{  100, -220, 0 }},
                (struct vec3){{ -100,  220, 0 }}, false ));
    scene_add_shape(scene,
            (Shape_t *)triangle_new(tiled,
                (struct vec3){{  100, -220, 0 }},
                (struct vec3){{  100,  220, 0 }},
                (struct vec3){{ -100,  220, 0 }}, false ));
#if 0
    scene_add_shape(scene,
            (Shape_t *)quad_new(phong_blue,
                (struct vec3){{ -100, 220, 0 }},
                (struct vec3){{ 100,  220, 0 }},
                (struct vec3){{ 100,  220, 100 }},
                (struct vec3){{ -100, 220, 100 }} ));
#endif

    pixel_t *img = malloc(SCREEN_W * SCREEN_H * sizeof(pixel_t));
    scene_render(scene, SCREEN_W, SCREEN_H, img);

    int exit_status = draw(SCREEN_W, SCREEN_H, img);
    free(img);
    return exit_status;
}
