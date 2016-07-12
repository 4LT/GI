#include "stats.h"

#include <string.h>
#include <math.h>
#include "ops.h"

double kth_simple_get(const void *values, int index)
{ return ((double *)values)[index]; }

static void swap_values(
        const void *values, size_t ai, size_t bi, size_t type_sz)
{
    char *a = (char *)values + ai*type_sz;
    char *b = (char *)values + bi*type_sz;
    char *tmp[type_sz];
    memcpy(tmp, a, type_sz);
    memcpy(a, b, type_sz);
    memcpy(b, tmp, type_sz);
}

double kth_select(const void *values, int start, int end, int k,
       kth_accessor_fp get, size_t type_sz)
{
    int length = end - start + 1;
    int pivot_i = rand() % length + start;
    double pivot = get(values, pivot_i);
    swap_values(values, pivot_i, end, type_sz);
    int l = start, r = end-1;
    enum { SCAN_LTR, SCAN_RTL } dir = SCAN_LTR;
    
    while (l <= r) {
        if (dir == SCAN_LTR) {
            if (get(values, l) < pivot)
                l++;
            else
                dir = SCAN_RTL;
        }
        else {
            if (get(values, r) >= pivot)
                r--;
            else {
                swap_values(values, l, r, type_sz);
                l++;
                r--;
                dir = SCAN_LTR;
            }
        }
    }
    swap_values(values, l, end, type_sz);

    if (l < k)
        return kth_select(values, l+1, end, k, get, type_sz);
    else if (l > k)
        return kth_select(values, start, l-1, k, get, type_sz);
    else
        return get(values, l);
}

double stats_mad(void *values, int count, kth_accessor_fp get, size_t type_sz)
{
    double deviations[count];
    double median = stats_median(values, count, get, type_sz);
    for (int i = 0; i < count; i++) {
        deviations[i] = fabs(median - get(values, i));
    }
    return stats_median(deviations, count, kth_simple_get, sizeof(double));
}

