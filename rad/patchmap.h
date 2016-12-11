/* A map for patch pointers to form factors */
#ifndef PATCHMAP_H_
#define PATCHMAP_H_

#include <stdlib.h>
#include <util/collections.h>
#include <util/linked_list.h>
#include "patch.h"

/* defaults */
static const float PATCHMAP_LOAD_FACTOR = 0.75;
static const int PATCHMAP_RESIZE = 2;

typedef struct
{
    /* handle collisions by building a list */
    Llist_t **buckets;
    /* quantity of elements that can be held without resizing */
    size_t capacity;
    /* current number of elements in map */
    size_t count;
    /* maximum ratio of # elements to capacity */
    float loadf;
    /* new capacity = old capacity * resize */
    int resize;
} PatchMap_t;

PatchMap_t *PatchMap_new(size_t init_capacity);

/* Free this map and its bucket list, but NOT the patches or kv pairs */
void PatchMap_free(PatchMap_t *map);

/* Increase form factor by a marginal quantity (dff) */
void PatchMap_apply_dff(PatchMap_t *map, Patch_t *key, double dff);

/* Get array of patch-form factor pairs from map, and empty the map. Count
 * stores the size of the array in number of elements. */
patchff_t *PatchMap_extract_and_clear(PatchMap_t *map, size_t *count);

#endif
