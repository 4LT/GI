#include <stdlib.h>
#include "scene.h"
#include "canvas.h"
#include "tone_mapping.h"

static const int SCREEN_W = 512;
static const int SCREEN_H = 512;

#if 1
    #define CAM_POS {{ 278, 273, -800 }}
#else
    #define CAM_POS {{ 278, 273, 1359.2 }}
#endif

#define CAM_UP {{ 0, 1, 0 }}
#define CAM_LOOK {{ 278, 273, 0 }}

static const camera_t CAM = { CAM_POS, CAM_UP, CAM_LOOK, 0.035, .05 };

int main(int argc, char *argv[])
{
    scene_t scene = scene_empty_scene(CLR_BLACK, CAM);

    vec3_t room_coords[] = {
        {{ 552.8, 0.0, 0.0 }},
        {{ 0.0, 0.0, 0.0 }},
        {{ 0.0, 0.0, 559.2 }},
        {{ 549.6, 0.0, 559.2 }},

        {{ 556.0, 548.8, 0.0 }},
        {{ 556.0, 548.8, 559.2 }},
        {{ 0.0, 548.8, 559.2 }},
        {{ 0.0, 548.8, 0.0 }}
    };

    Material_t *white = fullbright_new(&scene, (color_t) {{ 0.4, 0.4, 0.4 }});
    Material_t *green = fullbright_new(&scene, (color_t) {{ 0.0, 0.5, 0.0 }});
    Material_t *red = fullbright_new(&scene, (color_t) {{ 0.5, 0.0, 0.0 }});
    Material_t *mag = fullbright_new(&scene, (color_t) {{ .4, 0, .5 }});
#if 0
    Material_t *box_white =
        fullbright_new(&scene, (color_t) {{ 0.6, 0.6, 0.6 }});
#endif

    scene_add_shape(&scene, (Shape_t *)quad_new(white, room_coords[0],
            room_coords[1], room_coords[2], room_coords[3], false));
    scene_add_shape(&scene, (Shape_t *)quad_new(white, room_coords[4],
            room_coords[5], room_coords[6], room_coords[7], false));
    scene_add_shape(&scene, (Shape_t *)quad_new(white, room_coords[6],
            room_coords[5], room_coords[3], room_coords[2], false));
    scene_add_shape(&scene, (Shape_t *)quad_new(green, room_coords[7],
            room_coords[6], room_coords[2], room_coords[1], false));
    scene_add_shape(&scene, (Shape_t *)quad_new(red, room_coords[0],
            room_coords[3], room_coords[5], room_coords[4], false));
    scene_add_shape(&scene, (Shape_t *)quad_new(mag, room_coords[0],
            room_coords[4], room_coords[7], room_coords[1], false));

    light_t ambient = (light_t){ .type = AMBIENT, .color = CLR_BLACK };
    scene_add_light(&scene, &ambient);

    size_t pix_count = SCREEN_W * SCREEN_H;
    color_t *img = malloc(pix_count * sizeof(color_t));
    scene_render(&scene, SCREEN_W, SCREEN_H, img);

    pixel_t *pixmap = malloc(pix_count * sizeof(color_t));
    tonemap_nop(img, pix_count, pixmap);
    free(img);

    int exit_status = draw(SCREEN_W, SCREEN_H, pixmap);
    free(pixmap);

    return exit_status;
}
