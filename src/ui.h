// Copyright (c) Dawid "hhimko" Cyganek <dawid.cyganek1@gmail.com>. Licensed under the MIT Licence.
// See the LICENCE file in the repository root for full licence text.

#ifndef UI_H_
#define UI_H_

#include <SDL.h>

#include "./sdl/render.h"
#include "./animation.h"
#include "./app.h"


int loadSeigaihaBackgroundTexture(RenderContext* ctx, SDL_Color* bg, SDL_Color* fg);
void randomizeSeigaihaBackgroundDirection();
void pushSeigaihaAnimation();
void updateSeigaihaBackgroundParallax(RenderContext* ctx, int32_t mouse_x, int32_t mouse_y);
void renderSeigaihaBackground(RenderContext* ctx);

/* 
 * Render a tatami mat background on screen with given zoom level and offset values from the center. 
 * Zoom should be a float value bigger than 1. Smaller values are automatically mapped to 1. 
 */
void renderTatamiBackground(RenderContext* ctx, double zoom, double offset_x, double offset_y);

void pushCursorAnimation();
void renderSelectionCursor(SDL_Renderer* rend, SDL_Rect* rect);
void renderSelectionCursorF(SDL_Renderer* rend, FRect* frect);

typedef struct Label Label;

struct Label {
    int width, height; // intended for read only
    SDL_Texture* tex;
};

Label* createLabel(SDL_Renderer* rend, const char* text, int font_size);
void destroyLabel(Label* label);

/* Structure containing all data necessary for rendering and handling a button widget. */
typedef struct Button Button;
typedef void (*buttonCallback)(AppState* state);

struct Button {
    SDL_Rect rect;
    Label* label;
    buttonCallback callback;
    SDL_Texture* tex;
    SDL_Texture* shadow_tex;
    Animation* select_animation;
};

Button* createButton(RenderContext* ctx, SDL_Rect rect, const char* text, buttonCallback callback);

void renderButton(SDL_Renderer* rend, Button* btn);
void buttonPress(Button* btn, AppState* state);
void buttonSelect(Button* btn);
void buttonDeselect(Button* btn);

void destroyButton(Button* btn);

int initializeUI();
void destroyUI();

#endif /* UI_H_ */
