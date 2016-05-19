#include "scene.h"

const vfloat_t FUDGE_SCALE = 0.001;
const vfloat_t AMBIENT_SCALE = 0.4;
const int MAX_DEPTH = 10;

scene_t scene_empty_scene(color_t sky_color, camera_t camera)
{
    Material_t *sky_mtrl = fullbright_new(NULL, sky_color);
    sky_mtrl->ior = 1;
    return (scene_t) {
        ._sky = sky_mtrl,
        .camera = camera,
        .shapes = llist_new(),
        .lights = llist_new() };
}

void scene_add_shape(scene_t scene, Shape_t *shape)
{
    llist_append(scene.shapes, (void *)shape);
}

void scene_add_light(scene_t scene, light_t *light)
{
    llist_append(scene.lights, (void *)light);
}

void scene_teardown(scene_t scene)
{
    /* TODO: apply free function?
     * NOTE: implies ownership transfer of shapes and lights to scene/callee,
     * mark appropriate functions 
     * -OR- use arrays instead of linked lists, and dont transfer ownership */
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

/* true - shadow
 * false - illum */
bool shadow_test(intersect_result_t res, light_t *light)
{
    scene_t *scene = res.material->scene;
    struct vec3 light_vec = v3_sub(res.position, light->position);
    vfloat_t light_dist = v3_magnitude(light_vec);
    ray_t light_ray = (ray_t){ light->position,
            v3_divide(light_vec, light_dist) };

    intersect_result_t shadow_res = scene_intersect(scene->shapes,
            light_ray, MAX_DIST, NULL);

    return light_dist > shadow_res.distance + FUDGE_SCALE &&
        shadow_res.material->transmit_scale < 0.5;
}

color_t color_at_rec(scene_t scene, ray_t ray, int depth)
{
    color_t out_color = CLR_BLACK;
    vfloat_t max_dist = MAX_DIST;
    color_t sky_color = scene._sky->diffuse_color;

    /* find nearest intersection */
    intersect_result_t res = scene_intersect(scene.shapes, ray, max_dist,
            scene._sky);
    res.depth = depth;

    if (res.distance < MAX_DIST)
    {
        out_color = clr_add(out_color, shade_once(res));

        /* for each light, illum */
        for (llist_node_t *node = scene.lights->first; node != NULL;
                node = node->next)
        {
            light_t *light = (light_t *)(node->datum);
            out_color = clr_add(out_color, shade_light(res, light));
        }
#if 0
        color_t diffuse = res.material->diffuse_sample(
                res.material, res.position.v[0], res.position.v[1]);
        color_t ambient_color = clr_scale(sky_color, AMBIENT_SCALE);
        out_color = clr_add(out_color, clr_mul(diffuse, ambient_color));
#endif
    }
    else {
        out_color = sky_color;
    }

    return out_color;
}

color_t color_at(scene_t scene, ray_t ray)
{
    return color_at_rec(scene, ray, MAX_DEPTH);
}

void scene_render(scene_t scene, size_t w, size_t h,
        color_t *img)
{
    camera_t cam = scene.camera;
    struct vec3 lookVec = v3_sub(cam.lookAt, cam.pos);
    struct vec3 plane_right = v3_cross(lookVec, cam.up);
    struct vec3 plane_up = v3_cross(plane_right, lookVec);
    plane_right = v3_normalize(plane_right);
    plane_up = v3_normalize(plane_up);
    lookVec = v3_normalize(lookVec);

    vfloat_t plane_height = cam.plane_width * (vfloat_t)h / w;
    vfloat_t dx = cam.plane_width / w;
    vfloat_t dy = plane_height / h;

    for (int r = 0; r < h; r++) {
        vfloat_t plane_y = ((signed)h/2 - r) * dy - dy/2;
        for (int c = 0; c < w; c++) {
            vfloat_t plane_x = (c - (signed)w/2) * dx - dx/2;
            struct vec3 plane_world = v3_add(
                    v3_add(
                        v3_scale(plane_right, plane_x),
                        v3_scale(plane_up, plane_y)),
                    v3_scale(lookVec, cam.focal_length));
            ray_t ray;
            ray.position = cam.pos;
            ray.direction = v3_normalize(plane_world);

            img[r*w + c] = color_at(scene, ray);
        }
    }
}
