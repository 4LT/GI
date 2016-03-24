#include "scene.h"

const vfloat_t FUDGE_SCALE = .001;

scene_t scene_empty_scene(color_t sky_color, camera_t camera)
{
    Material_t *sky_mtrl = malloc(sizeof(Material_t));
    *sky_mtrl = (Material_t) {
        .shade = fullbright_shade,
        .diffuse_color = sky_color,
        .specular_color = CLR_BLACK,
        .specular_exp = 1
    };

    return (scene_t){ sky_mtrl, camera, llist_new(), llist_new() };
}

void scene_add_shape(scene_t scene, Shape_t *shape)
{
    llist_append(scene.shapes, (void *)shape);
}

void scene_add_light(scene_t scene, Light_t *light)
{
    llist_append(scene.lights, (void *)light);
}

void scene_teardown(scene_t scene)
{
    /* TODO: apply free function */
    llist_free_list(scene.shapes);
    llist_free_list(scene.lights);
    free(scene._sky);
}

intersect_result_t intersect(Shape_t *shape, ray_t ray)
{
    return shape->intersect(shape, ray);
}

Shape_t *transform(Shape_t *shape, struct mat4 transmat)
{
    return shape->transform(shape, transmat);
}

pixel_t color2pixel(color_t color)
{
    color.c[0] = color.c[0] > 1 ? 1 : color.c[0];
    color.c[1] = color.c[1] > 1 ? 1 : color.c[1];
    color.c[2] = color.c[2] > 1 ? 1 : color.c[2];
    return (int)(255 * color.c[0]) << 24 |
           (int)(255 * color.c[1]) << 16 |
           (int)(255 * color.c[2]) << 8  | 0xFF;
}

intersect_result_t scene_intersect(llist_t *shapes, ray_t ray,
        vfloat_t max_dist, Material_t *def_material)
{
    vfloat_t cur_dist = max_dist;
    intersect_result_t nearest;
    nearest.distance = max_dist;
    nearest.material = def_material;

    for (llist_node_t *node = shapes->first; node != NULL;
            node = node->next)
    {
        Shape_t *shape = (Shape_t *)(node->datum);
        intersect_result_t r = intersect(shape, ray);
        
        if (r.distance > 0 && r.distance < cur_dist) {
            nearest = r;
            cur_dist = r.distance;
        }
    }

    return nearest;
}

struct vec3 fudge(struct vec3 v, vfloat_t offset)
{
    return v3_add(v, (struct vec3){{ offset, offset, offset }});
}

pixel_t pixel_at(scene_t scene, ray_t ray)
{
    color_t out_color = CLR_BLACK;
    vfloat_t max_dist = MAX_DIST;

    /* find nearest intersection */
    intersect_result_t res = scene_intersect(scene.shapes, ray, max_dist,
            scene._sky);

    if (res.distance < MAX_DIST)
    {
        /* for each light, illum */
        for (llist_node_t *node = scene.lights->first; node != NULL;
                node = node->next)
        {
            Light_t *light = (Light_t *)(node->datum);
            struct vec3 light_vec = v3_sub(res.position, light->position);
            vfloat_t light_dist = v3_magnitude(light_vec);
            ray_t light_ray = (ray_t){ light->position,
                    v3_divide(light_vec, light_dist) };

            intersect_result_t shadow_res = scene_intersect(scene.shapes,
                    light_ray, max_dist, NULL);

            if (shadow_res.distance + FUDGE_SCALE >= light_dist)
            {
                out_color = clr_add(out_color, shade(res, light));
            }
        }
    }
    else {
        out_color = scene._sky->diffuse_color;
    }

    return color2pixel(out_color);
}

void scene_render(scene_t scene, unsigned int w, unsigned int h,
        pixel_t *pixmap)
{
    camera_t cam = scene.camera;
    struct vec3 lookVec = v3_sub(cam.lookAt, cam.pos);
    struct vec3 plane_right = v3_cross(lookVec, cam.up);
    struct vec3 plane_up = v3_cross(plane_right, lookVec);
    plane_right = v3_normalize(plane_right);
    plane_up = v3_normalize(plane_up);
    lookVec = v3_normalize(lookVec);

    vfloat_t plane_height = PLANE_WIDTH * (vfloat_t)h / w;
    vfloat_t dx = PLANE_WIDTH / w;
    vfloat_t dy = plane_height / h;

    for (int r = 0; r < h; r++) {
        vfloat_t plane_y = ((signed)h/2 - r) * dy - dy/2;
        for (int c = 0; c < w; c++) {
            vfloat_t plane_x = (c - (signed)w/2) * dx - dx/2;
            struct vec3 plane_world = v3_add(
                    v3_add(
                        v3_scale(plane_right, plane_x),
                        v3_scale(plane_up, plane_y)),
                    v3_scale(lookVec, PLANE_DIST));
            ray_t ray;
            ray.position = cam.pos;
            ray.direction = v3_normalize(plane_world);

            pixel_t pix = pixel_at(scene, ray);
            pixmap[r*w + c] = pix;
        }
    }
}