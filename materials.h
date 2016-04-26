#ifndef MATERIALS_H_
#define MATERIALS_H_

#include "vecmatops.h"
#include "material.h"
#include "light.h"
#include <stdio.h>
#include <tgmath.h>
#include <stdlib.h>

Material_t *phong_new(struct scene *scene, color_t color, color_t spec_color,
        float spec_exp);

Material_t *lambert_new(struct scene *scene, color_t color);

Material_t *fullbright_new(struct scene *scene, color_t color);

Material_t *tile_new(struct scene *scene);

Material_t *shiny_new(struct scene *scene, color_t color, color_t spec_color,
        float spec_exp, float reflect_scale, float roughness, int ray_count);

Material_t *refr_new(struct scene *scene, color_t color, color_t spec_color,
        float spec_exp, float reflect_scale, float transmit_scale, float ior);

Material_t *noisy_tile_new(struct scene *scene);

Material_t *concentric_new(struct scene *scene);

#endif
