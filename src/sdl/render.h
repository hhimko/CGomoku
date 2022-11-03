#ifndef RENDER_H_
#define RENDER_H_

#include <SDL.h>


void drawCircleAA(SDL_Renderer* rend, int x, int y, double rad);

/* Structure containing necessary data for general rendering. */
typedef struct RenderContext RenderContext;

struct RenderContext {
    SDL_Renderer* renderer;
    int win_w;
    int win_h;
};

RenderContext* createRenderContext(int win_w, int win_h);
void destroyRenderContext(RenderContext* ctx);

int initSDL(RenderContext* ctx, SDL_Window** win);
void quitSDL(RenderContext* ctx, SDL_Window* win);

#endif /* RENDER_H_ */
