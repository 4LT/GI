/*
 * types.h
 *
 * Commonly used types.
 * Author: Seth Rader
 */

#ifndef TYPES_H_
#define TYPES_H_
#include <stdint.h>

/* floating point type used in vectors */
typedef double vfloat_t;
/* pixel format for pixel buffer sent to canvas */
typedef int32_t pixel_t;

/* enumerated axis-aligned plane alignments. */
enum kd_plane_align { KDP_YZ = 0, KDP_XZ = 1, KDP_XY = 2 };

/* 3D vector value type */
typedef struct
{
    vfloat_t v[3];
} vec3_t;

/* 2D vector value type */
typedef struct
{
    vfloat_t v[2];
} vec2_t;

/* 4x4 matrix value type */
typedef struct
{
    vfloat_t m[4][4];
} mat4_t;

/* ray cast at some direction from some position */
typedef struct
{
    vec3_t position;
    vec3_t direction;
} ray_t;

/* scene containing shapes, light sources, and a camera */
struct scene;

#endif
