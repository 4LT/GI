#include "materials.h"
#include "shapes.h"

struct vec3 fudge(intersect_result_t res)
{
    return v3_add(res.position, v3_scale(res.normal, 0.001));
}

color_t flat_shade(intersect_result_t res, llist_t *shapes, llist_t *lights)
{
    float scaleb = 1;
    for (llist_node_t *node = lights->first; node != NULL;
            node = node->next)
    {
        light_t *light = (light_t *)(node->datum);
        struct vec3 surf2light = v3_sub(light->position, res.position);
        ray_t lray = (ray_t){ fudge(res), v3_normalize(surf2light) };

        int boolbreak = 0;
        for (llist_node_t *node = shapes->first; node != NULL; node = node->next)
        {
            Shape_t *shape = (Shape_t *)(node->datum);
            intersect_result_t r = intersect_shape(shape, lray);
            if (r.distance > 0) {
                boolbreak = 1;
                break;
            }
        }

        if (boolbreak) 
        {
            scaleb*= .7;
        }
    }
    return clr_scale(res.material->diffuse_color, scaleb);
}

color_t diffuse_light(struct vec3 normal, struct vec3 incident,
        Material_t *mtrl, color_t light_color, float attenuation)
{
    vfloat_t diffuse_scale = v3_dot(incident, normal)*attenuation;
    diffuse_scale = diffuse_scale < 0 ? 0 : diffuse_scale;
    return clr_scale(clr_mul(mtrl->diffuse_color, light_color), diffuse_scale);
}

color_t phong_light(struct vec3 normal, struct vec3 half_vector,
        Material_t *mtrl, color_t light_color)
{
    vfloat_t spec_scale = pow(v3_dot(half_vector, normal), mtrl->specular_exp);
    spec_scale = spec_scale < 0 ? 0 : spec_scale;
    return clr_scale(clr_mul(mtrl->specular_color, light_color), spec_scale);
}

color_t lambert_shade(intersect_result_t res, llist_t *shapes, llist_t *lights)
{
    color_t out_color = (color_t) {{ 0, 0, 0 }};

    for (llist_node_t *node = lights->first; node != NULL;
            node = node->next)
    {
        light_t *light = (light_t *)(node->datum);
        struct vec3 surf2light = v3_sub(light->position, res.position);
        vfloat_t lmag = v3_magnitude(surf2light);
        struct vec3 incident = v3_divide(surf2light, lmag);

        vfloat_t a = 1 +
                (lmag - light->radius) /
                light->radius;
        vfloat_t attenuation = 1 / a / a;

        out_color = clr_add(out_color,
                diffuse_light(res.normal, incident, res.material, light->color,
                attenuation));
    }
    return out_color;
}

color_t phong_shade(intersect_result_t res, llist_t *shapes, llist_t *lights)
{
    color_t out_color = (color_t) {{ 0, 0, 0 }};

    for (llist_node_t *node = lights->first; node != NULL;
            node = node->next)
    {
        light_t *light = (light_t *)(node->datum);
        struct vec3 surf2light = v3_sub(light->position, res.position);
        ray_t lray = (ray_t){ fudge(res), v3_normalize(surf2light) };

        int boolbreak = 0;
        for (llist_node_t *node = shapes->first; node != NULL; node = node->next)
        {
            Shape_t *shape = (Shape_t *)(node->datum);
            intersect_result_t r = intersect_shape(shape, lray);
            if (r.distance > 0) {
                boolbreak = 1;
                break;
            }
        }

        if (!boolbreak) 
        {

        struct vec3 sample = v3_scale(res.ray.direction, -1);
        vfloat_t lmag = v3_magnitude(surf2light);
        struct vec3 incident = v3_divide(surf2light, lmag);
        struct vec3 half = v3_normalize(v3_add(sample, incident));

        vfloat_t a = 1 +
                (lmag - light->radius) /
                light->radius;
        vfloat_t attenuation = 1 / a / a;

        out_color = clr_add(out_color,
                diffuse_light(res.normal, incident, res.material, light->color,
                attenuation));
        out_color = clr_add(out_color,
                phong_light(res.normal, half, res.material, light->color));
        }
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

Material_t *lambert_new(color_t color)
{
    Material_t *mtrl = (Material_t *) malloc(sizeof(Material_t));
    *mtrl = (Material_t) { lambert_shade, color, CLR_BLACK, 1 };
    return mtrl;
}

Material_t *flat_new(color_t color)
{
    Material_t *mtrl = (Material_t *) malloc(sizeof(Material_t));
    *mtrl = (Material_t) { flat_shade, color, CLR_BLACK , 1 };
    return mtrl;
}
