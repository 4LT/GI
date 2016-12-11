#include "reszarr.h"

struct ReszArr
{
    void **data;
    size_t count;
    size_t capacity;
};

ReszArr_t *ReszArr_new()
{
    ReszArr_t *arr = malloc(sizeof(ReszArr_t));
    arr->capacity = 2;
    arr->data = malloc(sizeof(void *) * arr->capacity);
    arr->count = 0;
    return arr;
}

void ReszArr_free_all(ReszArr_t *arr, free_datum_fp free_func)
{
    for (size_t i = 0; i < arr->count; i++) {
        free_func(arr->data[i]);
    }
    ReszArr_free_arr(arr);
}

void ReszArr_free_arr(ReszArr_t *arr)
{
    free(arr->data);
    free(arr);
}

void ReszArr_append(ReszArr_t *arr, void *datum)
{
    if (arr->count >= arr->capacity) {
        arr->capacity*= 2;
        arr->data = realloc(arr->data, sizeof(void *) * arr->capacity);
    }
    arr->data[arr->count] = datum;
    arr->count++;
}

void ReszArr_truncate(ReszArr_t *arr)
{
    arr->capacity = arr->count;
    arr->data = realloc(arr->data, sizeof(void *) * arr->capacity);
}

size_t ReszArr_get_count(ReszArr_t *arr)
{
    return arr->count;
}

void *ReszArr_at(ReszArr_t *arr, size_t index)
{
    return arr->data[index];
}
