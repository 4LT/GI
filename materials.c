#include <tgmath.h>
#include "materials.h"
#include "shapes.h"
#include "scene.h"

static const int TILE_SIZE = 20;
static const double PI2 = 2 * 3.14159;
static const vfloat_t FUDGE = 0.001;

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

color_t shade_fullbright(intersect_result_t res)
{
    return res.material->diffuse_color;
}

color_t ambient_illum(intersect_result_t res, color_t diffuse, light_t *light)
{
    return clr_mul(diffuse, light->color);
}

color_t diffuse_illum(intersect_result_t res, color_t diffuse, light_t *light)
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

color_t noisy_sample(Material_t *mtrl, vfloat_t x, vfloat_t y)
{
    const vfloat_t WAV_LEN = TILE_SIZE * 1.5;
    const vfloat_t AMP = TILE_SIZE / 2;
    vfloat_t oldY = y;
    y = y + AMP * cos(PI2 * x / WAV_LEN / 2) / 2;
    x = x + AMP * cos(PI2 * oldY / WAV_LEN / 2) / 2;

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
    return out_color;
}

color_t concentric_sample(Material_t *mtrl, vfloat_t x, vfloat_t y)
{
    const color_t GREY = {{ 0.2, 0.2, 0.2 }};
    const color_t GREEN = {{ 0, 1, 0 }};
    const vfloat_t THICK = 2;
    struct vec3 P1 = {{ -10, -40, 0 }}, P2 = {{ 30, 50, 0 }};
    struct vec3 xyv = {{ x/2, y/2, 0 }};
    vfloat_t r1 = v3_magnitude(v3_sub(P1, xyv)) / THICK / 2;
    vfloat_t r2 = v3_magnitude(v3_sub(P2, xyv)) / THICK / 2;
    vfloat_t scale = cos(PI2 * r1) + cos(PI2 * r2);
    scale = (scale + 2)/4;
    return clr_add(clr_scale(GREY, scale), clr_scale(GREEN, 1-scale));
}

color_t specular_illum(intersect_result_t res, light_t *light)
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
    if (light->type == AMBIENT)
        return ambient_illum(res, res.material->diffuse_color, light);
    else if (shadow_test(res, light)) 
        return CLR_BLACK;
    else
        return diffuse_illum(res, res.material->diffuse_color, light);
}

color_t tile_shade(intersect_result_t res, light_t *light)
{
    vfloat_t x = res.position.v[0], y = res.position.v[1];
    color_t color = res.material->diffuse_sample(res.material, x, y);
    if (light->type == AMBIENT)
        return ambient_illum(res, color, light);
    else if (shadow_test(res, light))
        return CLR_BLACK;
    else
        return diffuse_illum(res, color, light);
}

color_t phong_shade(intersect_result_t res, light_t *light)
{
    if (light->type == AMBIENT)
        return ambient_illum(res, res.material->diffuse_color, light);
    else if (shadow_test(res, light))
        return CLR_BLACK;
    else
        return clr_add(diffuse_illum(res, res.material->diffuse_color, light),
                specular_illum(res, light));
}

color_t reflect(intersect_result_t res)
{
    if (res.depth == 0)
        return CLR_BLACK;

    color_t out_color = CLR_BLACK;
    Material_t *mtrl = res.material;
    struct vec3 normal = res.normal;

    /* axis-aligned vector such that (aa x normal) is never (0, 0, 0) */
    struct vec3 aa = (struct vec3) {{ 1, 0, 0 }};
    if (fabs(normal.v[0]) > fabs(normal.v[1]))
        aa = (struct vec3) {{ 0, 1, 0 }};

    struct vec3 unit_x = v3_normalize(v3_cross(normal, aa));
    struct vec3 unit_y = v3_cross(normal, unit_x);
    
    const bool UNIFORM = false;
    for (int i = 0; i < mtrl->reflect_ray_count; i++) {

        float theta = PI2 * rand_float();
        float r = rand_float();
        if (UNIFORM) r = sqrt(r);
        r*= mtrl->roughness;
        struct vec3 mod_normal = v3_add(normal, v3_add(
                v3_scale(unit_x, r * cos(theta)),
                v3_scale(unit_y, r * sin(theta)) ));
        mod_normal = v3_normalize(mod_normal);
                  
        struct vec3 proj_i = v3_project(mod_normal, res.incoming);
        struct vec3 proj_r = v3_add(proj_i,
                v3_scale(v3_sub(mod_normal, proj_i),2));
        ray_t reflected_ray;
        reflected_ray.direction = v3_normalize(proj_r);
        reflected_ray.position = v3_add(res.position,
                v3_scale(normal, FUDGE));
        
        color_t reflect_color = color_at_rec(*(mtrl->scene), reflected_ray,
                res.depth - 1);
        out_color = clr_add(out_color, reflect_color);
    }

    return clr_scale(out_color, (vfloat_t)1/mtrl->reflect_ray_count);
}

