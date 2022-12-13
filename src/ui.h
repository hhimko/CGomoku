#ifndef UI_H_
#define UI_H_

#include <SDL.h>

#include "./sdl/render.h"
#include "./app.h"


void renderSelectionCursor(SDL_Renderer* rend, SDL_Rect* rect);

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

int initializeUI();
void updateUI(uint64_t dt);
void destroyUI();

#endif /* UI_H_ */
