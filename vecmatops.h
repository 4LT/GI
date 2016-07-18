/* vecmatops.h
 *
 * Vector an matrix operations. No pointers are used, so there are no
 * side-effects.  Matrices are treated as Row-major.
 * author: Seth Rader
 */
#ifndef VECMATOPS_H_
#define VECMATOPS_H_

#include <tgmath.h>
#include "types.h"
#include "util/ops.h"

#define MAT4_IDENTITY = { {{ 1, 0, 0, 0 },\
                           { 0, 1, 0, 0 },\
                           { 0, 0, 1, 0 },\
                           { 0, 0, 0, 1 }} };

/* Dot product of vecA and vecB */
static inline vfloat_t v3_dot(const vec3_t vecA, const vec3_t vecB)
{
    return vecA.v[0]*vecB.v[0] + vecA.v[1]*vecB.v[1] + vecA.v[2]*vecB.v[2];
}

/* Cross porduct of VecA nad vecB */
static inline vec3_t v3_cross(vec3_t vecA, vec3_t vecB)
{
    vec3_t out;
    out.v[0] = vecA.v[1]*vecB.v[2] - vecA.v[2]*vecB.v[1];
    out.v[1] = vecA.v[2]*vecB.v[0] - vecA.v[0]*vecB.v[2];
    out.v[2] = vecA.v[0]*vecB.v[1] - vecA.v[1]*vecB.v[0];
    return out;
}

/* Multiplies each element of "in" by "scale". */
static inline vec3_t v3_scale(vec3_t in, vfloat_t scale)
{
    vec3_t out;
    out.v[0] = scale * in.v[0];
    out.v[1] = scale * in.v[1];
    out.v[2] = scale * in.v[2];
    return out;
}

static inline vec3_t v3_neg(vec3_t in)
{
    return v3_scale(in, -1);
}

/* Projects vector vecA onto vecB. */
static inline vec3_t v3_project(vec3_t vecA, vec3_t vecB)
{
    return v3_scale(vecB, v3_dot(vecA, vecB) / v3_dot(vecB, vecB));
}

/* Divides each element of "in" by "divisor". */
static inline vec3_t v3_divide(vec3_t in, vfloat_t divisor)
{
    vec3_t out;
    out.v[0] = in.v[0] / divisor;
    out.v[1] = in.v[1] / divisor;
    out.v[2] = in.v[2] / divisor;
    return out;
}

static inline vec3_t v3_add(vec3_t vecA, vec3_t vecB)
{
    vec3_t out;
    out.v[0] = vecA.v[0] + vecB.v[0];
    out.v[1] = vecA.v[1] + vecB.v[1];
    out.v[2] = vecA.v[2] + vecB.v[2];
    return out;
}

static inline vec3_t v3_sub(vec3_t vecA, vec3_t vecB)
{
    vec3_t out;
    out.v[0] = vecA.v[0] - vecB.v[0];
    out.v[1] = vecA.v[1] - vecB.v[1];
    out.v[2] = vecA.v[2] - vecB.v[2];
    return out;
}

static inline vec3_t v3_min(vec3_t vecA, vec3_t vecB)
{
    vec3_t out;
    for (int i = 0; i < 3; i++)
        out.v[i] = ME_MIN(vecA.v[i], vecB.v[i]);
    return out;
}

static inline vec3_t v3_max(vec3_t vecA, vec3_t vecB)
{
    vec3_t out;
    for (int i = 0; i < 3; i++)
        out.v[i] = ME_MAX(vecA.v[i], vecB.v[i]);
    return out;
}

static inline vfloat_t v3_magnitude(vec3_t in)
{
    return sqrt(v3_dot(in, in));
}

static inline vec3_t v3_normalize(vec3_t in)
{
    vfloat_t magnitude = v3_magnitude(in);
    if (magnitude == 0) {
        vec3_t out = {{ 0, 0, 0 }};
        return out;
    }
    else {
        return v3_divide(in, magnitude);
    }
}

static inline vfloat_t v3_distance(vec3_t ptA, vec3_t ptB)
{
    return v3_magnitude(v3_sub(ptB, ptA));
}

static inline vec3_t v3_blend(vec3_t vecA, vec3_t vecB, vfloat_t blend)
{
    vec3_t out;
    for (int i = 0; i < 3; i++) {
        out.v[i] = (1-blend)*vecA.v[i] + blend*vecB.v[i];
    }
    return out;
}

/* Arbitrary vector normal to input vector.  Chooses a vector perpendicular to
 * vecIn and either the Y axis or Z axis, whichever has a better
 * approximation. */
static inline vec3_t v3_arbinormal(vec3_t in)
{
    return in.v[1] > in.v[2] ?
            v3_cross(in, (vec3_t){{ 0, 0, 1 }}) :
            v3_cross((vec3_t) {{ 0, 1, 0 }}, in);
}

static inline vec3_t m4v3_column(mat4_t mat, unsigned int index)
{
    return (vec3_t){{ mat.m[0][index], mat.m[1][index], mat.m[2][index] }};
}

static inline vec3_t m4v3_row(mat4_t mat, unsigned int index)
{
    return (vec3_t){{ mat.m[index][0], mat.m[index][1], mat.m[index][2] }};
}

/* Use xformMat to transform the vector "in". */
static inline vec3_t m4v3_transform(mat4_t xformMat, vec3_t in)
{
   vec3_t out;
   for (int r = 0; r < 3; r++) {
       vfloat_t sum = 0;
       for (int c = 0; c < 3; c++)
           sum+= xformMat.m[r][c] * in.v[c];
       sum+= xformMat.m[r][3];
       out.v[r] = sum;
   }
   return out;
}

#if 0
mat4_t m4v3_buildMatrixRows(
        vec3_t v1, vec3_t v2, vec3_t v3, vec3_t v4)
{
    mat4_t out;
    for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++) {
        out[]
}
#endif

static inline mat4_t m4_transpose(mat4_t in)
{
    mat4_t out;
    for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++) {
        out.m[r][c] = in.m[c][r];
    }
    return out;
}

/* Performs matrix multiplication on matA and matB. */
static inline mat4_t m4_mul(mat4_t matA, mat4_t matB)
{
    mat4_t out;
    for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++) {
        float sum = 0;
        for (int i = 0; i < 4; i++)
            sum+= matA.m[i][c] * matB.m[r][i];
        out.m[r][c] = sum;
    }
    return out;
}

#endif