color_t recursive(intersect_result_t);

color_t refract(intersect_result_t res)
{
    if (res.depth == 0)
        return CLR_BLACK;

    Material_t *mtrl = res.material;

    /* assume we're in air or space (atm ior = 1) */
    vfloat_t refract_ratio = res.exit ? mtrl->ior : 1/mtrl->ior;

    vfloat_t cos_theta1 = v3_dot(v3_scale(res.incoming, -1), res.normal);
    vfloat_t discrim = 1 +
        (refract_ratio*refract_ratio * (cos_theta1*cos_theta1 - 1));

    if (discrim <= 0) return reflect(res);

    struct vec3 transmit = v3_add(v3_scale(res.incoming, refract_ratio),
            v3_scale(res.normal, refract_ratio*cos_theta1 - sqrt(discrim)));

    ray_t refracted_ray;
    refracted_ray.direction = v3_normalize(transmit);
    refracted_ray.position = v3_sub(res.position,
            v3_scale(res.normal, FUDGE));

    return color_at_rec(*(res.material->scene), refracted_ray,
            res.depth - 1);
}

color_t recursive(intersect_result_t res)
{
    color_t out_color = CLR_BLACK;
    Material_t *mtrl = res.material;
    if (mtrl->reflect_scale != 0) {
        out_color = clr_add(out_color, clr_scale(reflect(res),
                    mtrl->reflect_scale));
    }
    if (mtrl->transmit_scale != 0) {
        out_color = clr_add(out_color, clr_scale(refract(res),
                    mtrl->transmit_scale));
    }
    return out_color;
}

Material_t *phong_new(struct scene *scene, color_t color, color_t spec_color,
        float spec_exp)
{
    Material_t *mtrl = (Material_t *) malloc(sizeof(Material_t));
    *mtrl = DEF_MTRL;
    mtrl->scene = scene;
    mtrl->shade_per_light = phong_shade;
    mtrl->diffuse_color = color;
    mtrl->specular_color = spec_color;
    mtrl->specular_exp = spec_exp;
    return mtrl; 
}

Material_t *lambert_new(struct scene *scene, color_t color)
{
    Material_t *mtrl = (Material_t *) malloc(sizeof(Material_t));
    *mtrl = DEF_MTRL;
    mtrl->scene = scene;
    mtrl->shade_per_light = lambert_shade;
    mtrl->diffuse_sample = solid_sample;
    mtrl->diffuse_color = color;
    return mtrl;
}

Material_t *fullbright_new(struct scene *scene, color_t color)
{
    Material_t *mtrl = (Material_t *) malloc(sizeof(Material_t));
    *mtrl = DEF_MTRL;
    mtrl->scene = scene;
    mtrl->shade_once = shade_fullbright;
    mtrl->diffuse_color = color;
    return mtrl;
}

Material_t *tile_new(struct scene *scene)
{
    Material_t *mtrl = (Material_t *) malloc(sizeof(Material_t));
    *mtrl = DEF_MTRL;
    mtrl->scene = scene;
    mtrl->shade_per_light = tile_shade;
    mtrl->diffuse_sample = tile_sample;
    return mtrl;
}

Material_t *shiny_new(struct scene *scene, color_t color, color_t spec_color,
        float spec_exp, float reflect_scale, float roughness, int ray_count)
{
    Material_t *mtrl = phong_new(scene, color, spec_color, spec_exp);
    mtrl->reflect_scale = reflect_scale;
    mtrl->shade_once = recursive;
    mtrl->roughness = roughness;
    mtrl->reflect_ray_count = ray_count;
    return mtrl;
}

Material_t *refr_new(struct scene *scene, color_t color, color_t spec_color,
        float spec_exp, float reflect_scale, float transmit_scale, float ior)
{
    Material_t *mtrl = shiny_new(scene, color, spec_color, spec_exp,
            reflect_scale, 0, 1);
    mtrl->transmit_scale = transmit_scale;
    mtrl->ior = ior;
    return mtrl;
}

Material_t *noisy_tile_new(struct scene *scene)
{
    Material_t *mtrl = (Material_t *) malloc(sizeof(Material_t));
    *mtrl = DEF_MTRL;
    mtrl->scene = scene;
    mtrl->shade_per_light = tile_shade;
    mtrl->diffuse_sample = noisy_sample;
    return mtrl;
}

Material_t *concentric_new(struct scene *scene)
{
    Material_t *mtrl = (Material_t *) malloc(sizeof(Material_t));
    *mtrl = DEF_MTRL;
    mtrl->scene = scene;
    mtrl->shade_per_light = tile_shade;
    mtrl->diffuse_sample = concentric_sample;
    return mtrl;
}
