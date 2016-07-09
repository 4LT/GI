#include <string.h>
#include "hemicube.h"
#include "constants.h"
/* debugging */
#include "canvas.h"
#include "tone_mapping.h"

#define RESOLUTION 256
#define HALF_RES (RESOLUTION / 2)

static const vfloat_t CELL_W = 2 / (vfloat_t)RESOLUTION;
static const vfloat_t CELL_AREA = CELL_W * CELL_W;

static vfloat_t dff_top[RESOLUTION][RESOLUTION];
static vfloat_t dff_side[RESOLUTION][HALF_RES];

static void debug_hemicube(
    color_t *xyp,
    color_t *yzp,
    color_t *yzn,
    color_t *xzp,
    color_t *xzn);

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

color_t Hcube_gather(const scene_t *scene, vec3_t pos, vec3_t normal)
{
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        init_d_formfactors();
    }

    vec3_t local_z = normal;
    vec3_t local_x = v3_arbinormal(normal);
    vec3_t local_y = v3_cross(local_z, local_x);

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

    color_t *img_xyp = malloc(RESOLUTION * RESOLUTION * sizeof(color_t));
    color_t *img_yzp = malloc(RESOLUTION * HALF_RES * sizeof(color_t));
    color_t *img_yzn = malloc(RESOLUTION * HALF_RES * sizeof(color_t));
    color_t *img_xzp = malloc(RESOLUTION * HALF_RES * sizeof(color_t));
    color_t *img_xzn = malloc(RESOLUTION * HALF_RES * sizeof(color_t));

    scene_render(scene, &cam_xyp, img_xyp);
    scene_render(scene, &cam_yzp, img_yzp);
    scene_render(scene, &cam_yzn, img_yzn);
    scene_render(scene, &cam_xzp, img_xzp);
    scene_render(scene, &cam_xzn, img_xzn);

#ifndef DEBUG_HEMICUBE
#   define DEBUG_HEMICUBE false
#endif
    if (DEBUG_HEMICUBE) {
        debug_hemicube(img_xyp, img_yzp, img_yzn, img_xzp, img_xzn);
    }

    color_t gathered = CLR_BLACK;
    for (int x = 0; x < RESOLUTION; x++)
    for (int y = 0; y < RESOLUTION; y++) {
        int index = y*RESOLUTION + x;
        gathered = clr_add(gathered, clr_scale(img_xyp[index], dff_top[x][y]));
        if (y < HALF_RES) {
            gathered = clr_add(gathered,
                clr_scale(img_yzp[index], dff_side[x][y]));
            gathered = clr_add(gathered,
                clr_scale(img_yzn[index], dff_side[x][y]));
            gathered = clr_add(gathered,
                clr_scale(img_xzp[index], dff_side[x][y]));
            gathered = clr_add(gathered,
                clr_scale(img_xzn[index], dff_side[x][y]));
        }
    }

    free(img_xyp);
    free(img_yzp);
    free(img_yzn);
    free(img_xzp);
    free(img_xzn);

    return gathered;
}

static void debug_hemicube(
    color_t *xyp,
    color_t *yzp,
    color_t *yzn,
    color_t *xzp,
    color_t *xzn)
{
    int w = 4 * RESOLUTION;
    int h = RESOLUTION + HALF_RES;
    color_t *img_composite = calloc(w * h, sizeof(color_t));

    for (int x = 0; x < RESOLUTION; x++)
    for (int y = 0; y < RESOLUTION; y++) {
        img_composite[w*(y+HALF_RES) + x] =
                clr_scale(xyp[y*RESOLUTION + x],  dff_top[x][y]);
        if (y < HALF_RES) {
            img_composite[w*y + x] =
                    clr_scale(xzn[y*RESOLUTION + x], dff_side[x][y]);
            img_composite[w*y + x + RESOLUTION] =
                    clr_scale(yzn[y*RESOLUTION + x], dff_side[x][y]);
            img_composite[w*y + x + 2*RESOLUTION] =
                    clr_scale(xzp[y*RESOLUTION + x], dff_side[x][y]);
            img_composite[w*y + x + 3*RESOLUTION] =
                    clr_scale(yzp[y*RESOLUTION + x], dff_side[x][y]);
        }
    }

    pixel_t *pixmap = malloc(w * h * sizeof(pixel_t));
    tonemap_max(img_composite, w*h, pixmap);
    free(img_composite);
    draw(w, h, pixmap);
    free(pixmap);
}
