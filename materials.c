#include "materials.h"
#include "shapes.h"
#include "scene.h"

static const int TILE_SIZE = 20;
#if 0
static const double PI2 = atan(-1) * 8;
#else
static const double PI2 = 2 * 3.14159;
#endif

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

float rand_float() {
    return (float)rand() / RAND_MAX;
}

color_t fullbright_shade(intersect_result_t res, light_t *light)
{
    return res.material->diffuse_color;
}

color_t diffuse_light(intersect_result_t res, color_t diffuse, light_t *light)
{
    if (light->type == AMBIENT)
        return clr_mul(diffuse, light->color);
    if (shadow_test(res, light))
        return CLR_BLACK;

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

color_t noisy_sample(Material_t *mtrl, vfloat_t x, vfloat_t y)
{
    int tx = (int)floor(x / TILE_SIZE);
    int ty = (int)floor(y / TILE_SIZE);
    srand(tx);
    /* fix for srand(0) and srand(1) having same effect in glibc */
    if (tx == 0)
        srand(rand());
    srand(ty + rand());

    /* generate a random, fully-saturated color */
    color_t out_color;
    int low_i = rand() % 3;
    int high_i = (rand() % 2 + 1 + low_i) % 3;
    int mid_i = 2 * (low_i + high_i) % 3;
    out_color.c[low_i] = 0;
    out_color.c[high_i] = 1;
    out_color.c[mid_i] = rand() / (clrfloat_t)RAND_MAX;
#if 1
    return out_color;
#else
#endif
}

color_t solid_sample(Material_t *mtrl, vfloat_t x, vfloat_t y)
{
    return mtrl->diffuse_color;
}

color_t phong_light(intersect_result_t res, light_t *light)
{
    if (shadow_test(res, light) || light->type == AMBIENT)
        return CLR_BLACK;

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

color_t shiny_shade(intersect_result_t res, light_t *light)
{
    Material_t *mtrl = res.material;
    color_t out_color = CLR_BLACK;
    struct vec3 normal = res.normal;

    out_color = clr_add(out_color, lambert_shade(res, light));
    out_color = clr_add(out_color, phong_light(res, light));

    if (light->type != AMBIENT)
        return out_color;

    struct vec3 aa = (struct vec3) {{ 1, 0, 0 }};
    if (fabs(normal.v[0]) > fabs(normal.v[1]))
        aa = (struct vec3) {{ 0, 1, 0 }};

    struct vec3 unit_x = v3_normalize(v3_cross(normal, aa));
    struct vec3 unit_y = v3_cross(normal, unit_x);
    
    const vfloat_t SPREAD = 0;
    const int RAYCOUNT = 1;
    const bool UNIFORM = false;
    for (int i = 0; i < RAYCOUNT; i++) {

        float theta = PI2 * rand_float();
        float r = rand_float();
        if (UNIFORM) r = sqrt(r);
        r*= SPREAD;
        struct vec3 mod_normal = v3_add(normal, v3_add(
                v3_scale(unit_x, r * cos(theta)),
                v3_scale(unit_y, r * sin(theta)) ));
        mod_normal = v3_normalize(mod_normal);
                  
        struct vec3 proj_i = v3_project(mod_normal, res.incoming);
        struct vec3 proj_r = v3_add(proj_i,
                v3_scale(v3_sub(mod_normal, proj_i),2));
        ray_t reflected_ray = (ray_t){ res.position, v3_normalize(proj_r) };
        reflected_ray.position = v3_add(reflected_ray.position,
                v3_scale(reflected_ray.direction, .001));
        
        out_color = clr_add(out_color,
                clr_scale(color_at(*(mtrl->scene), reflected_ray),
                mtrl->reflect_scale / RAYCOUNT));
    }

    return out_color;
}

Material_t *phong_new(struct scene *scene, color_t color, color_t spec_color,
        float spec_exp)
{
    Material_t *mtrl = (Material_t *) malloc(sizeof(Material_t));
    *mtrl = (Material_t) {
        .scene = scene,
        .shade = phong_shade,
        .diffuse_sample = solid_sample,
        .diffuse_color = color,
        .specular_color = spec_color,
        .specular_exp = spec_exp
    };
    return mtrl; 
}

Material_t *lambert_new(struct scene *scene, color_t color)
{
    Material_t *mtrl = (Material_t *) malloc(sizeof(Material_t));
    *mtrl = (Material_t) {
        .scene = scene,
        .shade = lambert_shade,
        .diffuse_sample = solid_sample,
        .diffuse_color = color
    };
    return mtrl;
}

Material_t *fullbright_new(struct scene *scene, color_t color)
{
    Material_t *mtrl = (Material_t *) malloc(sizeof(Material_t));
    *mtrl = (Material_t) {
        .scene = scene,
        .shade = fullbright_shade,
        .diffuse_sample = solid_sample,
        .diffuse_color = color
    };
    return mtrl;
}

Material_t *tile_new(struct scene *scene)
{
    Material_t *mtrl = (Material_t *) malloc(sizeof(Material_t));
    *mtrl = (Material_t) {
        .scene = scene,
        .shade = tile_shade,
        .diffuse_sample = tile_sample
    };
    return mtrl;
}

Material_t *shiny_new(struct scene *scene, color_t color, color_t spec_color,
        float spec_exp, float reflect_scale)
{
    Material_t *mtrl = phong_new(scene, color, spec_color, spec_exp);
    mtrl->reflect_scale = reflect_scale;
    mtrl->shade = shiny_shade;
    return mtrl;
}

Material_t *noisy_tile_new(struct scene *scene)
{
    Material_t *mtrl = (Material_t *) malloc(sizeof(Material_t));
    *mtrl = (Material_t) { tile_shade, noisy_sample,
            CLR_WHITE, CLR_BLACK, 1 };
    return mtrl;
}
