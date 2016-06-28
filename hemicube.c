#include "hemicube.h"

static const int RESOLUTION = 256;
static const int HALF_RES = RESOLUTION/2;
static const vfloat_t CELL_W = 2 / (vfloat_t)RESOLUTION;
static const vfloat_t CELL_AREA = CELL_W * CELL_W;

typedef struct
{
    vfloat_t form_factor;
} cell_t;

typedef struct
{
    cell_t xyp[RESOLUTION][RESOLUTION];
    cell_t yzp[RESOLUTION][HALF_RES];
    cell_t yzn[RESOLUTION][HALF_RES];
    cell_t xzp[RESOLUTION][HALF_RES];
    cell_t xzn[RESOLUTION][HALF_RES];
} Hcube_t;

Hcube_t *Hcube_new()
{
    Hcube_t *cube = malloc(sizeof(Hcube_t));
    for (int xi = 0; xi < RESOLUTION; xi++) {
        vfloat_t x = (vfloat_t)(2*xi) / RESOLUTION - 1 + CELL_W/2;
        for (int yi = 0; yi < RESOLUTION; yi++) {
            vfloat_t y = (vfloat_t)(2*yi) / RESOLUTION - 1 + CELL_W/2;
            vfloat_t denom = (1 + x*x + y*y);
            denom = PI * denom*denom;
            cube->xyp[xi][yi].form_factor = CELL_AREA / denom;
        }
        for (int zi = 0; zi < HALF_RES; zi++) {
            vfloat_t z = (vfloat_t)zi / HALF_RES + CELL_W/2;
            vfloat_t denom = (1 + x*x + z*z);
            denom = PI * denom*denom;
            cube->xzp[xi][zi].form_factor = z * CELL_AREA / denom;
        }
    }
    memcpy(&cube->xzn, &cube->xzp, sizeof(cube->xzp));
    memcpy(&cube->yzp, &cube->xzp, sizeof(cube->xzp));
    memcpy(&cube->yzn, &cube->xzp, sizeof(cube->xzp));
}

void Hcube_free(Hcube *cube)
{
    free(cube);
}
