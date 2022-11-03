#include "./sdl/render.h"

#include <math.h>
#include <assert.h>


void drawCircleAA(SDL_Renderer* rend, int x, int y, double rad) {
    /* 
     * Slightly modified algorithm based on Xiaolin Wu's method. 
     * https://yellowsplash.wordpress.com/2009/10/23/fast-antialiased-circles-and-ellipses-from-xiaolin-wus-concepts/
     */
    static const double SQRT2 = 1.41421356237;
    
    double radsq = rad*rad; 
    int ffd = (int)round(rad / SQRT2); // 45 degree pixel coordinate

    uint8_t r,g,b,alpha;
    SDL_GetRenderDrawColor(rend, &r, &g, &b, NULL);

    for (int xi = 0; xi <= ffd; ++xi) {
        double yi = sqrt(radsq - xi*xi); // y coordinate on the circle for the iterated x

        int ipart = (int)yi; // integer part of the calculated real
        double fpart = yi - ipart; // fractional part of the calculated real

        // draw the first point in 8-symmetry
        alpha = (uint8_t)((1.0 - fpart) * 255.0);
        SDL_SetRenderDrawColor(rend, r, g, b, alpha);

        SDL_RenderDrawPoint(rend, x + xi, y + ipart);
        SDL_RenderDrawPoint(rend, x - xi, y + ipart);
        SDL_RenderDrawPoint(rend, x - xi, y - ipart);
        SDL_RenderDrawPoint(rend, x + xi, y - ipart);

        SDL_RenderDrawPoint(rend, x + ipart, y + xi);
        SDL_RenderDrawPoint(rend, x - ipart, y + xi);
        SDL_RenderDrawPoint(rend, x - ipart, y - xi);
        SDL_RenderDrawPoint(rend, x + ipart, y - xi);

        // draw the offset point in 8-symmetry
        ipart++; 
        alpha = (uint8_t)(fpart * 255.0);
        SDL_SetRenderDrawColor(rend, r, g, b, alpha);

        SDL_RenderDrawPoint(rend, x + xi, y + ipart);
        SDL_RenderDrawPoint(rend, x - xi, y + ipart);
        SDL_RenderDrawPoint(rend, x - xi, y - ipart);
        SDL_RenderDrawPoint(rend, x + xi, y - ipart);

        SDL_RenderDrawPoint(rend, x + ipart, y + xi);
        SDL_RenderDrawPoint(rend, x - ipart, y + xi);
        SDL_RenderDrawPoint(rend, x - ipart, y - xi);
        SDL_RenderDrawPoint(rend, x + ipart, y - xi);
    }
}

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

    if (SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_BLEND) < 0) return -1;

    return 0;
}

void quitSDL(RenderContext* ctx, SDL_Window* win) {
    SDL_DestroyRenderer(ctx->renderer);
    SDL_DestroyWindow(win);

    SDL_Quit();
}
