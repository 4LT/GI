#ifndef AABB_H_
#define AABB_H_

#include <stdbool.h>
#include "types.h"

/* axis aligned bounding box, b: [axis][0 = lower bound, 1 = upper] */
typedef struct
{
    vec3_t lower;
    vec3_t upper;
} aabb_t;

void grow_aabb_by_aabb(aabb_t *targ, const aabb_t *new_box);

void grow_aabb_by_vertex(aabb_t *targ, vec3_t vertex);

aabb_t init_aabb(vec3_t p1, vec3_t p2);

void split_aabb(const aabb_t *in, enum kd_plane_align a,
        vfloat_t offset, aabb_t *front, aabb_t *back);

bool hit_aabb(const aabb_t *bbox, ray_t ray);

#endif
