#include "patch.h"

#include <stdlib.h>
#include <constants.h>
#include <util/reszarr.h>
#include "hemicube.h"

/* A quadrilateral patch with exitance, and a map of patches associating other
 * patches visible from this patch with form factors. */
struct Patch
{
    const Quad_t *quad; /* Geometeric component */
    vec3_t position; /* world space location of patch center */
    vec3_t normal; /* (average) normal vector of patch */
    color_t exitance; 
    color_t d_exitance;
    color_t d_unsent;
    color_t reflectance;
    vfloat_t area;

    patchff_t *patchffs; /* patch-to-form factor map */
    size_t patchff_count;
};

/* Information on scene properties and a set of patches. Includes the amount
 * of total unsent exitance in a scene. */
struct PatchSet
{
    ReszArr_t *patches;
    color_t total_unsent; /* total unsent exitance in the scene */
    scene_t scene;
};

enum
{
    INDEX_COLOR_ASSERT = 
        1/ /* COLOR type must be AT LEAST as large as size_t */
        (sizeof(color_t) >= sizeof(Patch_t *))
};

vfloat_t tri_area(Triangle_t *tri)
{
    vec3_t ab = v3_sub(tri->verts[1], tri->verts[0]);
    vec3_t ac = v3_sub(tri->verts[2], tri->verts[0]);
    return v3_magnitude(v3_cross(ab, ac)) / 2;
}

Patch_t *Patch_by_color(color_t color)
{
    union { color_t color; Patch_t *patch; } patch_u;
    patch_u.color = color;
    return patch_u.patch;
}

color_t Patch_to_color(Patch_t *patch)
{
    union { color_t color; Patch_t *patch; } patch_u;
    patch_u.patch = patch;
    return patch_u.color;
}

vec3_t Patch_get_normal(const Patch_t *patch)
{ return patch->normal; }

vec3_t Patch_get_position(const Patch_t *patch)
{ return patch->position; }

void apply_patch(Patch_t *recv, const Patch_t *send, vfloat_t dff)
{
    color_t incoming = clr_scale(send->d_unsent, dff);
    recv->exitance = clr_add(recv->exitance, incoming);
    recv->d_exitance = clr_add(recv->d_exitance, incoming);
}

PatchSet_t *PatchSet_new()
{
    PatchSet_t *patchset = malloc(sizeof(PatchSet_t));
    *patchset = (PatchSet_t) {
        .patches = ReszArr_new(),
        .total_unsent = CLR_BLACK,
        .scene = scene_empty_scene(CLR_BLACK)
    };
    return patchset;
}

void PatchSet_free(PatchSet_t *pset)
{
    ReszArr_free_all(pset->patches, free);
    free(pset);
}

void PatchSet_add_patch(PatchSet_t *pset, partial_quad_t part_quad,
        color_t init_exitance, color_t reflectance)
{
    Patch_t *patch = malloc(sizeof(Patch_t));
    Material_t *mtrl = fullbright_new(&(pset->scene), Patch_to_color(patch));
    Quad_t *quad = quad_new(mtrl,
            part_quad.vert0, part_quad.vert1, part_quad.vert2, part_quad.vert3,
            false);
    Shape_t *qshape = (Shape_t *)quad;
    *patch = (Patch_t) {
        .quad = quad,
        .normal = part_quad.normal,
        .position = v3_add(v3_scale(part_quad.normal, VEC_FUDGE),
                qshape->position),
        .exitance = init_exitance,
        .d_exitance = init_exitance,
        .d_unsent = init_exitance,
        .reflectance = reflectance,
        .area = tri_area(quad->t1) + tri_area(quad->t2)
    };

    ReszArr_append(pset->patches, (void *)patch);
    pset->total_unsent = clr_add(pset->total_unsent, init_exitance);
    scene_add_shape(&(pset->scene), qshape);
}

/* Set up form factors, and calculate radiosity */
void PatchSet_compute(PatchSet_t *pset)
{
    /* Get all form factors for each patch */
    ReszArr_t *parr = pset->patches;
    size_t count = ReszArr_get_count(parr);
    for (size_t i = 0; i < count; i++) {
        Patch_t *patch = (Patch_t *)ReszArr_at(parr, i);
        patch->patchffs = Hcube_get_ffs(&pset->scene, patch,
                &patch->patchff_count);
    }

    /* drain exitance */
    for (int i = 0; i < 3; i++) {
        for (size_t j = 0; j < ReszArr_get_count(pset->patches); j++) {
            Patch_t *sender = ReszArr_at(pset->patches, j);
            for (size_t k = 0; k < sender->patchff_count; k++) {
                Patch_t *recvr = sender->patchffs[k].patch;
                double ffactor = sender->patchffs[k].ffactor;
                color_t ext = clr_mul(recvr->reflectance, sender->d_unsent);
                ext = clr_scale(ext,
                        (vfloat_t)(ffactor * sender->area / recvr->area));
                
                recvr->d_exitance = clr_add(recvr->d_exitance, ext);
                recvr->d_unsent = clr_add(recvr->d_unsent, ext);
                recvr->exitance = clr_add(recvr->exitance, ext);
                pset->total_unsent = clr_sub(clr_add(pset->total_unsent,
                            ext), sender->d_unsent);
                sender->d_unsent = (color_t) {{ 0, 0, 0 }};
            }
        }

        for (size_t j = 0; j < ReszArr_get_count(pset->patches); j++) {
        }
    }
}

size_t PatchSet_size(PatchSet_t *pset)
{
    return ReszArr_get_count(pset->patches);
}

Patch_t *PatchSet_patch_at(PatchSet_t *pset, size_t index)
{
    return (Patch_t *)ReszArr_at(pset->patches, index);
}
