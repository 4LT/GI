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

/* 3D vector value type */
typedef struct
{
    vfloat_t v[3];
} vec3_t;

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

/* general-purpose pair type (not used) */
typedef struct pair
{
    void *left;
    void *right;
} pair_t;

/* scene containing shapes, light sources, and a camera */
struct scene;

#endif
