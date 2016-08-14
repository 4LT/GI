#ifndef RESZARR_H_
#define RESZARR_H_

typedef struct
{
    void **data;
    size_t count;
    size_t capacity;
} ReszArr_t;

ReszArr_t *ReszArr_new();

void ReszArr_free_all(ReszArr_t *arr, free_datum_fp free_func);

void ReszArr_free_arr(ReszArr_t *arr);

void ReszArr_append(ReszArr_t *arr, void *datum);

void ReszArr_truncate(ReszArr_t *arr);

#endif
