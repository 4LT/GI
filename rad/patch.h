#ifndef PATCH_H_
#define PATCH_H_

#include "shapes.h"

typedef struct
{
    const Quad_t *quad;
    vec3_t position;
    vec3_t normal;
    color_t exitance;
    color_t d_exitance;
    color_t d_unsent;
} Patch_t;

enum
{
    PATCHPTR_COLOR_ASSERT = 
        1/ /* COLOR type must be AT LEAST as large as a POINTER */
        (sizeof(color_t) >= sizeof(Patch_t *))
};

union patchptr_color
{
    color_t color;
    Patch_t *patchptr;
};

static inline Patch_t* Patch_by_color(color_t color)
{
    union patchptr_color patch_u;
    patch_u.color = color;
    return patch_u.patchptr;
}

static inline color_t Patch_to_color(Patch_t *patch)
{
    union patchptr_color patch_u;
    patch_u.patchptr = patch;
    return patch_u.color;
}

Patch_t *Patch_new(const Quad_t *quad, vec3_t normal, color_t init_exitance);

void Patch_free(Patch_t *patch);

void Patch_apply(
        Patch_t *receiving, const Patch_t *sending, vfloat_t dff);

vec3_t Patch_get_normal(const Patch_t *patch);

vec3_t Patch_get_position(const Patch_t *patch);

#endif
