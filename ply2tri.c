#include <stdlib.h>
#include "scene.h"
#include "canvas.h"
#include "rply-1.1.4/rply.h"
#include "shapes.h"
#include "tone_mapping.h"

static const int SCREEN_W = 640;
static const int SCREEN_H = 480;

#define CAM_POS {{ 0, 0.1, 0.25 }}
#define CAM_UP {{ 0, 1, 0 }}
#define CAM_LOOK {{ 0, 0.1, 0 }}

static const camera_t CAM = { CAM_POS, CAM_UP, CAM_LOOK, 1.2, 1.0 };

static struct vec3 vert;
static size_t vert_index = 0;
static int face_element = 0;

static int on_vertex(p_ply_argument argument)
{
    struct vec3 *vlist;
    long offset;

    ply_get_argument_user_data(argument, (void *)&vlist, &offset);
    vfloat_t value = (vfloat_t)ply_get_argument_value(argument);
    vert.v[offset] = value;
    if (offset == 2) {
        vlist[vert_index++] = vert;
    }
    return 1;
}

static int on_face(p_ply_argument argument)
{
    long *index;
    llist_t *tri_i;
    static int face_element = 0;

    if (face_element++ == 0) 
        return 1;
    else 
        if (face_element >= 4) face_element = 0;

    index = (long *) malloc(sizeof(long));
    *index = (long)ply_get_argument_value(argument);
    ply_get_argument_user_data(argument, (void *)&tri_i, NULL);
    llist_append(tri_i, index);
    return 1;
}

llist_t *ply2tri(const char *filename)
{
    vert_index = 0;
    face_element = 0;

    long vert_count, tri_count;
    struct vec3 *vlist;
    llist_t *tri_i = llist_new();
    llist_t *tris = llist_new();
    printf("opening file \"%s\"\n", filename);
    p_ply ply = ply_open(filename, NULL, 0, NULL);
    ply_read_header(ply);

    vert_count = ply_set_read_cb(ply, "vertex", "x", on_vertex, NULL, 0);
    vlist = malloc(sizeof(struct vec3) * vert_count);
    ply_set_read_cb(ply, "vertex", "y", on_vertex, NULL, 1);
    ply_set_read_cb(ply, "vertex", "z", on_vertex, vlist, 2);

    tri_count =
        ply_set_read_cb(ply, "face", "vertex_indices", on_face, tri_i, 0);

    ply_read(ply);

    for (llist_node_t *node = tri_i->first; node != NULL; node = node->next) {
        struct vec3 *vert = (struct vec3 *) malloc(sizeof(struct vec3));
        *vert = vlist[*(long *)node->datum];
        llist_append(tris, vert);
    }

    llist_free_all(tri_i, free);
    return tris;
}

int main(int argc, char *argv[])
{
    llist_t *tris;
    scene_t scene = scene_empty_scene(CLR_BLACK, CAM);

    if (argc <= 1)
        tris = ply2tri("bun_zipper_res4.ply");
    else
        tris = ply2tri(argv[1]);

    Material_t *white = lambert_new(&scene, (color_t) {{ .8, .7, .7 }});
#if 1
    //int ct = 0;
    for (llist_node_t *node = tris->first; node != NULL;) {
        struct vec3 tri[3];
        for (int i = 0; i < 3; i++, node = node->next) {
            struct vec3 vert = *((struct vec3 *)node->datum);
            printf("%f %f %f\n", vert.v[0], vert.v[1], vert.v[2]);
            tri[i] = vert;
        }
        scene_add_shape(scene, (Shape_t *)triangle_new(white,
                tri[0], tri[1], tri[2], false));
        //if (ct++ > 1000) break;
    }
#endif
/*    scene_add_shape(scene, (Shape_t *)triangle_new(white,
                (struct vec3){{ -10, 0, 0 }},
                (struct vec3){{ 10, 0, 0 }},
                (struct vec3){{ 10, 0, 20 }}, true));
*/
    light_t light1 = (light_t) {
        .type = SPHERE,
        .position = (struct vec3) {{ 65, 80, 128 }},
        .color = (color_t) {{ 200, 200, 200 }},
        .radius = 7
    };
    light_t ambient = (light_t) {
        .type = AMBIENT,
        .color = (color_t) {{ .5, .5, .5 }}
    };
    scene_add_light(scene, &light1);
    scene_add_light(scene, &ambient);

    size_t pix_count = SCREEN_W * SCREEN_H;
    color_t *img = malloc(pix_count * sizeof(color_t));
    scene_render(scene, SCREEN_W, SCREEN_H, img);

    pixel_t *pixmap = malloc(pix_count * sizeof(color_t));
    tonemap_nop(img, pix_count, pixmap);
    free(img);

    int exit_status = 0;
#ifdef NODRAW
    exit_status = draw(SCREEN_W, SCREEN_H, pixmap);
#endif
    free(pixmap);

    return exit_status;
}
