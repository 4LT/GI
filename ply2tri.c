/* ply2tri.c
 *
 * Draws a standford bunny.
 * author: Seth Rader
 */
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "scene.h"
#include "canvas.h"
#include "rply-1.1.4/rply.h"
#include "shapes.h"
#include "tone_mapping.h"


#define MS_TIMER(stmt) {\
    clock_t time__ = clock();\
    { stmt }\
    time__ = clock() - time__;\
    printf("%f seconds elapsed\n", ((float)time__) / CLOCKS_PER_SEC);\
}

/* screen dimensions in pixels */
static const int SCREEN_W = 640;
static const int SCREEN_H = 480;

static const vec3_t CAM_POS = {{ 0.0, 0.1, 0.25 }};
static const vec3_t CAM_UP = {{ 0.0, 1.0, 0.0 }};
static const vec3_t CAM_LOOK = {{ 0.0, 0.1, 0.0 }};

static const char *const DEFAULT_BUNNY = "bun_zipper_res3.ply";
static const double DEFAULT_REDUNDANCY_LIMIT = 0.125;

static vec3_t g_vert;
static size_t g_vert_index = 0;
static int g_face_element = 0;

/* (Callback) Adds a vertex component (X, Y, or Z) to the vertex list.
 * argument - data from ply encountered by rply
 */
static int on_vertex(p_ply_argument argument)
{
    vec3_t *vlist;
    long offset;

    ply_get_argument_user_data(argument, (void *)&vlist, &offset);
    vfloat_t value = (vfloat_t)ply_get_argument_value(argument);
    g_vert.v[offset] = value;
    if (offset == 2) {
        vlist[g_vert_index++] = g_vert;
    }
    return 1;
}

/* (Callback) Collects vertex indices for each face (triangle). */
static int on_face(p_ply_argument argument)
{
    size_t *index;
    Llist_t *tri_i;
    static int face_element = 0;

    if (face_element++ == 0) 
        return 1;
    else 
        if (face_element >= 4) face_element = 0;

    index = malloc(sizeof(size_t));
    *index = (size_t)ply_get_argument_value(argument);
    ply_get_argument_user_data(argument, (void *)&tri_i, NULL);
    Llist_append(tri_i, index);
    return 1;
}

/* Gets a list of triangles from a ply file.
 * filename - name of ply file
 */
Llist_t *ply2tri(const char *filename)
{
    g_vert_index = 0;
    g_face_element = 0;

    size_t vert_count;
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
        vec3_t *vert = malloc(sizeof(vec3_t));
        *vert = vlist[*(size_t *)node->datum];
        Llist_append(tris, vert);
    }

    Llist_free_all(tri_i, free);
    return tris;
}

/* Reads a ply file and draws it to the screen
 * argv - first element is file name of ply file to draw
 *      - if the second argument is "usetree", enables KD tree optimization
 *      - third argument is the redundancy limit, or the maximum percentage of
 *        shapes that can straddle a dividing plane before being placed in a
 *        leaf node
 */
int main(int argc, char *argv[])
{
    Llist_t *tris;
    bool use_kdtree;
    double redundancy_limit;
    camera_t cam = cam_centered(CAM_POS, CAM_UP, CAM_LOOK, SCREEN_W, SCREEN_H);
    cam_set_projection(&cam, 1.2, 1.0);
    scene_t scene = scene_empty_scene(CLR_BLACK);

    tris = argc > 1 ? ply2tri(argv[1]) : ply2tri(DEFAULT_BUNNY);
    use_kdtree = argc > 2 && strcmp(argv[2], "usetree") == 0;
    redundancy_limit = argc > 3 ? atof(argv[3]) : DEFAULT_REDUNDANCY_LIMIT;

    Material_t *white = lambert_new(&scene, (color_t) {{ .8, .7, .7 }});
    for (Llist_node_t *node = tris->first; node != NULL;) {
        vec3_t tri[3];
        for (int i = 0; i < 3; i++, node = node->next) {
            vec3_t vert = *((vec3_t *)node->datum);
            tri[i] = vert;
        }
        scene_add_shape(&scene, (Shape_t *)triangle_new(white,
                tri[0], tri[1], tri[2], false));
    }

    if (use_kdtree) {
        puts("Building tree...");
        MS_TIMER(scene_gen_kdtree(&scene, redundancy_limit);)
    }
    else {
        puts("No tree");
    }

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
    puts("Rendering...");
    MS_TIMER(scene_render(&scene, &cam, img);)

    pixel_t *pixmap = malloc(pix_count * sizeof(pixel_t));
    tonemap_nop(img, pix_count, pixmap);
    free(img);

    int exit_status = 0;
    exit_status = draw(SCREEN_W, SCREEN_H, pixmap);
    free(pixmap);

    return exit_status;
}
