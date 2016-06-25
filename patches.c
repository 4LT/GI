#include "read_patches.h"
#include "tone_mapping.h"
#include "canvas.h"

static const int SCREEN_W = 256;
static const int SCREEN_H = 256;

int main(int argc, char *argv[])
{
    scene_t *scene = patch_read_file("mymesh.txt");
    size_t pix_count = SCREEN_W * SCREEN_H;
    color_t *img = malloc(pix_count * sizeof(color_t));
    scene_gen_kdtree(scene);
    scene_render(scene, SCREEN_W, SCREEN_H, img);

    pixel_t *pixmap = malloc(pix_count * sizeof(pixel_t));
    tonemap_nop(img, pix_count, pixmap);
    free(img);
    int exit_status = draw(SCREEN_W, SCREEN_H, pixmap);
    free(pixmap);

    return exit_status;
}
