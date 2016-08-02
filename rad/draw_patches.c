#include "read_patches.h"
#include "tone_mapping.h"
#include "canvas.h"

int main(int argc, char *argv[])
{
    scene_t scene;
    camera_t cam;
    patch_read_file("mymesh.txt", &scene, &cam);
    int screen_w = cam.img_width;
    int screen_h = cam.img_height;
    size_t pix_count = screen_w * screen_h;
    color_t *img = malloc(pix_count * sizeof(color_t));
    scene_gen_kdtree(&scene, 0.15);
    scene_render(&scene, &cam, img);

    pixel_t *pixmap = malloc(pix_count * sizeof(pixel_t));
    tonemap_nop(img, pix_count, pixmap);
    free(img);
    int exit_status = draw(screen_w, screen_h, pixmap);
    free(pixmap);

    return exit_status;
}
