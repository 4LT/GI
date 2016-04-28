#include "material.h"

color_t shade_nop(intersect_result_t res) { return CLR_BLACK; }
color_t shade_nop_light(intersect_result_t r, light_t *l) { return CLR_BLACK; }
color_t solid_sample(Material_t *mtrl, vfloat_t x, vfloat_t y)
{
    return mtrl->diffuse_color;
}


color_t shade_light(intersect_result_t res, light_t *light)
{
    return res.material->shade_per_light(res, light);
}

color_t shade_once(intersect_result_t res)
{
    return res.material->shade_once(res);
}
