#ifndef PATCHMAP_H_
#define PATCHMAP_H_

#include <stdlib.h>
#include <util/collections.h>
#include <util/linked_list.h>

const float PATCHMAP_LOAD_FACTOR = 0.75;
const int PATCHMAP_RESIZE = 2;

typedef struct
{
    size_t key;
    double ffactor;
} kffpair_t;

typedef struct
{
    Llist_t **buckets;
    size_t capacity;
    size_t count;
    float loadf;
    int resize;
} PatchMap_t;

PatchMap_t *PatchMap_new(size_t init_capacity);

void PatchMap_free(PatchMap_t *map);

void PatchMap_apply_dff(PatchMap_t *map, size_t key, double dff);

kffpair_t *PatchMap_extract_and_clear(PatchMap_t *map, size_t *count);

#endif
