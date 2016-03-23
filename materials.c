#include "materials.h"
#include "shapes.h"

color_t fullbright_shade(intersect_result_t res, Light_t *light)
{
    return res.material->diffuse_color;
}

vfloat_t attenuation(vfloat_t radius, vfloat_t distance)
{
    vfloat_t a = 1 +
            (distance - radius) /
            radius;
    return 1 / (a*a);
}

color_t diffuse_light(intersect_result_t res, Light_t *light)
{
    struct vec3 surf2light = v3_sub(light->position, res.position);
    vfloat_t lmag = v3_magnitude(surf2light);
    vfloat_t a = attenuation(light->radius, lmag);
    struct vec3 lnorm = v3_divide(surf2light, lmag);

    color_t diffuse_color = res.material->diffuse_color;
    vfloat_t diffuse_scale = v3_dot(lnorm, res.normal) * a;
    diffuse_scale = diffuse_scale < 0 ? 0 : diffuse_scale;
    return clr_scale(clr_mul(diffuse_color, light->color), diffuse_scale);
}

color_t phong_light(intersect_result_t res, Light_t *light)
{
    struct vec3 lnorm = v3_normalize(v3_sub(light->position, res.position));
    struct vec3 half = v3_normalize(v3_add(
                v3_scale(res.incoming, -1), lnorm));

    color_t specular_color = res.material->specular_color;
    vfloat_t specular_exp = res.material->specular_exp; 
    vfloat_t spec_scale = pow(v3_dot(half, res.normal), specular_exp);
    spec_scale = spec_scale < 0 ? 0 : spec_scale;
    return clr_scale(clr_mul(specular_color, light->color), spec_scale);
}

color_t lambert_shade(intersect_result_t res, Light_t *light)
{
    return diffuse_light(res, light);
}

color_t phong_shade(intersect_result_t res, Light_t *light)
{
    return clr_add(diffuse_light(res, light), phong_light(res, light));
}

Material_t *phong_new(color_t color, color_t spec_color, float spec_exp)
{
    Material_t *mtrl = (Material_t *) malloc(sizeof(Material_t));
    *mtrl = (Material_t) { phong_shade, color, spec_color, spec_exp };
    return mtrl;
}

Material_t *lambert_new(color_t color)
{
    Material_t *mtrl = (Material_t *) malloc(sizeof(Material_t));
    *mtrl = (Material_t) { lambert_shade, color, CLR_BLACK, 1 };
    return mtrl;
}

Material_t *fullbright_new(color_t color)
{
    Material_t *mtrl = (Material_t *) malloc(sizeof(Material_t));
    *mtrl = (Material_t) { fullbright_shade, color, CLR_BLACK , 1 };
    return mtrl;
}
