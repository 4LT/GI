/* Radiosity implementation.  Takes a set of unlit patches, applies radiosity,
 * and outputs patches with lighting
 */
#include <stdlib.h>
#include <scene.h>
#include <canvas.h>
#include <tone_mapping.h>
#include "hemicube.h"
#include "patch.h"
#include "read_patches.h"

int main(int argc, char *argv[])
{
    PatchSet_t *pset = PatchSet_new(); 
    patch_header_t header;
    patch_read_for_rad("coarse.txt", pset, &header);
    PatchSet_compute(pset);
    patch_write_for_draw("coarse_out.txt", pset, &header);
}

