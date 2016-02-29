#include <stdlib.h>
#include "renderer.h"
#include "vecmatops.h"
#include "scene.h"

static const color_t BG_COLOR = { 0.4, 0.7, 1.0 };

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
    unsigned int w = 640;
    unsigned int h = 480;

    scene_t scene = scene_emptyScene(BG_COLOR, CAM);
    scene_addShape(scene, 
            (Shape_t *)sphere_new((color_t){ 0, 0, 1 }, 24,
            (struct vec3){{ 55, -20, 30 }}));
    scene_addShape(scene,
            (Shape_t *) sphere_new((color_t){ 0, 0.7, 0 }, 18,
            (struct vec3){{ 20, 30, 22 }}));
    scene_addShape(scene,
            (Shape_t *)triangle_new((color_t){ .7, .3, 0 },
                (struct vec3){{ -100, -220, 0 }},
                (struct vec3){{  100, -220, 0 }},
                (struct vec3){{ -100,  220, 0 }} ));
    scene_addShape(scene,
            (Shape_t *)triangle_new((color_t){ .7, .3, 0 },
                (struct vec3){{  100, -220, 0 }},
                (struct vec3){{  100,  220, 0 }},
                (struct vec3){{ -100,  220, 0 }} ));

    pixel_t img[w * h];
    scene_render(scene, w, h, img);

    int exit_status = draw(w, h, img);
    return exit_status;
}
