#include "read_patches.h"

#include <shapes.h>
#include <scene.h>
#include "patch.h"

typedef struct
{
    vec3_t vert0;
    vec3_t vert1;
    vec3_t vert2;
    vec3_t vert3;
} partial_quad_t;

static inline Quad_t *finish_new_quad(Material_t *mtrl,
        partial_quad_t part_quad, bool draw_backface)
{
    return quad_new(mtrl, part_quad.vert0, part_quad.vert1, part_quad.vert2,
            part_quad.vert3, draw_backface);
}

vec3_t patch_read_vec3(FILE* file)
{
    vec3_t vec;
    fscanf(file, "%lf %lf %lf ", &vec.v[0], &vec.v[1], &vec.v[2]);
    return vec;
}

color_t patch_read_color(FILE* file)
{
    color_t color;
    fscanf(file, "%f %f %f ", &color.c[0], &color.c[1], &color.c[2]);
    return color;
}

partial_quad_t patch_read_quad(FILE* file)
{
    vec3_t vert0 = patch_read_vec3(file);
    vec3_t vert1 = patch_read_vec3(file);
    vec3_t vert2 = patch_read_vec3(file);
    vec3_t vert3 = patch_read_vec3(file);
    vec3_t normal = patch_read_vec3(file);
    /* subtract y to Fix left-hand rule nonsense */
    if (normal.v[0] - normal.v[1] + normal.v[2] > 0)
        return (partial_quad_t) { vert0, vert1, vert2, vert3 };
    else
        return (partial_quad_t) { vert3, vert2, vert1, vert0 };
}

void patch_read_file(const char *file_name, scene_t *scene, camera_t *cam)
{
    FILE *f = fopen(file_name, "r");
    vec3_t cam_pos = patch_read_vec3(f);
    vec3_t cam_look = patch_read_vec3(f);
    vec3_t cam_up = patch_read_vec3(f);

    int width, height, discard;
    fscanf(f, "%d", &width);
    fscanf(f, "%d", &height);
    for (int i = 0; i < 3; i++)
        fscanf(f, "%d ", &discard);

    *scene = scene_empty_scene(CLR_BLACK);
    *cam = cam_centered(cam_pos, cam_up, cam_look, width, height);
    cam_set_projection(cam, 0.035, 0.05);

    int last_ch;
    while ((last_ch = getc(f)) != EOF) {
        ungetc(last_ch, f);
        partial_quad_t part_quad = patch_read_quad(f);
        patch_read_color(f); /* ignore exitance */
        color_t color = patch_read_color(f);
        Material_t *mtrl = fullbright_new(scene, color);
        Quad_t *quad = finish_new_quad(mtrl, part_quad, false);
        scene_add_shape(scene, (Shape_t *)quad);
    }

    fclose(f);
}

#if 0
void patch_read_to_set(const char* file_name, PatchSet_t *pset)
{
    FILE *f = fopen(file_name, "r");
    vec3_t cam_pos = patch_read_vec3(f);
    vec3_t cam_look = patch_read_vec3(f);
    vec3_t cam_up = patch_read_vec3(f);
    int width, height, hcube_w, hcube_h, max_iter;
    fscanf(f, "%d", &width); /* ignore */
    fscanf(f, "%d", &height); /* ignore */
    fscanf(f, "%d", &hcube_w); /* TODO: use  (statically defined) */
    fscanf(f, "%d", &hcube_h); /* TODO: use (statically defined) */
    fscanf(f, "%d", &max_iter); /* TODO: use (using threshhold) */

    int last_ch;
    while ((last_ch = getc(f)) != EOF) {
        ungetc(last_ch, f);
        Quad_t *quad = patch_read_quad(f, scene);
        vec3_t normal = patch_read_vec3(f);
        color_t initial_exitance = patch_read_color(f);
        color_t reflectance = patch_read_color(f);

        PatchSet_add_patch(pset, quad, normal, initial_exitance, reflectance);
    }

    fclose(f);
}
#endif

