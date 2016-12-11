#ifndef PATCH_H_
#define PATCH_H_

#include <shapes.h>

typedef struct
{
    vec3_t vert0;
    vec3_t vert1;
    vec3_t vert2;
    vec3_t vert3;
    vec3_t normal;
} partial_quad_t;

typedef struct PatchSet PatchSet_t;
typedef struct Patch Patch_t;

/* Associate a patch with a form factor, using a patch pointer as a key to the
 * form factor value. */
typedef struct patchff
{
    Patch_t *patch;
    double ffactor;
} patchff_t;

/* Obtain the patch indexed by color */
Patch_t *Patch_by_color(color_t color);

/* Obtain the color connected to the given patch */
color_t Patch_to_color(Patch_t *index);

Patch_t *Patch_new(const Quad_t *quad, vec3_t normal, color_t init_exitance);

void Patch_free(Patch_t *patch);

void Patch_apply(
        Patch_t *receiving, const Patch_t *sending, vfloat_t dff);

vec3_t Patch_get_normal(const Patch_t *patch);

vec3_t Patch_get_position(const Patch_t *patch);

PatchSet_t *PatchSet_new();

void PatchSet_compute(PatchSet_t *pset);

void PatchSet_add_patch(PatchSet_t *pset, partial_quad_t part_quad,
        color_t init_exitance, color_t reflectance);

size_t PatchSet_size(PatchSet_t *pset);

Patch_t *PatchSet_patch_at(PatchSet_t *pset, size_t index);

#endif
