#include "scene.h"
#include "aabb.h"

const vfloat_t FUDGE_SCALE = 0.001;
const vfloat_t AMBIENT_SCALE = 0.4;
const int MAX_DEPTH = 10;

scene_t scene_empty_scene(color_t sky_color)
{
    Material_t *sky_mtrl = fullbright_new(NULL, sky_color);
    sky_mtrl->ior = 1;
    return (scene_t) {
        ._sky = sky_mtrl,
        .shapes = Llist_new(),
        .lights = Llist_new(),
        .root = NULL };
}

void scene_add_shape(scene_t *scene, const Shape_t *shape)
{
    Llist_append(scene->shapes, (void *)shape);
}

void scene_add_light(scene_t *scene, const light_t *light)
{
    Llist_append(scene->lights, (void *)light);
}

void scene_gen_kdtree(scene_t *scene, double redundancy_limit)
{
    scene->root = kdnode_new_root(scene->shapes, redundancy_limit);
}

void scene_teardown(scene_t *scene)
{
    /* TODO: apply free function?
     * NOTE: implies ownership transfer of shapes and lights to scene/callee,
     * mark appropriate functions 
     * -OR- use arrays instead of linked lists, and dont transfer ownership */
    Llist_free_list(scene->shapes);
    Llist_free_list(scene->lights);
    free(scene->_sky);
}

intersect_result_t intersect(Shape_t *shape, ray_t ray)
{
    return shape->intersect(shape, ray);
}

Shape_t *transform(Shape_t *shape, mat4_t transmat)
{
    return shape->transform(shape, transmat);
}

intersect_result_t scene_kd_intersect(const scene_t *scene, ray_t ray,
        vfloat_t max_dist, KDnode_t *kdn);

intersect_result_t scene_intersect(const scene_t *scene, ray_t ray,
        vfloat_t max_dist)
{
    if (scene->root != NULL)
        return scene_kd_intersect(scene, ray, max_dist, scene->root);

    Llist_t *shapes = scene->shapes;
    vfloat_t cur_dist = max_dist;
    intersect_result_t nearest;
    nearest.distance = max_dist;
    nearest.material = scene->_sky;

    for (Llist_node_t *node = shapes->first; node != NULL;
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

intersect_result_t scene_kd_intersect(const scene_t *scene, ray_t ray,
        vfloat_t max_dist, KDnode_t *kdn)
{
    vfloat_t cur_dist = max_dist;
    intersect_result_t nearest;
    nearest.distance = max_dist;
    nearest.material = scene->_sky;

    if (kdn->leaf_data != NULL) {
        for (int i = 0; i < kdn->shape_count; i++) {
            intersect_result_t r = intersect(kdn->leaf_data[i], ray);
            if (r.distance > 0 && r.distance < cur_dist) {
                nearest = r;
                cur_dist = r.distance;
            }
        }
        return nearest;
    }
    else {
#if 0
        vfloat_t side =  ray.position.v[kdn->plane] - kdn->plane_offset;
        bool hit_plane = ray.direction.v[kdn->plane] * side <= 0;
#endif

        if (hit_aabb(&kdn->front->bbox, ray))
            nearest = scene_kd_intersect(scene, ray, max_dist, kdn->front);
        if (hit_aabb(&kdn->back->bbox, ray)) {
            intersect_result_t test_nearest =
                    scene_kd_intersect(scene, ray, max_dist, kdn->back);
            if (test_nearest.distance < nearest.distance)
                nearest = test_nearest;
        }

        return nearest;
#if 0
        if (side > 0)
            nearest = scene_kd_intersect(scene, ray, max_dist, kdn->front);
        else
            nearest = scene_kd_intersect(scene, ray, max_dist, kdn->back);

        if (hit_plane && nearest.distance >= max_dist) {
            if (side > 0)
                nearest = scene_kd_intersect(scene, ray, max_dist, kdn->back);
            else
                nearest = scene_kd_intersect(scene, ray, max_dist, kdn->front);
        }
#endif
    }
}

/* true - shadow
 * false - illum */
bool shadow_test(intersect_result_t res, light_t *light)
{
    scene_t *scene = res.material->scene;
    vec3_t light_vec = v3_sub(res.position, light->position);
    vfloat_t light_dist = v3_magnitude(light_vec);
    ray_t light_ray = (ray_t){ light->position,
            v3_divide(light_vec, light_dist) };

    intersect_result_t shadow_res = scene_intersect(scene,
            light_ray, MAX_DIST);

    return light_dist > shadow_res.distance + FUDGE_SCALE &&
        shadow_res.material->transmit_scale < 0.5;
}

color_t color_at_rec(const scene_t *scene, ray_t ray, int depth)
{
    color_t out_color = CLR_BLACK;
    vfloat_t max_dist = MAX_DIST;
    color_t sky_color = scene->_sky->diffuse_color;

    /* find nearest intersection */
    intersect_result_t res;
    res = scene_intersect(scene, ray, max_dist);
    res.depth = depth;

    if (res.distance < MAX_DIST)
    {
        out_color = clr_add(out_color, shade_once(res));

        /* for each light, illum */
        for (Llist_node_t *node = scene->lights->first; node != NULL;
                node = node->next)
        {
            light_t *light = (light_t *)(node->datum);
            out_color = clr_add(out_color, shade_light(res, light));
        }
    }
    else {
        out_color = sky_color;
    }

    return out_color;
}

color_t color_at(const scene_t *scene, ray_t ray)
{
    return color_at_rec(scene, ray, MAX_DEPTH);
}

void scene_render(const scene_t *scene, const camera_t *cam, color_t *img)
{
    int w = cam->img_width;
    int h = cam->img_height;
    vec3_t lookVec = v3_sub(cam->lookAt, cam->pos);
    vec3_t plane_right = v3_cross(lookVec, cam->up);
    vec3_t plane_up = v3_cross(plane_right, lookVec);
    plane_right = v3_normalize(plane_right);
    plane_up = v3_normalize(plane_up);
    lookVec = v3_normalize(lookVec);

    vfloat_t plane_height = cam->plane_width * (vfloat_t)h / w;
    vfloat_t dx = cam->plane_width / w;
    vfloat_t dy = plane_height / h;

    for (int r = 0; r < h; r++) {
        vfloat_t plane_y = (r - (signed)h/2 + cam->y_off) * dy - dy/2;
        for (int c = 0; c < w; c++) {
            vfloat_t plane_x = (c - (signed)w/2 + cam->x_off) * dx - dx/2;
            vec3_t plane_world = v3_add(
                    v3_add(
                        v3_scale(plane_right, plane_x),
                        v3_scale(plane_up, plane_y)),
                    v3_scale(lookVec, cam->focal_length));
            ray_t ray;
            ray.position = cam->pos;
            ray.direction = v3_normalize(plane_world);

            img[r*w + c] = color_at(scene, ray);
        }
    }
}
