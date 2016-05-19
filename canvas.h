#ifndef CANVAS_H_
#define CANVAS_H_

#include "types.h"

/* Draws image to screen.
 * pixmap - final image in RGBA8888 pixels
 */
int draw(size_t width, size_t height, pixel_t *pixmap);

#endif
