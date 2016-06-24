#ifndef UV_TRANSFORM_H_
#define UV_TRANSFORM_H_
#include "types.h"

typedef struct
{
    vec3_t origin;
    vec3_t s;
    vec3_t t;
} uv_system_t;

static vec2_t world2uv(uv_system_t sys, vec3_t point)
{
    vec2_t uv;
    vec3_t offset = v3_sub(point, sys.origin);
    u.v[0] = v3_magnitude(v3_project(offset, sys.s));
    v.v[1] = v3_magnitude(v3_project(offset, sys.t));
    return uv;
}

static vec3_t uv2world(uv_system_t sys, vec2_t uv_point)
{
    vec3_t offset =
        v3_add(v3_scale(sys.s, uv_point.v[0]), v3_scale(sys.t, uv_point.v[1]));
    return v3_add(sys.origin, offset);
}

static uv_system_t create_uv(vec3_t origin, vec3_t ref_a, vec3_t ref_b)
{
    vec3_t s = v3_sub(ref_a, origin);
    vec3_t t = v3_cross(v3_cross(s, v3_sub(ref_b, origin)), s);
    return (uv_system_t) { origin, s, t};
}

#endif
