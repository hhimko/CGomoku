#include "./sdl/render.h"

#include <assert.h>


RenderContext* createRenderContext(int win_w, int win_h) {
    RenderContext* ctx = malloc(sizeof(RenderContext));

    ctx->renderer = NULL; // renderer should only ever be created and assigned in `initSDL`
    ctx->win_w = win_w;
    ctx->win_h = win_h; 

    return ctx;
}

void destroyRenderContext(RenderContext* ctx) {
    assert(ctx->renderer == NULL || "SDL_Renderer* should be freed before calling destroyRenderContext()");
    free(ctx);
}


int initSDL(RenderContext* ctx, SDL_Window** win) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return -1;

    Uint32 winFlags = 0;
    *win = SDL_CreateWindow("CGomoku", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        ctx->win_w, ctx->win_h, winFlags);
    if (win == NULL) return -1;

    Uint32 rendFlags = SDL_RENDERER_ACCELERATED;
    ctx->renderer = SDL_CreateRenderer(*win, -1, rendFlags);
    if (ctx->renderer == NULL) return -1;

    return 0;
}

void quitSDL(RenderContext* ctx, SDL_Window* win) {
    SDL_DestroyRenderer(ctx->renderer);
    SDL_DestroyWindow(win);

    SDL_Quit();
}
