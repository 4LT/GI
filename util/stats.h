#ifndef STATS_H_
#define STATS_H_

#include <stdlib.h>

typedef double (*kth_accessor_fp)(const void *values, int index);

double kth_simple_get(const void *values, int index);

double kth_select(const void *values, int start, int end, int k,
       kth_accessor_fp get, size_t type_sz);

static inline double stats_median(
        void *values, int count, kth_accessor_fp get, size_t type_sz)
{ return kth_select(values, 0, count - 1, (count-1)/2, get, type_sz); }

double stats_mad(void *values, int count, kth_accessor_fp get, size_t type_sz);

#endif
