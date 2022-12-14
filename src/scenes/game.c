#include "./game.h"

#include <stdio.h>
#include <math.h>
#include <SDL.h>

#include "../sdl/texture.h"
#include "../board.h"
#include "../app.h"
#include "../ui.h"


void gameUpdate(uint64_t dt) {
    (void) dt;
}

void gameRender(RenderContext* ctx) {
    renderSeigaihaBackground(ctx);
}

SDL_bool gameHandleInput(SDL_Event* e, AppState* state) {
    switch (e->type) {
        case SDL_MOUSEMOTION:
            updateSeigaihaBackgroundParallax(state->context, e->motion.x, e->motion.y);
            break;

        case SDL_KEYDOWN:
            switch (e->key.keysym.sym) {
                default:
                    break;
            }
    }

    return SDL_FALSE;
}

void setGameSceneCallbacks(AppState* state) {
    state->scene.update = gameUpdate;
    state->scene.render = gameRender;
    state->scene.handle_input = gameHandleInput;
    state->scene.destroy = gameDestroy;
}

int gamePrepare(AppState* state) {
    SDL_Color bg = { .r=0x98, .g=0x2D, .b=0x49 };
    SDL_Color fg = { .r=0x81, .g=0x21, .b=0x36 };
    if (loadSeigaihaBackgroundTexture(state->context, &bg, &fg) < 0) goto fail;
    randomizeSeigaihaBackgroundDirection();

    setGameSceneCallbacks(state);
    return 0;

fail:
    fprintf(stderr, "Failed to prepare game scene\n");
    gameDestroy();
    return -1;
}

void gameDestroy() {

}
