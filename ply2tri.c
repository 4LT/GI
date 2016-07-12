/* ply2tri.c
 *
 * Draws a standford bunny.
 * author: Seth Rader
 */
#include <stdlib.h>
#include "scene.h"
#include "canvas.h"
#include "rply-1.1.4/rply.h"
#include "shapes.h"
#include "tone_mapping.h"

/* screen dimensions in pixels */
static const int SCREEN_W = 640;
static const int SCREEN_H = 480;

static const vec3_t CAM_POS = {{ 0.0, 0.1, 0.25 }};
static const vec3_t CAM_UP = {{ 0.0, 1.0, 0.0 }};
static const vec3_t CAM_LOOK = {{ 0.0, 0.1, 0.0 }};

static const char *const DEFAULT_BUNNY = "bun_zipper_res3.ply";
static const double DEFAULT_REDUNDANCY_LIMIT = 0.125;

static vec3_t vert;
static size_t vert_index = 0;
static int face_element = 0;

/* (Callback) Adds a vertex component (X, Y, or Z) to the vertex list.
 * argument - data from ply encountered by rply
 */
static int on_vertex(p_ply_argument argument)
{
    vec3_t *vlist;
    long offset;

    ply_get_argument_user_data(argument, (void *)&vlist, &offset);
    vfloat_t value = (vfloat_t)ply_get_argument_value(argument);
    vert.v[offset] = value;
    if (offset == 2) {
        vlist[vert_index++] = vert;
    }
    return 1;
}

/* (Callback) Collects vertex indices for each face (triangle). */
static int on_face(p_ply_argument argument)
{
    long *index;
    Llist_t *tri_i;
    static int face_element = 0;

    if (face_element++ == 0) 
        return 1;
    else 
        if (face_element >= 4) face_element = 0;

    index = (long *) malloc(sizeof(long));
    *index = (long)ply_get_argument_value(argument);
    ply_get_argument_user_data(argument, (void *)&tri_i, NULL);
    Llist_append(tri_i, index);
    return 1;
}

/* Gets a list of triangles from a ply file.
 * filename - name of ply file
 */
Llist_t *ply2tri(const char *filename)
{
    vert_index = 0;
    face_element = 0;

    long vert_count;
    vec3_t *vlist;
    Llist_t *tri_i = Llist_new();
    Llist_t *tris = Llist_new();
    printf("opening file \"%s\"\n", filename);
    p_ply ply = ply_open(filename, NULL, 0, NULL);
    ply_read_header(ply);

    vert_count = ply_set_read_cb(ply, "vertex", "x", on_vertex, NULL, 0);
    vlist = malloc(sizeof(vec3_t) * vert_count);
    ply_set_read_cb(ply, "vertex", "y", on_vertex, NULL, 1);
    ply_set_read_cb(ply, "vertex", "z", on_vertex, vlist, 2);

#if 0
    tri_count =
#endif
        ply_set_read_cb(ply, "face", "vertex_indices", on_face, tri_i, 0);

    ply_read(ply);

    for (Llist_node_t *node = tri_i->first; node != NULL; node = node->next) {
        vec3_t *vert = (vec3_t *) malloc(sizeof(vec3_t));
        *vert = vlist[*(long *)node->datum];
        Llist_append(tris, vert);
    }

    Llist_free_all(tri_i, free);
    return tris;
}

/* Reads a ply file and draws it to the screen
 * argv - first element is file name of ply file to draw
 */
int main(int argc, char *argv[])
{
    Llist_t *tris;
    double redundancy_limit;
    camera_t cam = cam_centered(CAM_POS, CAM_UP, CAM_LOOK, SCREEN_W, SCREEN_H);
    cam_set_projection(&cam, 1.2, 1.0);
    scene_t scene = scene_empty_scene(CLR_BLACK);

    tris = argc > 1 ? ply2tri(argv[1]) : ply2tri(DEFAULT_BUNNY);
    redundancy_limit = argc > 2 ? atof(argv[2]) : DEFAULT_REDUNDANCY_LIMIT;

    Material_t *white = lambert_new(&scene, (color_t) {{ .8, .7, .7 }});
    //int ct = 0;
    for (Llist_node_t *node = tris->first; node != NULL;) {
        vec3_t tri[3];
        for (int i = 0; i < 3; i++, node = node->next) {
            vec3_t vert = *((vec3_t *)node->datum);
            tri[i] = vert;
        }
        scene_add_shape(&scene, (Shape_t *)triangle_new(white,
                tri[0], tri[1], tri[2], false));
    }

#ifdef KDTREE
    puts("Building tree...");
    scene_gen_kdtree(&scene, redundancy_limit);
    puts("Tree done");
#else
    puts("No tree");
#endif

    light_t light1 = (light_t) {
        .type = SPHERE,
        .position = (vec3_t) {{ 65, 80, 128 }},
        .color = (color_t) {{ 200, 200, 200 }},
        .radius = 7
    };
    light_t ambient = (light_t) {
        .type = AMBIENT,
        .color = (color_t) {{ .5, .5, .5 }}
    };
    scene_add_light(&scene, &light1);
    scene_add_light(&scene, &ambient);

    size_t pix_count = SCREEN_W * SCREEN_H;
    color_t *img = malloc(pix_count * sizeof(color_t));
    scene_render(&scene, &cam, img);

    pixel_t *pixmap = malloc(pix_count * sizeof(pixel_t));
    tonemap_nop(img, pix_count, pixmap);
    free(img);

    int exit_status = 0;
#ifndef NODRAW
    puts("Drawing...");
    exit_status = draw(SCREEN_W, SCREEN_H, pixmap);
#else
    puts("No drawing");
#endif
    free(pixmap);

    return exit_status;
}
