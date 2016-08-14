#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

/* Include C file for white-box testing */
#include "patchmap.c"

void print_pairs(kffpair_t *pairs, size_t count)
{
    for (size_t i = 0; i < count; i++)
        printf("%4zu %lf\n", pairs[i].key, pairs[i].ffactor);
}

void test_make_destroy()
{
    PatchMap_t *map = PatchMap_new(20);
    PatchMap_free(map);
}

void test_apply()
{
    PatchMap_t *map = PatchMap_new(20);
    PatchMap_apply_dff(map, 2, 3.0);
    PatchMap_apply_dff(map, 2, 4.0);
    PatchMap_apply_dff(map, 1, 0.5);
    PatchMap_apply_dff(map, 5, 0.0);
    PatchMap_apply_dff(map, 5, 1.0);
    PatchMap_apply_dff(map, 5, 2.0);
    size_t pair_count;
    kffpair_t *ffactors = PatchMap_extract_and_clear(map, &pair_count);
    PatchMap_free(map);
    assert(pair_count == 3);
    print_pairs(ffactors, pair_count);
    for (int i = 0; i < pair_count; i++) {
        kffpair_t pair = ffactors[i];
        switch (pair.key) {
            case 1: assert(pair.ffactor == 0.5); break;
            case 2: assert(pair.ffactor == 7.0); break;
            case 5: assert(pair.ffactor == 3.0); break;
            default: assert(false);
        }
    }
    free(ffactors);
}

void test_rehash()
{
    size_t init_cap = 10;
    PatchMap_t *map = PatchMap_new(init_cap);
    for (size_t i = 0; init_cap == map->capacity; i++) {
        PatchMap_apply_dff(map, i, 1.0);
    }
    printf("Old capacity: %zu\n", init_cap);
    printf("New capacity: %zu\n", map->capacity);
    printf("Count: %zu\n", map->count);
    printf("Load factor: %lf\n", map->loadf);
    assert((double)map->count / map->capacity <= map->loadf);

    size_t count;
    kffpair_t *pairs = PatchMap_extract_and_clear(map, &count);
    PatchMap_free(map);
    print_pairs(pairs, count);
    free(pairs);
}

int main(int argc, char *argv[])
{
    test_make_destroy();
    test_apply();
    test_rehash();
}
