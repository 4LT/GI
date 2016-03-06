#ifndef MATERIALS_H_
#define MATERIALS_H_

#include "vecmatops.h"
#include "material.h"
#include "light.h"
#include <stdio.h>

color_t flat_shade(intersect_result_t res, llist_t *lights)
{
    return res.material->diffuse_color;
}

color_t phong_shade(intersect_result_t res, llist_t *lights)
{
    color_t out_color = (color_t) { 0, 0, 0 };

    for (llist_node_t *node = lights->first; node != NULL;
            node = node->next)
    {
        light_t *light = (light_t *)(node->datum);
        struct vec3 surf2light = v3_sub(light->position, res.position);
        vfloat_t q = 1 +
                (v3_magnitude(surf2light) - light->radius) /
                light->radius;
        vfloat_t attenuation = 1 / q / q;
        
        struct vec3 incoming = v3_normalize(surf2light);
        vfloat_t scale = v3_dot(incoming, res.normal) * attenuation;
        scale = scale < 0 ? 0 : scale;

        out_color.red+= scale *
                res.material->diffuse_color.red * light->color.red;
        out_color.green+= scale *
                res.material->diffuse_color.green * light->color.green;
        out_color.blue+= scale *
                res.material->diffuse_color.blue * light->color.blue;
    }
    return out_color;
}

Material_t *phong_new(color_t color, color_t spec_color, float spec_exp)
{
    Material_t *mtrl = (Material_t *) malloc(sizeof(Material_t));
    *mtrl = (Material_t) { phong_shade, color, spec_color, spec_exp };
    return mtrl;
}

void phong_free(Material_t *mtrl)
{
    free(mtrl);
}

Material_t *flat_new(color_t color)
{
    Material_t *mtrl = (Material_t *) malloc(sizeof(Material_t));
    *mtrl = (Material_t) { flat_shade, color, (color_t) {0,0,0}, 1 };
    return mtrl;
}

#endif
