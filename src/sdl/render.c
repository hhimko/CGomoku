#include "./sdl/render.h"


int initSDL(SDL_Renderer** rend, SDL_Window** win) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return -1;

    Uint32 winFlags = 0;
    *win = SDL_CreateWindow("CGomoku", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        1000, 800, winFlags);
    if (win == NULL) return -1;

    Uint32 rendFlags = SDL_RENDERER_ACCELERATED;
    *rend = SDL_CreateRenderer(*win, -1, rendFlags);
    if (rend == NULL) return -1;

    return 0;
}

void quitSDL(SDL_Renderer* rend, SDL_Window* win) {
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);

    SDL_Quit();
}
