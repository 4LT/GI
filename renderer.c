#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdbool.h>

#include "renderer.h"

#include <assert.h>

static int sdlErr()
{
    fprintf(stderr, "SDL error: %s\n", SDL_GetError());
    return EXIT_FAILURE;
}

int draw(unsigned int width, unsigned int height, pixel_t *pixmap)
{
    SDL_Window *win = NULL;
    SDL_Surface *winSurf = NULL;
    SDL_Surface *inputSurf = NULL;
    
    const uint32_t R_MASK = 0xFF000000;
    const uint32_t G_MASK = 0xFF0000;
    const uint32_t B_MASK = 0xFF00;
    const uint32_t A_MASK = 0xFF;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
        return sdlErr();

    win = SDL_CreateWindow("Ray Tracer", SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, width, height, 0);

    if (win == NULL)
        return sdlErr();

    winSurf = SDL_GetWindowSurface(win);
    
    inputSurf = SDL_CreateRGBSurfaceFrom((void*)pixmap, width, height,
            8 * sizeof *pixmap,
            width * sizeof *pixmap,
            R_MASK, G_MASK, B_MASK, A_MASK);
    
    inputSurf = SDL_ConvertSurface(inputSurf, winSurf->format, 0);

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

    return EXIT_SUCCESS;
}
