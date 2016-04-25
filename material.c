#include "material.h"

color_t shade_light(intersect_result_t res, light_t *light)
{
    return res.material->shade_per_light(res, light);
}

color_t shade_once(intersect_result_t res)
{
    return res.material->shade_once(res);
}
