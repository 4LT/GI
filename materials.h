#ifndef MATERIALS_H_
#define MATERIALS_H_

#include "material.h"

color_t flat_shade(intersect_result_t res)
{
    return res.material->diffuse_color;
}

color_t phong_shade(intersect_result_t res)
{
    return res.material->diffuse_color;
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
