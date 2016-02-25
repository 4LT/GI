#ifndef VECMATOPS_H_
#define VECMATOPS_H_

#include "types.h"
#include <tgmath.h>

static const struct mat4 MAT4_IDENTITY = { {{ 1, 0, 0, 0 },
                                            { 0, 1, 0, 0 },
                                            { 0, 0, 1, 0 },
                                            { 0, 0, 0, 1 }} };

vfloat_t v3_dot(const struct vec3 vecA, const struct vec3 vecB);

struct vec3 v3_cross(struct vec3 vecA, struct vec3 vecB);

struct vec3 v3_scale(struct vec3 in, vfloat_t scale);

struct vec3 v3_divide(struct vec3 in, vfloat_t divisor);

struct vec3 v3_add(struct vec3 vecA, struct vec3 vecB);

struct vec3 v3_sub(struct vec3 vecA, struct vec3 vecB);

vfloat_t v3_magnitude(struct vec3 in);

struct vec3 v3_normalize(struct vec3 in);

struct vec3 m4v3_transform(struct mat4 xformMat, struct vec3 in);

struct mat4 m4v3_buildMatrix(
        struct vec3 v1, struct vec3 v2, struct vec3 v3, struct vec3 v4);

struct mat4 m4_transpose(struct mat4 in);

struct mat4 m4_mul(struct mat4 matA, struct mat4 matB);

#endif
