#ifndef UI_H_
#define UI_H_

#include <SDL.h>

#include "./sdl/render.h"
#include "./app.h"

int loadSeigaihaBackgroundTexture(RenderContext* ctx, SDL_Color* bg, SDL_Color* fg);
void randomizeSeigaihaBackgroundDirection();
void updateSeigaihaBackgroundParallax(RenderContext* ctx, int32_t mouse_x, int32_t mouse_y);
void renderSeigaihaBackground(RenderContext* ctx);

/* 
 * Render a tatami mat background on screen with given zoom level and offset values from the center. 
 * Zoom should be a float value bigger than 1. Smaller values are automatically mapped to 1. 
 */
void renderTatamiBackground(RenderContext* ctx, double zoom, double offset_x, double offset_y);

void renderSelectionCursor(SDL_Renderer* rend, SDL_Rect* rect);
void renderSelectionCursorF(SDL_Renderer* rend, FRect* frect);

/* Structure containing all data necessary for rendering and handling a button widget. */
typedef struct Button Button;
typedef void (*buttonCallback)(AppState* state);

struct Button {
    SDL_Rect rect;
    buttonCallback callback;
};

Button* createButton(SDL_Rect rect, buttonCallback callback);
void destroyButton(Button* btn);

void renderButton(SDL_Renderer* rend, Button* btn);

void updateUI(uint64_t dt);
void destroyUI();

#endif /* UI_H_ */
