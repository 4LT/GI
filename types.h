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
struct vec3
{
    vfloat_t v[3];
};

/* 4x4 matrix value type */
struct mat4
{
    vfloat_t m[4][4];
};

/* ray cast at some direction from some position */
typedef struct
{
    struct vec3 position;
    struct vec3 direction;
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
