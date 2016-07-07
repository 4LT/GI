#include <stdlib.h>
#include "scene.h"
#include "canvas.h"
#include "tone_mapping.h"
#include "hemicube.h"

static const int SCREEN_W = 512;
static const int SCREEN_H = 512;

#if 1
    static const vec3_t CAM_POS = (vec3_t) {{ 278, 273, -800 }};
#else
    static const vec3_t CAM_POS = (vec3_t) {{ 278, 273, 1359.2 }};
#endif

static const vec3_t CAM_UP = (vec3_t) {{ 0, 1, 0 }};
static const vec3_t CAM_LOOK = (vec3_t) {{ 278, 273, 0 }};

int main(int argc, char *argv[])
{
    camera_t cam = cam_centered(CAM_POS, CAM_UP, CAM_LOOK, SCREEN_W, SCREEN_H);
    cam_set_projection(&cam, 0.035, 0.05);

    scene_t scene = scene_empty_scene(CLR_BLACK);

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
    scene_render(&scene, &cam, img);

    pixel_t *pixmap = malloc(pix_count * sizeof(pixel_t));
    tonemap_nop(img, pix_count, pixmap);
    free(img);

    draw(SCREEN_W, SCREEN_H, pixmap);
    free(pixmap);

    color_t *img2 = Hcube_render(&scene,
            (vec3_t){{278, 274, 10}}, (vec3_t){{0, 0, 1}});
    pix_count = (4*256 * 3*256/2);
    pixel_t *pmap2 = malloc(pix_count * sizeof(pixel_t));
    tonemap_nop(img2, pix_count, pmap2);
    free(img2);
    int exit_status = draw(4*256, 3*256/2, pmap2);
    free(pmap2);

    return exit_status;
}
