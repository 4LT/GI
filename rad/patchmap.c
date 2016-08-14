#include "patchmap.h"

static inline size_t hash(PatchMap_t *map, size_t key)
{ return key % map->capacity; }

PatchMap_t *PatchMap_new(size_t init_capacity)
{
    PatchMap_t *map = malloc(sizeof(PatchMap_t));
    map->capacity = init_capacity;
    map->buckets = calloc(map->capacity, sizeof(Llist_t *)); 
    map->count = 0;
    map->loadf = PATCHMAP_LOAD_FACTOR;
    map->resize = PATCHMAP_RESIZE;
    return map;
}

void free_buckets(PatchMap_t *map)
{
    for (int i = 0; i < map->capacity ; i++) {
        if (map->buckets[i] != NULL) {
            Llist_free_all(map->buckets[i], free);
        }
    }
    free(map->buckets);
}

void rehash(PatchMap_t *map)
{
    PatchMap_t *new_map = PatchMap_new(map->capacity * map->resize);
    for (int i = 0; i < map->capacity; i++) {
        if (map->buckets[i] != NULL) {
            for (Llist_node_t *node = map->buckets[i]->first; node != NULL;
                    node = node->next) {
                kffpair_t *pair = (kffpair_t *)(node->datum);
                PatchMap_apply_dff(new_map, pair->key, pair->ffactor);
            }
        }
    }
    free_buckets(map);
    *map = *new_map;
}

void PatchMap_free(PatchMap_t *map)
{
    free_buckets(map);
    free(map);
}

kffpair_t *PatchMap_extract_and_clear(PatchMap_t *map, size_t *count)
{
    kffpair_t *pair_arr = malloc(sizeof(kffpair_t) * map->count);
    size_t pair_arr_i = 0;
    for (size_t i = 0; i < map->capacity; i++) {
        if (map->buckets[i] != NULL) {
            for (Llist_node_t *node = map->buckets[i]->first; node != NULL;
                    node = node->next) {
                pair_arr[pair_arr_i++] = *(kffpair_t *)(node->datum);
            }
            Llist_free_all(map->buckets[i], free);
            map->buckets[i] = NULL;
        }
    }
    *count = map->count;
    map->count = 0;
    return pair_arr;
}

void PatchMap_apply_dff(PatchMap_t *map, size_t key, double dff)
{
    if ((double)map->count+1 > map->loadf * map->capacity)
        rehash(map);

    Llist_t **buckets = map->buckets;
    size_t i = hash(map, key);
    if (buckets[i] == NULL) {
        buckets[i] = Llist_new();
    }
    else {
        for (Llist_node_t *node = buckets[i]->first; node != NULL;
                node = node->next)
        {
            kffpair_t *pair = (kffpair_t *)(node->datum);
            if (key == pair->key) {
                pair->ffactor+= dff;
                return;
            }
        }
    }
    kffpair_t *pair = malloc(sizeof(kffpair_t));
    *pair = (kffpair_t) { key, dff };
    Llist_append(buckets[i], pair);
    map->count++;
}
