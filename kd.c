#include "kd.h"
#include <stdio.h>
#include <string.h>
#include "util/ops.h"

static const int KD_MAX_LEAF_SZ = 120;
static const int MAX_REPEATS = 1;

static vfloat_t select_kth(Shape_t *shapes[], int start, int end, int k,
       enum kd_plane_align a)
{
    int length = end - start + 1;
    int pivot_i = rand() % length + start;
    vfloat_t pivot = shapes[pivot_i]->position.v[a];
    MS_SWAP(shapes[pivot_i], shapes[end], Shape_t *)
    int l = start, r = end-1;
    enum { SCAN_LTR, SCAN_RTL } dir = SCAN_LTR;
    
    while (l <= r) {
        if (dir == SCAN_LTR) {
            if (shapes[l]->position.v[a] < pivot)
                l++;
            else
                dir = SCAN_RTL;
        }
        else {
            if (shapes[r]->position.v[a] >= pivot)
                r--;
            else {
                MS_SWAP(shapes[l], shapes[r], Shape_t *)
                l++;
                r--;
                dir = SCAN_LTR;
            }
        }
    }
    MS_SWAP(shapes[l], shapes[end], Shape_t *)

    if (l < k)
        return select_kth(shapes, l+1, end, k, a);
    else if (l > k)
        return select_kth(shapes, start, l-1, k, a);
    else
        return shapes[l]->position.v[a];
}

/* TRANSFERS ownership of "shapes" to CALLEE */
KDnode_t *kdnode_new(Shape_t **shapes, size_t shapes_length, 
        enum kd_plane_align a, int repeats, int *leaf_count)
{
    KDnode_t *kdn = malloc(sizeof(KDnode_t));
    if (shapes_length <= KD_MAX_LEAF_SZ || repeats >= MAX_REPEATS) {
        printf("%d ", (int)shapes_length);
        (*leaf_count)++;
        kdn->leaf_data = malloc(shapes_length * sizeof(Shape_t *));
        memcpy(kdn->leaf_data, shapes, shapes_length * sizeof(Shape_t *));
        free(shapes);
        kdn->front = NULL;
        kdn->back = NULL;
        kdn->shape_count = shapes_length;
        return kdn;
    }

    kdn->leaf_data = NULL;

    /* *approximate* median */
    vfloat_t median = select_kth(
            shapes, 0, shapes_length - 1, (shapes_length-1)/2, a);
    kdn->plane_offset = median;
    kdn->plane = a;

    Shape_t **front_shapes = malloc(shapes_length * sizeof(Shape_t *));
    Shape_t **back_shapes  = malloc(shapes_length * sizeof(Shape_t *));
    size_t front_sz = 0, back_sz = 0;
    for (size_t i = 0; i < shapes_length; i++) {
        if (shapes[i]->bounds[a][1] > median)
            front_shapes[front_sz++] = shapes[i];
        if (shapes[i]->bounds[a][0] < median)
            back_shapes[back_sz++] = shapes[i];
    }

    free(shapes);
    shapes = NULL;
    front_shapes = realloc(front_shapes, front_sz * sizeof(Shape_t *));
    back_shapes  = realloc(back_shapes, back_sz * sizeof(Shape_t *));

    enum kd_plane_align new_align;
    switch (a) {
        case KDP_YZ: new_align = KDP_XZ;
                     break;
        case KDP_XZ: new_align = KDP_XY;
                     break;
        case KDP_XY: new_align = KDP_YZ;
    }

    kdn->front = kdnode_new(front_shapes, front_sz, new_align,
            (front_sz == shapes_length) ? repeats+1 : 0, leaf_count);
    kdn->back = kdnode_new(back_shapes, back_sz, new_align,
            (back_sz == shapes_length) ? repeats+1 : 0, leaf_count);
    return kdn;
}

/* TODO: if caller later-on frees the data in shapes, we're in trouble.
 * However, we can't re-allocate shapes because we don't know the size of the
 * shapes (we don't know the type). Note as weakness to this style of
 * polymorphism. Perhaps add overridable deep copy to shape.
 */
KDnode_t *kdnode_new_root(const Llist_t *shapes, enum kd_plane_align a)
{
    Llist_node_t *node = shapes->first;
    Shape_t **shape_arr =
            (Shape_t **)malloc(shapes->length * sizeof(Shape_t *));
    for (int i = 0; i < shapes->length; i++) {
        shape_arr[i] = (Shape_t *)(node->datum);
        node = node->next;
    }
    int leaf_count = 0;
    KDnode_t *kdn = kdnode_new(shape_arr, shapes->length, a, 0, &leaf_count);
    printf("\nshapes: %d\n", shapes->length);
    printf("leaf count: %d\n", leaf_count);
    return kdn;
}
