#include "./render.h"

#include <SDL.h>


SDL_Window* g_Window = NULL;
SDL_Renderer* g_Renderer = NULL;


int initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 0;

    Uint32 winFlags = 0;
    g_Window = SDL_CreateWindow("CGomoku", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        1000, 800, winFlags);

    if (g_Window == NULL) return 0;

    Uint32 rendFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    g_Renderer = SDL_CreateRenderer(g_Window, -1, rendFlags);

    if (g_Renderer == NULL) return 0;

    return 1;
}


void quitSDL() {
    SDL_Quit();

    g_Window = NULL;
    g_Renderer = NULL;
}
