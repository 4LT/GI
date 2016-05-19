#include "kd.h"
#include "util/ops.h"

static const int KD_MAX_LEAF_SZ = 3;

static vfloat_t select_kth(Shape_t *shapes[], int start, int end, int k,
       enum plane_align a)
{
    size_t length = end - start + 1;
    vfloat_t pivot = shapes[rand() % length + start]->position.v[a];
    int l = start, r = end;
    enum { SCAN_LTR, SCAN_RTL } dir;
    
    while (l < r) {
        if (dir == SCAN_LTR) {
            if (shapes[l]->position.v[a] > pivot)
                dir = SCAN_RTL;
            else
                l++;
        }
        else {
            if (shapes[r]->position.v[a] > pivot)
                r--;
            else {
                dir = SCAN_LTR;
                MS_SWAP(shapes[l], shapes[r])
                l++;
                r--;
            }
        }
    }

    if (l == end)
        return shapes[l]->position.v[a];
    else if (l > k)
        return select_kth(shapes, start, l-1, k);
    else
        return select_kth(shapes, l, end, k);
}

/* TRANSFERS ownership of "shapes" to CALLEE */
KDnode_t *kdnode_new(Shape_t *shapes, size_t shapes_length, 
        enum kd_plane_align a)
{
    KDnode_t *kdn = malloc(sizeof(KDnode_t));
    if (shapes_length <= KD_MAX_LEAF_SZ) {
        kdn->leaf_data = malloc(shapes_length * sizeof(void *));
        memcpy(kdn->leaf_data, shapes, shapes_length);
        kdn->front = NULL;
        kdn->back = NULL;
        return kdn;
    }
    else {
        kdn->leaf_data = NULL;
    }

    /* *approximate* median */
    vfloat_t median = select_kth(
            shapes, 0, shapes_length - 1, plane);
    kdn->offset = median;
    kdn->plane = a;

    /* TODO: More work than necessary may be done here, but bug in select_kth
     * means shapes might not be perfectly partitioned. If fixed, this could
     * be optimized for less rampant malloc/free by using array slices instead
     * of whole arrays
     * */
    Shape_t *front_shapes[] = malloc(shapes_length * sizeof(void *));
    Shape_t *back_shapes[]  = malloc(shapes_length * sizeof(void *));
    size_t front_sz = 0, back_sz = 0;
    for (int i = 0; i < kdn->shapes_length; i++) {
        if (kdn->shapes[i]->bounds[kdn->plane][0] > median)
            front_shapes[front_sz++] = kdn->shapes[i];
        if (kdn->shapes[i]->bounds[kdn->plane][1] < median)
            back_shapes[back_sz++]  = kdn->shapes[i];
    }

    free(shapes);
    realloc(front_shapes, front_sz);
    realloc(back_shapes, back_sz);
    kdn->front = kdnode_new(front_shapes, front_sz);
    kdn->back = kdnode_new(back_shapes, back_sz);
    return kdn;
}
