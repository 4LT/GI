#include <stdlib.h>
#include "scene.h"
#include "canvas.h"
#include "tone_mapping.h"
#include "hemicube.h"
#include "patch.h"

int main(int argc, char *argv[])
{
    scene_t scene = scene_empty_scene(CLR_BLACK);

    PatchSet_t *pset; 
    patch_read_to_set("mymesh.txt", pset);

}
