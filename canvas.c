#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdbool.h>
#include "canvas.h"

static int sdlErr()
{
    fprintf(stderr, "SDL error: %s\n", SDL_GetError());
    return EXIT_FAILURE;
}

int draw(size_t width, size_t height, pixel_t *pixmap)
{
    SDL_Window *win = NULL;
    SDL_Surface *winSurf = NULL;
    SDL_Surface *inputSurf = NULL;
    size_t px_sz = sizeof(pixel_t);
    pixel_t *flipped_pix = malloc(width * height * px_sz);
    
    const uint32_t R_MASK = 0xFF000000;
    const uint32_t G_MASK = 0xFF0000;
    const uint32_t B_MASK = 0xFF00;
    const uint32_t A_MASK = 0xFF;

    for (int r = 0; r < height; r++) {
        memcpy(flipped_pix + r*width, pixmap + (height-r)*width, width * px_sz);
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
        return sdlErr();

    win = SDL_CreateWindow("Ray Tracer", SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, width, height, 0);
    if (win == NULL)
        return sdlErr();

    winSurf = SDL_GetWindowSurface(win);
    if (winSurf == NULL)
        return sdlErr();
    
    inputSurf = SDL_CreateRGBSurfaceFrom((void*)flipped_pix, width, height,
            8 * px_sz,
            width * px_sz,
            R_MASK, G_MASK, B_MASK, A_MASK);
    if (inputSurf == NULL)
        return sdlErr();
    
    inputSurf = SDL_ConvertSurface(inputSurf, winSurf->format, 0);
    if (inputSurf == NULL)
        return sdlErr();

    if (SDL_BlitSurface(inputSurf, NULL, winSurf, NULL) < 0)
        return sdlErr();

    SDL_Event event;
    bool done = false;

    while (!done) {
        if (SDL_UpdateWindowSurface(win) < 0)
            return sdlErr();
        SDL_Delay(20);
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_WINDOWEVENT)
            if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
    }
    SDL_Quit();
    free(flipped_pix);

    return EXIT_SUCCESS;
}
