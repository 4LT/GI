#ifndef MATERIALS_H_
#define MATERIALS_H_

#include "vecmatops.h"
#include "material.h"
#include "light.h"
#include <stdio.h>
#include <tgmath.h>
#include <stdlib.h>

color_t flat_shade(intersect_result_t res, llist_t *shapes, llist_t *lights);

color_t lambert_shade(intersect_result_t res, llist_t *shapes, llist_t *lights);

color_t phong_shade(intersect_result_t res, llist_t *shapes, llist_t *lights);

Material_t *phong_new(color_t color, color_t spec_color, float spec_exp);

void phong_free(Material_t *mtrl);

Material_t *lambert_new(color_t color);

Material_t *flat_new(color_t color);

#endif
