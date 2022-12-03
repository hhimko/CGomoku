#ifndef RENDER_H_
#define RENDER_H_

#include <SDL.h>


/* Structure containing necessary data for general rendering. */
typedef struct RenderContext RenderContext;

struct RenderContext {
    SDL_Renderer* renderer;
    int win_w;
    int win_h;
};

RenderContext* createRenderContext(int win_w, int win_h);
void destroyRenderContext(RenderContext* ctx);

/*  
 *  Used for specifying the renderig method bor borders 
 *  Can be set to either BORDER_TYPE_INNER, BORDER_TYPE_CENTER or BORDER_TYPE_OUTER
 */
typedef enum {
    BORDER_TYPE_INNER = 0,  // \skip border rendering relies on the enum integer values
    BORDER_TYPE_CENTER = 1,
    BORDER_TYPE_OUTER = 2
} BorderType;

void drawFRectBorder(SDL_Renderer* rend, SDL_FRect* frect, float thickness, BorderType border_type);

void drawCircleAA(SDL_Renderer* rend, int x, int y, double rad);
void drawFilledCircleAA(SDL_Renderer* rend, int x, int y, double rad);
void drawCircleBorderAA(SDL_Renderer* rend, int x, int y, double rad, double thickness, BorderType border_type);

int initSDL(RenderContext* ctx, SDL_Window** win);
void quitSDL(RenderContext* ctx, SDL_Window* win);

#endif /* RENDER_H_ */
