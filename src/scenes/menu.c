#include "./menu.h"

#include <SDL.h>


void menuRender(SDL_Renderer* rend) {
    (void) rend;
}

void menuUpdate(int dt) {
    (void) dt;
}

void setMenuSceneCallbacks(AppState* state) {
    state->render = menuRender;
    state->update = menuUpdate;
}
