/* Resizable array */
#ifndef RESZARR_H_
#define RESZARR_H_

#include <stdlib.h>
#include "collections.h"

typedef struct ReszArr ReszArr_t;

ReszArr_t *ReszArr_new();

void ReszArr_free_all(ReszArr_t *arr, free_datum_fp free_func);

void ReszArr_free_arr(ReszArr_t *arr);

void ReszArr_append(ReszArr_t *arr, void *datum);

void ReszArr_truncate(ReszArr_t *arr);

void ReszArr_sort_descending(ReszArr_t *arr);

size_t ReszArr_get_count(ReszArr_t *arr);

void *ReszArr_at(ReszArr_t *arr, size_t index);

#endif
