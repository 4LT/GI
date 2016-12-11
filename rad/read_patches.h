#ifndef READ_PATCHES_H_
#define READ_PATCHES_H_

#include <stdio.h>
#include <scene.h>
#include "patch.h"

typedef struct patch_header
{
    /* camera parameters */
    vec3_t cam_pos;
    vec3_t cam_look;
    vec3_t cam_up;
    /* image dimensions */
    int width;
    int height;
    /* hemicube size */
    int hc_width;
    int hc_height;
    /* number of maximum radiosity lighting passes */
    int max_iter;
} patch_header_t;

/* Read a patch file for the purpose of drawing to screen using the given scene
 * and camera */
void patch_read_for_draw(const char *file_name, scene_t *scene, camera_t *cam);

/* Read a patch file and perform radiosity, and store the data in a patch set */
void patch_read_for_rad(const char* file_name, PatchSet_t *pset, patch_header_t
        *header);

void patch_write_for_draw(const char *file_name, PatchSet_t *pset,
        const patch_header_t *header);

#endif
