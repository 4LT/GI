#include "vecmatops.h"

vfloat_t v3_dot(const struct vec3 vecA, const struct vec3 vecB)
{
    return vecA.v[0]*vecB.v[0] + vecA.v[1]*vecB.v[1] + vecA.v[2]*vecB.v[2];
}

struct vec3 v3_cross(struct vec3 vecA, struct vec3 vecB)
{
    struct vec3 out;
    out.v[0] = vecA.v[1]*vecB.v[2] - vecA.v[2]*vecB.v[1];
    out.v[1] = vecA.v[2]*vecB.v[0] - vecA.v[0]*vecB.v[2];
    out.v[2] = vecA.v[0]*vecB.v[1] - vecA.v[1]*vecB.v[0];
    return out;
}

struct vec3 v3_scale(struct vec3 in, vfloat_t scale)
{
    struct vec3 out;
    out.v[0] = scale * in.v[0];
    out.v[1] = scale * in.v[1];
    out.v[2] = scale * in.v[2];
    return out;
}

struct vec3 v3_divide(struct vec3 in, vfloat_t divisor)
{
    return v3_scale(in, 1.0f / divisor);
}

struct vec3 v3_add(struct vec3 vecA, struct vec3 vecB)
{
    struct vec3 out;
    out.v[0] = vecA.v[0] + vecB.v[0];
    out.v[1] = vecA.v[1] + vecB.v[1];
    out.v[2] = vecA.v[2] + vecB.v[2];
    return out;
}

struct vec3 v3_sub(struct vec3 vecA, struct vec3 vecB)
{
    struct vec3 out;
    out.v[0] = vecA.v[0] - vecB.v[0];
    out.v[1] = vecA.v[1] - vecB.v[1];
    out.v[2] = vecA.v[2] - vecB.v[2];
    return out;
}

vfloat_t v3_magnitude(struct vec3 in)
{
    return sqrt(v3_dot(in, in));
}

struct vec3 v3_normalize(struct vec3 in)
{
    vfloat_t magnitude = v3_magnitude(in);
    if (magnitude == 0) {
        struct vec3 out = {{ 0, 0, 0 }};
        return out;
    }
    else {
        return v3_divide(in, magnitude);
    }
}

struct vec3 m4v3_column(struct mat4 mat, unsigned int index)
{
    return (struct vec3){{ mat.m[0][index], mat.m[1][index], mat.m[2][index] }};
}

struct vec3 m4v3_row(struct mat4 mat, unsigned int index)
{
    return (struct vec3){{ mat.m[index][0], mat.m[index][1], mat.m[index][2] }};
}

struct vec3 m4v3_transform(struct mat4 xformMat, struct vec3 in)
{
   struct vec3 out;
   for (int r = 0; r < 3; r++) {
       vfloat_t sum = 0;
       for (int c = 0; c < 3; c++)
           sum+= xformMat.m[r][c] * in.v[c];
       sum+= xformMat.m[r][3];
       out.v[r] = sum;
   }
   return out;
}

/*struct mat4 m4v3_buildMatrixRows(
        struct vec3 v1, struct vec3 v2, struct vec3 v3, struct vec3 v4)
{
    struct mat4 out;
    for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++) {
        out[]
}*/

struct mat4 m4_transpose(struct mat4 in)
{
    struct mat4 out;
    for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++) {
        out.m[r][c] = in.m[c][r];
    }
    return out;
}

struct mat4 m4_mul(struct mat4 matA, struct mat4 matB)
{
    struct mat4 out;
    for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++) {
        float sum = 0;
        for (int i = 0; i < 4; i++)
            sum+= matA.m[i][c] * matB.m[r][i];
        out.m[r][c] = sum;
    }
    return out;
}
