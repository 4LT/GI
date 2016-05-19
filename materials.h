/* materials.h
 *
 * A collection of material types and shading methods.
 * author: Seth Rader
 */
#ifndef MATERIALS_H_
#define MATERIALS_H_

#include "vecmatops.h"
#include "material.h"
#include "light.h"
#include <stdio.h>
#include <tgmath.h>
#include <stdlib.h>

/* Create a specular material with a diffuse component */
Material_t *phong_new(struct scene *scene, color_t color, color_t spec_color,
        float spec_exp);

/* Create a diffusely-lit material */
Material_t *lambert_new(struct scene *scene, color_t color);

/* Create a material that is not lit by the scene, and is not darkened */
Material_t *fullbright_new(struct scene *scene, color_t color);

/* Create a red-and-yellow tiled surface */
Material_t *tile_new(struct scene *scene);

/* Create a material that reflects rays of light */
Material_t *shiny_new(struct scene *scene, color_t color, color_t spec_color,
        float spec_exp, float reflect_scale, float roughness, int ray_count);

/* Create a material that refract and reflects light */
Material_t *refr_new(struct scene *scene, color_t color, color_t spec_color,
        float spec_exp, float reflect_scale, float transmit_scale, float ior);

/* Create a tiled material with random colors */
Material_t *noisy_tile_new(struct scene *scene);

/* create a material with two sets of concentric rings */
Material_t *concentric_new(struct scene *scene);

#endif
