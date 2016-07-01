/* scene.h
 *
 * Scenes contain a camera, sky, objects, and light sources, and can output
 * images.
 * author: Seth Rader
 */

#ifndef SCENE_H_
#define SCENE_H_

#include <stdbool.h>
#include "vecmatops.h"
#include "shapes.h"
#include "util/linked_list.h"
#include "materials.h"
#include "light.h"
#include "types.h"
#include "color.h"
#include "kd.h"

/* maximum draw distance */
static const vfloat_t MAX_DIST = 5000;

/* camera data */
typedef struct
{
    /* orientation of camera */
    vec3_t pos;
    vec3_t up;
    vec3_t lookAt;

    /* properties for projection onto a plane */
    vfloat_t plane_width;
    vfloat_t focal_length; 
} camera_t;

typedef struct scene
{
    Material_t *_sky;
    camera_t camera;
    Llist_t *shapes;
    Llist_t *lights;
    KDnode_t *root;
} scene_t;

/* Tests if an object blocks a ray cast from a light source.
 *
 * res - collistion data from point we would like to cast light onto
 * light - light source ray originates from
 * returns: true on light ray collision, false otherwise
 */
bool shadow_test(intersect_result_t res, light_t *light);

/* Creates an empty scene with no lights or objects.
 *
 * sky_color - color of sky, used when rays miss all objects
 * camera - image is rendered from this point in the scene
 * returns: an empty scene with a camera and sky
 */
scene_t scene_empty_scene(color_t sky_color, camera_t camera);

/* Adds a physical object to the scene.
 *
 * scene - scene to add object to
 * shape - shape to be added
 *
 * MUTATES scene
 * TODO: use pointer to scene instead for consistency, and to make it clear that
 * the scene is mutable
 */
void scene_add_shape(scene_t *scene, const Shape_t *shape);

/* Adds light source to the scene.
 *
 * scene - scene to add object to
 * shape - shape to be added
 *
 * MUTATES scene
 * TODO: use pointer to scene instead for consistency, and to make it clear that
 * the scene is mutable
 */
void scene_add_light(scene_t *scene, const light_t *light);

/* Generates a KD tree using the scene's shapes
 */
void scene_gen_kdtree(scene_t *scene);

/* Frees memory allocated in scene.
 *
 * scene - scene to tear down
 */
void scene_teardown(scene_t *scene);

/* Renders image as [0-1] RGB colors to pre-allocated buffer.
 *
 * scene - scene to render
 * w - width of image
 * h - height of image
 * img - buffer to render to
 */
void scene_render(scene_t *scene, size_t w, size_t h,
        color_t *img);

/* Renders image with an offset in pixels. 
 *
 * scene - scene to render
 * w - width of image
 * h - height of image
 * off_x - x offset of image in pixels
 * off_y - y offset of image in pixels
 * img - buffer to render to
 */
void scene_render_offset(scene_t *scene, size_t w, size_t h, int off_x,
        int off_y, color_t *img);

/* Fires a ray to obtain color from direct or indirect collision, or sky color
 * on a miss.
 */
color_t color_at(scene_t *scene, ray_t ray);

/* Fires rays recursively. */
color_t color_at_rec(scene_t *scene, ray_t ray, int depth);

#if 0
void scene_add_kdtree(scene_t *scene, Shape_t *shapes[]);
#endif

#endif
