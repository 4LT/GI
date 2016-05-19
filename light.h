/** light.h
 *
 * Light source of various types.
 * author: Seth Rader
 */
#ifndef LIGHT_H_
#define LIGHT_H_

#include "color.h"

typedef struct
{
    enum
    {
        POINT,
        SPHERE, /* attenuation affected by size of light source */
        ORTHO, /* parallel rays, i.e. sunlight */
        AMBIENT
    } type;
    struct vec3 position;
    struct vec3 direction; /* not used */
    color_t color;
    vfloat_t radius; /*used by sphere */
} light_t;

#endif
