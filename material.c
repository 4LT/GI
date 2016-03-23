#include "material.h"

color_t shade(intersect_result_t res, Light_t *light)
{
    return res.material->shade(res, light);
}
