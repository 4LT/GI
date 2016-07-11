/* kd.h
 *
 * A kd-tree constructed with a list of shapes.
 * author: Seth Rader */
#ifndef KDIMENSIONAL_H_
#define KDIMENSIONAL_H_

#include <stdlib.h>
#include "util/linked_list.h"
#include "shapes.h"

/* enumerated axis-aligned plane alignments. */
enum kd_plane_align { KDP_YZ = 0, KDP_XZ = 1, KDP_XY = 2 };
/* ways that a ray may interact with the plane */
//enum kd_plane_intersect { KDP_FRONT, KDP_BACK, KDP_MISS };

/* Binary tree divided by planes.  Leaf nodes have NULL children and a list of
 * shapes, while non-leaf nodes have NULL shape data children. */
typedef struct KDnode KDnode_t;
struct KDnode
{
    /* plane with which shapes are divided */
    enum kd_plane_align plane;
    /* how far the plane is offset from the origin */
    vfloat_t plane_offset;

    /* node for shapes in front of the plane */
    KDnode_t *front;
    /* or behind */
    KDnode_t *back;

    /* if this is a leaf node, this contains shapes */
    Shape_t** leaf_data;
    size_t shape_count;
};

/* Creates a new kd-tree.
 * shapes - shapes that are added to the tree
 */
KDnode_t *kdnode_new_root(const Llist_t *shapes);

#endif
