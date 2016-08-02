#include "patch.h"

#include <stdlib.h>
#include "constants.h"

Patch_t *Patch_new(const Quad_t *quad, vec3_t normal, color_t init_exitance)
{
    Patch_t *patch = malloc(sizeof(Patch_t));
    Shape_t *shape = (Shape_t *)quad;
    *patch = (Patch_t) {
        .quad = quad,
        .position = v3_add(v3_scale(normal, VEC_FUDGE), shape->position),
        .normal = normal,
        .exitance = init_exitance,
        .d_exitance = init_exitance,
        .d_unsent = init_exitance
    };
    return patch;
}

vec3_t Patch_get_normal(const Patch_t *patch)
{
    return patch->normal;
}

vec3_t Patch_get_position(const Patch_t *patch)
{
    return patch->position;
}

void Patch_apply(Patch_t *recv, const Patch_t *send, vfloat_t dff)
{
    color_t incoming = clr_scale(send->d_unsent, dff);
    recv->exitance = clr_add(recv->exitance, incoming);
    recv->d_exitance = clr_add(recv->d_exitance, incoming);
}

PatchSet_t *PatchSet_new()
{
    PatchSet_t *patchset = malloc(sizeof(PatchSet_t));
    patchset->capacity = 1;
    patchset->count = 0;
    patchset->patches = NULL;
    return patchset;
}

void PatchSet_add_patch(PatchSet_t *pset, Patch_t *patch)
{
    if (pset->count == pset->capacity) {
        pset->capacity*= 2;
        pset->patches = realloc(pset->patches, pset->size * sizeof(patch));
    }
    pset->patches[pset->count] = patch;
    pset->count++;
}
