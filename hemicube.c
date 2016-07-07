#include <string.h>
#include "hemicube.h"
#include "constants.h"

#define RESOLUTION 256
#define HALF_RES (RESOLUTION / 2)

static const vfloat_t CELL_W = 2 / (vfloat_t)RESOLUTION;
static const vfloat_t CELL_AREA = CELL_W * CELL_W;

static vfloat_t dff_top[RESOLUTION][RESOLUTION];
static vfloat_t dff_side[RESOLUTION][HALF_RES];

void init_d_formfactors()
{
    for (int xi = 0; xi < RESOLUTION; xi++) {
        vfloat_t x = (vfloat_t)(2*xi) / RESOLUTION - 1 + CELL_W/2;
        for (int yi = 0; yi < RESOLUTION; yi++) {
            vfloat_t y = (vfloat_t)(2*yi) / RESOLUTION - 1 + CELL_W/2;
            vfloat_t denom = (1 + x*x + y*y);
            denom = PI * denom*denom;
            dff_top[xi][yi] = CELL_AREA / denom * 100000;
        }
        for (int zi = 0; zi < HALF_RES; zi++) {
            vfloat_t z = (vfloat_t)zi / HALF_RES + CELL_W/2;
            vfloat_t denom = (1 + x*x + z*z);
            denom = PI * denom*denom;
            dff_side[xi][zi] = z * CELL_AREA / denom * 100000;
        }
    }
}

static inline camera_t camera_by_dir(vec3_t pos, vec3_t up, vec3_t dir, 
        int w, int h)
{ return cam_centered(pos, up, v3_add(pos, dir), w, h); }

color_t *Hcube_render(scene_t *scene, vec3_t pos, vec3_t normal)
{
    init_d_formfactors();

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

    color_t *img_composite = calloc(4*RESOLUTION * (RESOLUTION+HALF_RES),
            sizeof(color_t));

    for (int x = 0; x < RESOLUTION; x++)
    for (int y = 0; y < RESOLUTION; y++) {
        img_composite[4*(y+HALF_RES)*RESOLUTION + x] =
            clr_scale(img_xyp[y*RESOLUTION + x],  dff_top[x][y]);
        if (y < HALF_RES) {
            img_composite[4*y*RESOLUTION + x] =
                clr_scale(img_xzn[y*RESOLUTION + x], dff_side[x][y]);
            img_composite[4*y*RESOLUTION + x + RESOLUTION] =
                clr_scale(img_yzn[y*RESOLUTION + x], dff_side[x][y]);
            img_composite[4*y*RESOLUTION + x + 2*RESOLUTION] =
                clr_scale(img_xzp[y*RESOLUTION + x], dff_side[x][y]);
            img_composite[4*y*RESOLUTION + x + 3*RESOLUTION] =
                clr_scale(img_yzp[y*RESOLUTION + x], dff_side[x][y]);
        }
    }

    free(img_xyp);
    free(img_yzp);
    free(img_yzn);
    free(img_xzp);
    free(img_xzn);

    return img_composite;
}
