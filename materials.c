#include "materials.h"
#include "shapes.h"

static const int TILE_SIZE = 20;

color_t fullbright_shade(intersect_result_t res, light_t *light)
{
    return res.material->diffuse_color;
}

vfloat_t attenuation(vfloat_t radius, vfloat_t distance)
{
    /* derived from
     * https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/
     * (approximation of spherical light sources)
     * attenuation = 1 / (d/r + 1)^2
     * where r = radius, d = distance - radius
     */
    return radius * radius / (distance * distance);
}

color_t diffuse_light(intersect_result_t res, color_t diffuse, light_t *light)
{
    struct vec3 lnorm;
    vfloat_t a;
    if (light->type == ORTHO) {
        lnorm = v3_scale(light->direction, -1);
        a = 1;
    }
    else {
        struct vec3 surf2light = v3_sub(light->position, res.position);
        vfloat_t lmag = v3_magnitude(surf2light);
        a = attenuation(light->radius, lmag);
        lnorm = v3_divide(surf2light, lmag);
    }

    float_t diffuse_scale = v3_dot(lnorm, res.normal) * a;
    diffuse_scale = diffuse_scale < 0 ? 0 : diffuse_scale;
    return clr_scale(clr_mul(diffuse, light->color), diffuse_scale);
}

color_t tile_sample(Material_t *mtrl, vfloat_t x, vfloat_t y)
{
    const color_t RED = (color_t) {{ 1, 0, 0 }};
    const color_t YELLOW = (color_t) {{ 1, 1, 0 }};
    x = x / TILE_SIZE / 2;
    y = y / TILE_SIZE / 2;
    return ((x - floor(x) < 0.5) ^ (y - floor(y) < 0.5)) ? RED : YELLOW;
}

color_t solid_sample(Material_t *mtrl, vfloat_t x, vfloat_t y)
{
    return mtrl->diffuse_color;
}

color_t phong_light(intersect_result_t res, light_t *light)
{
    struct vec3 lnorm;
    if (light->type == ORTHO)
        lnorm = v3_scale(light->direction, -1);
    else
        lnorm = v3_normalize(v3_sub(light->position, res.position));

    struct vec3 half = v3_normalize(v3_add(
                v3_scale(res.incoming, -1), lnorm));

    color_t specular_color = res.material->specular_color;
    vfloat_t specular_exp = res.material->specular_exp; 
    vfloat_t spec_scale = pow(v3_dot(half, res.normal), specular_exp);
    spec_scale = spec_scale < 0 ? 0 : spec_scale;
    return clr_scale(clr_mul(specular_color, light->color), spec_scale);
}

color_t lambert_shade(intersect_result_t res, light_t *light)
{
    return diffuse_light(res, res.material->diffuse_color, light);
}

color_t tile_shade(intersect_result_t res, light_t *light)
{
    vfloat_t x = res.position.v[0], y = res.position.v[1];
    color_t color = res.material->diffuse_sample(res.material, x, y);
    return diffuse_light(res, color, light);
}

color_t phong_shade(intersect_result_t res, light_t *light)
{
    return clr_add(lambert_shade(res, light), phong_light(res, light));
}

Material_t *phong_new(struct scene *scene, color_t color, color_t spec_color,
        float spec_exp)
{
    Material_t *mtrl = (Material_t *) malloc(sizeof(Material_t));
    *mtrl = (Material_t) { scene, phong_shade, solid_sample,
            color, spec_color, spec_exp };
    return mtrl;
}

Material_t *lambert_new(struct scene *scene, color_t color)
{
    Material_t *mtrl = (Material_t *) malloc(sizeof(Material_t));
    *mtrl = (Material_t) { scene, lambert_shade, solid_sample,
            color, CLR_BLACK, 1 };
    return mtrl;
}

Material_t *fullbright_new(struct scene *scene, color_t color)
{
    Material_t *mtrl = (Material_t *) malloc(sizeof(Material_t));
    *mtrl = (Material_t) { scene, fullbright_shade, solid_sample,
            color, CLR_BLACK , 1 };
    return mtrl;
}

Material_t *tile_new(struct scene *scene)
{
    Material_t *mtrl = (Material_t *) malloc(sizeof(Material_t));
    *mtrl = (Material_t) { scene, tile_shade, tile_sample,
            CLR_WHITE, CLR_BLACK, 1 };
    return mtrl;
}
