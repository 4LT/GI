#ifndef CAMERA_H_
#define CAMERA_H_

#include "types.h"

/* camera for rendering a scene */
typedef struct
{
    /* orientation of camera */
    vec3_t pos;
    vec3_t up;
    vec3_t lookAt;

    /* properties for projection onto a plane */
    vfloat_t plane_width;
    vfloat_t focal_length; 

    int img_width;
    int img_height;
    int x_off;
    int y_off;
} camera_t;

static inline camera_t cam_centered(vec3_t pos, vec3_t up, vec3_t lookAt,
        int img_width, int img_height)
{
    return (camera_t) {
        .pos = pos,
        .up = up,
        .lookAt = lookAt,
        .plane_width = 2,
        .focal_length = 1,
        .img_width = img_width,
        .img_height = img_height,
        .x_off = 0,
        .y_off = 0
    };
}

static inline void cam_set_projection(camera_t *cam,
        vfloat_t plane_width, vfloat_t focal_length)
{
    cam->plane_width = plane_width;
    cam->focal_length = focal_length;
}

static inline void cam_set_offset(camera_t *cam, int x_off, int y_off)
{
    cam->x_off = x_off;
    cam->y_off = y_off;
}

#endif
