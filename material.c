#include "material.h"

color_t shade(intersect_result_t res, llist_t *shapes, llist_t *lights)
{
    return res.material->shade(res, shapes, lights);
}
