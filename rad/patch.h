#ifndef PATCH_H_
#define PATCH_H_

#include <shapes.h>

/* Vertex data and normal vector of a quad, data available from patch scene
 * files */
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

vec3_t Patch_get_normal(const Patch_t *patch);

vec3_t Patch_get_position(const Patch_t *patch);

/* Create an empty seet of patches. */
PatchSet_t *PatchSet_new();

/* Free memory allocated with PatchSet_new. */
void PatchSet_free(PatchSet_t *pset);

/* Perform radiosity calculations */
void PatchSet_compute(PatchSet_t *pset);

/* Add a new patch to a patch set, provided data available in a patch scene
 * file. */
void PatchSet_add_patch(PatchSet_t *pset, partial_quad_t part_quad,
        color_t init_exitance, color_t reflectance);

/* Get the number of patches in a set */
size_t PatchSet_size(PatchSet_t *pset);

Patch_t *PatchSet_patch_at(PatchSet_t *pset, size_t index);

#endif
