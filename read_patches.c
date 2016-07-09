#include "read_patches.h"
#include "shapes.h"

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

Quad_t *patch_read_quad(FILE* file, scene_t *scene)
{
    vec3_t vert0 = patch_read_vec3(file);
    vec3_t vert1 = patch_read_vec3(file);
    vec3_t vert2 = patch_read_vec3(file);
    vec3_t vert3 = patch_read_vec3(file);
#if 0
    vec3_t normal = patch_read_vec3(file);
    vec3_t my_normal = v3_cross(v3_sub(vert1, vert0), v3_sub(vert2, vert0));
#else
    patch_read_vec3(file);
#endif
    patch_read_color(file); // discard initial exitance
    color_t mtrl_color = patch_read_color(file);
    Material_t *mtrl = fullbright_new(scene, mtrl_color);
#if 0
    if (v3_dot(normal, my_normal) > 0)
        return quad_new(mtrl, vert0, vert1, vert2, vert3, false);
    else
        return quad_new(mtrl, vert0, vert3, vert2, vert1, false);
#endif
    return quad_new(mtrl, vert3, vert2, vert1, vert0, false);
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
        Quad_t *quad = patch_read_quad(f, scene);
        scene_add_shape(scene, (Shape_t *)quad);
    }

    fclose(f);
}


