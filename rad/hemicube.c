#include "hemicube.h"

#include <string.h>
#include "constants.h"
#include "patchmap.h"

/* debugging */
#include "canvas.h"
#include "tone_mapping.h"

#define RESOLUTION 256
#define HALF_RES (RESOLUTION / 2)

const vfloat_t CELL_W = 2 / (vfloat_t)RESOLUTION;
#define CELL_AREA (CELL_W * CELL_W)


static vfloat_t dff_top[RESOLUTION][RESOLUTION];
static vfloat_t dff_side[RESOLUTION][HALF_RES];

static void debug_hemicube(color_t *top, color_t *sides[]);

static void init_d_formfactors()
{
    for (int xi = 0; xi < RESOLUTION; xi++) {
        vfloat_t x = (vfloat_t)(2*xi) / RESOLUTION - 1 + CELL_W/2;
        for (int yi = 0; yi < RESOLUTION; yi++) {
            vfloat_t y = (vfloat_t)(2*yi) / RESOLUTION - 1 + CELL_W/2;
            vfloat_t denom = (1 + x*x + y*y);
            denom = PI * denom*denom;
            dff_top[xi][yi] = CELL_AREA / denom;
        }
        for (int zi = 0; zi < HALF_RES; zi++) {
            vfloat_t z = (vfloat_t)zi / HALF_RES + CELL_W/2;
            vfloat_t denom = (1 + x*x + z*z);
            denom = PI * denom*denom;
            dff_side[xi][zi] = z * CELL_AREA / denom;
        }
    }
}

static inline camera_t camera_by_dir(vec3_t pos, vec3_t up, vec3_t dir, 
        int w, int h)
{ return cam_centered(pos, up, v3_add(pos, dir), w, h); }

patchff_t *Hcube_get_ffs(const scene_t *scene, Patch_t *this_patch,
        size_t *out_count)
{
    static bool initialized = false;
    if (!initialized) {
        init_d_formfactors();
        initialized = true;
    }

    vec3_t pos = Patch_get_position(this_patch);
    vec3_t local_z = Patch_get_normal(this_patch);
    vec3_t local_x = v3_arbinormal(local_z);
    vec3_t local_y = v3_cross(local_z, local_x);
    PatchMap_t *pmap = PatchMap_new(256);

    camera_t cam_xyp = camera_by_dir(pos, local_y, local_z,
            RESOLUTION, RESOLUTION);

    camera_t cam_yzp = camera_by_dir(pos, local_z, local_x,
            RESOLUTION, HALF_RES);
    cam_set_offset(&cam_yzp, 0, HALF_RES/2);

    camera_t cam_yzn = camera_by_dir(pos, local_z, v3_neg(local_x),
            RESOLUTION, HALF_RES);
    cam_set_offset(&cam_yzn, 0, HALF_RES/2);

    camera_t cam_xzp = camera_by_dir(pos, local_z, local_y,
            RESOLUTION, HALF_RES);
    cam_set_offset(&cam_xzp, 0, HALF_RES/2);

    camera_t cam_xzn = camera_by_dir(pos, local_z, v3_neg(local_y),
            RESOLUTION, HALF_RES);
    cam_set_offset(&cam_xzn, 0, HALF_RES/2);

    color_t *img_top = malloc(RESOLUTION * RESOLUTION * sizeof(color_t));
    scene_render(scene, &cam_xyp, img_top);

    color_t *img_sides[4];
    for (int i = 0; i < 4; i++) {
        img_sides[i] = malloc(RESOLUTION * HALF_RES * sizeof(color_t));
    }

    scene_render(scene, &cam_xzn, img_sides[0]);
    scene_render(scene, &cam_yzn, img_sides[1]);
    scene_render(scene, &cam_xzp, img_sides[2]);
    scene_render(scene, &cam_yzp, img_sides[3]);

#ifndef DEBUG_HEMICUBE
#   define DEBUG_HEMICUBE false
#endif
    if (DEBUG_HEMICUBE) {
        debug_hemicube(img_top, img_sides);
    }

    for (int x = 0; x < RESOLUTION; x++)
    for (int y = 0; y < RESOLUTION; y++) {
        int index = y*RESOLUTION + x;
        Patch_t *target = Patch_by_color(img_top[index]);
        PatchMap_apply_dff(pmap, target, dff_top[x][y]);
        if (y < HALF_RES) {
            for (int s = 0; s < 4; s++) {
                target = Patch_by_color(img_sides[s][index]);
                PatchMap_apply_dff(pmap, target, dff_side[x][y]);
            }
        }
    }

    free(img_top);
    for (int i = 0; i < 4; i++) {
        free(img_sides[i]);
    }

    return PatchMap_extract_and_clear(pmap, out_count);
}

static void debug_hemicube(color_t *top, color_t *sides[])
{
    int w = 4 * RESOLUTION;
    int h = RESOLUTION + HALF_RES;
    color_t *img_composite = calloc(w * h, sizeof(color_t));

    for (int x = 0; x < RESOLUTION; x++)
    for (int y = 0; y < RESOLUTION; y++) {
        
        int index = y*RESOLUTION + x;
        img_composite[w*(y+HALF_RES) + x] =
                clr_scale(top[index],  dff_top[x][y]);
        if (y < HALF_RES) {
            for (int s = 0; s < 4; s++) {
            img_composite[w*y + x + s*RESOLUTION] =
                    clr_scale(sides[s][index], dff_side[x][y]);
            }
        }
    }

    pixel_t *pixmap = malloc(w * h * sizeof(pixel_t));
    tonemap_max(img_composite, w*h, pixmap);
    free(img_composite);
    draw(w, h, pixmap);
    free(pixmap);
}
