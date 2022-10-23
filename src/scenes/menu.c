#include "./menu.h"

#include <SDL.h>

#include "../board.h"


void menuUpdate(uint64_t dt) {
    (void) dt;
}

void menuRender(SDL_Renderer* rend) {
    renderBoard(rend);
}

void menuHandleInput(SDL_Event* e) {
    (void) e;
}

void setMenuSceneCallbacks(AppState* state) {
    state->update = menuUpdate;
    state->render = menuRender;
    state->handle_input = menuHandleInput;
}
