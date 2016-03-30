#ifndef MATERIALS_H_
#define MATERIALS_H_

#include "vecmatops.h"
#include "material.h"
#include "light.h"
#include <stdio.h>
#include <tgmath.h>
#include <stdlib.h>

color_t fullbright_shade(intersect_result_t res, light_t *light);

color_t lambert_shade(intersect_result_t res, light_t *light);

color_t phong_shade(intersect_result_t res, light_t *light);

Material_t *phong_new(color_t color, color_t spec_color, float spec_exp);

Material_t *lambert_new(color_t color);

Material_t *fullbright_new(color_t color);

Material_t *tile_new();

#endif
