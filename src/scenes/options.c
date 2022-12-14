#include "./options.h"

#include <stdio.h>
#include <math.h>
#include <SDL.h>

#include "../sdl/texture.h"
#include "../board.h"
#include "../app.h"
#include "../ui.h"


void optionsUpdate(uint64_t dt) {
    (void) dt;
}

void optionsRender(RenderContext* ctx) {
    renderSeigaihaBackground(ctx);
}

SDL_bool optionsHandleInput(SDL_Event* e, AppState* state) {
    switch (e->type) {
        case SDL_MOUSEMOTION:
            updateSeigaihaParallax(state->context, e->motion.x, e->motion.y);
            break;
    }

    return SDL_FALSE;
}

void setOptionsSceneCallbacks(AppState* state) {
    state->scene.update = optionsUpdate;
    state->scene.render = optionsRender;
    state->scene.handle_input = optionsHandleInput;
    state->scene.destroy = optionsDestroy;
}

int optionsPrepare(AppState* state) {
    SDL_Color bg = { .r=0x4B, .g=0x67, .b=0x9C };
    SDL_Color fg = { .r=0x2A, .g=0x4B, .b=0x74 };
    if (loadSeigaihaBackgroundTexture(state->context, &bg, &fg) < 0) goto fail;

    setOptionsSceneCallbacks(state);
    return 0;

fail:
    fprintf(stderr, "Failed to prepare options scene\n");
    optionsDestroy();
    return -1;
}

void optionsDestroy() {

}
