#include "kd.h"
#include <stdio.h>
#include <string.h>
#include "util/ops.h"

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

/* *approximate* median */
static vfloat_t find_median(Shape_t **shapes, int shapes_length,
        enum kd_plane_align a)
{
    return select_kth(shapes, 0, shapes_length - 1, (shapes_length-1)/2, a);
}

/* TRANSFERS ownership of "shapes" to CALLEE */
static KDnode_t *kdnode_new(Shape_t **shapes, size_t shapes_length,
        double redundancy_limit)
{
    KDnode_t *kdn = malloc(sizeof(KDnode_t));

    vfloat_t best_median;
    size_t cur_redundancy;
    size_t best_redundancy = SIZE_MAX;
    Shape_t **best_front = NULL, **best_back = NULL;
    size_t best_front_sz, best_back_sz;
    enum kd_plane_align best_align;

    for (enum kd_plane_align a = 0; a < 3; a++) {
        vfloat_t cur_median = find_median(shapes, shapes_length, a);
        size_t cur_front_sz = 0, cur_back_sz = 0;
        Shape_t **front_shapes = malloc(shapes_length * sizeof(Shape_t *));
        Shape_t **back_shapes  = malloc(shapes_length * sizeof(Shape_t *));

        for (size_t i = 0; i < shapes_length; i++) {
            if (shapes[i]->bounds[a][1] >= cur_median)
                front_shapes[cur_front_sz++] = shapes[i];
            if (shapes[i]->bounds[a][0] <= cur_median)
                back_shapes[cur_back_sz++] = shapes[i];
        }
        cur_redundancy = cur_front_sz + cur_back_sz;

        if (cur_redundancy < best_redundancy) {
            best_redundancy = cur_redundancy;
            best_median = cur_median;
            if (best_front != NULL) {
                free(best_front);
                free(best_back);
            }
            best_front = front_shapes;
            best_back = back_shapes;
            best_front_sz = cur_front_sz;
            best_back_sz = cur_back_sz;
            best_align = a;
        }
        else {
            free(front_shapes);
            free(back_shapes);
        }
    }

    double redundancy_frac =
        (best_redundancy - shapes_length) / (double)shapes_length;
    if (redundancy_frac >= redundancy_limit) {
        kdn->leaf_data = shapes;
        kdn->front = NULL;
        kdn->back = NULL;
        kdn->shape_count = shapes_length;
    }

    else {
        free(shapes);
        shapes = NULL;
        best_front = realloc(best_front, best_front_sz * sizeof(Shape_t *));
        best_back  = realloc(best_back, best_back_sz * sizeof(Shape_t *));
        printf("%3zu:%3zu %3zu %5.2f\n",
                shapes_length, best_front_sz, best_back_sz, redundancy_frac);

        kdn->leaf_data = NULL;
        kdn->plane_offset = best_median;
        kdn->plane = best_align;
        kdn->front = kdnode_new(best_front, best_front_sz, redundancy_limit);
        kdn->back = kdnode_new(best_back, best_back_sz, redundancy_limit);
    }

    return kdn;
}

KDnode_t *kdnode_new_root(const Llist_t *shapes, double redundancy_limit)
{
    Llist_node_t *node = shapes->first;
    Shape_t **shape_arr =
            (Shape_t **)malloc(shapes->length * sizeof(Shape_t *));
    for (int i = 0; i < shapes->length; i++) {
        shape_arr[i] = (Shape_t *)(node->datum);
        node = node->next;
    }
    KDnode_t *kdn = kdnode_new(shape_arr, shapes->length, redundancy_limit);
    printf("\nshapes: %d\n", shapes->length);
    return kdn;
}
