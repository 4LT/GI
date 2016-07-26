#include "aabb.h"

#include <assert.h>
#include "util/ops.h"
#include "vecmatops.h"

void validate_aabb(const aabb_t *bbox)
{
    for (int i = 0; i < 3; i++) {
//        assert(bbox->lower.v[i] < bbox->upper.v[i]);
    }
}

/* TODO: Hack, find better way of dealing with "empty" boxes */
void aabb_bump(aabb_t *bbox)
{
#if 0
    for (int i = 0; i < 3; i++) {
        if (bbox->lower.v[i] == bbox->upper.v[i])
            bbox->upper.v[i]+= 0.001;
    }
#endif
}

aabb_t init_aabb(vec3_t p1, vec3_t p2)
{
    aabb_t bbox = (aabb_t) { v3_min(p1, p2), v3_max(p1, p2) };
    aabb_bump(&bbox);
    validate_aabb(&bbox);
    return bbox;
}

void grow_aabb_by_aabb(aabb_t *targ, const aabb_t *new_box)
{
    validate_aabb(targ);
    validate_aabb(new_box);
    targ->lower = v3_min(targ->lower, new_box->lower);
    targ->upper = v3_max(targ->upper, new_box->upper);
    validate_aabb(targ);
}

void grow_aabb_by_vertex(aabb_t *targ, vec3_t vertex)
{
    validate_aabb(targ);
    targ->lower = v3_min(targ->lower, vertex);
    targ->upper = v3_max(targ->upper, vertex);
    validate_aabb(targ);
}

void split_aabb(const aabb_t *in, enum kd_plane_align a,
        vfloat_t offset, aabb_t *front, aabb_t *back)
{
    validate_aabb(in);
    *front = *in;
    *back = *in;
    front->lower.v[a] = ME_MAX(in->lower.v[a], offset);
    back-> upper.v[a] = ME_MIN(in->upper.v[a], offset);
    aabb_bump(front);
    aabb_bump(back);
    validate_aabb(front);
    validate_aabb(back);
}

bool hit_aabb(const aabb_t *bbox, ray_t ray)
{
    validate_aabb(bbox);
    for (enum kd_plane_align align = 0; align < 3; align++) {
        vfloat_t lower_offset = bbox->lower.v[align];
        vfloat_t upper_offset = bbox->upper.v[align];
        vfloat_t dir = ray.direction.v[align];
        vfloat_t pos = ray.position.v[align];

        if (dir == 0)
            continue;

        vfloat_t t;
        for (int i = 0; i < 2; i++) {
            t = (i == 0) ? (lower_offset - pos)/dir :
                        (upper_offset - pos)/dir;

            if (t >= 0) {
                enum kd_plane_align align2 = (align + 1)%3;
                enum kd_plane_align align3 = (align + 2)%3;
                vfloat_t u = ray.direction.v[align2] * t + ray.position.v[align2];
                vfloat_t v = ray.direction.v[align3] * t + ray.position.v[align3];
                if (u >= bbox->lower.v[align2] && u <= bbox->upper.v[align2])
                if (v >= bbox->lower.v[align3] && v <= bbox->upper.v[align3])
                    return true;
            }
        } 
    }
    return false;
}
