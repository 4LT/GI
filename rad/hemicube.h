#ifndef HEMICUBE_H_
#define HEMICUBE_H_

#include "scene.h"
#include "patch.h"

patchff_t *Hcube_get_ffs(const scene_t *scene, Patch_t *this_patch,
        size_t *out_count);

#endif
