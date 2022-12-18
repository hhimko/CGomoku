#include "./game.h"

#include <stdio.h>
#include <math.h>
#include <SDL.h>

#include "../sdl/texture.h"
#include "../board.h"
#include "../app.h"
#include "../ui.h"

static Board* s_board = NULL;

#define MIN(x, y) ((x) < (y) ? (x) : (y))


void gameUpdate(uint64_t dt) {
    (void) dt;
}

void gameRender(RenderContext* ctx) {
    renderSeigaihaBackground(ctx);
    renderBoard(ctx, s_board);
}

SDL_bool gameHandleInput(SDL_Event* e, AppState* state) {
    switch (e->type) {
        case SDL_MOUSEMOTION:
            updateSeigaihaBackgroundParallax(state->context, e->motion.x, e->motion.y);
            if (boardHandleMouseMotion(s_board, e->motion.x, e->motion.y)) return SDL_TRUE;
            break;

        case SDL_KEYDOWN:
            if (boardHandleKeyDown(s_board, e->key.keysym.sym)) return SDL_TRUE;
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

    uint32_t size = MIN(state->context->win_w, state->context->win_h) - 300;
    s_board = createBoard(state->context->win_w/2 - size/2, state->context->win_h/2 - size/2, size);
    if (s_board == NULL) goto fail;

    setGameSceneCallbacks(state);
    return 0;

fail:
    fprintf(stderr, "Failed to prepare game scene\n");
    gameDestroy();
    return -1;
}

void gameDestroy() {
    destroyBoard(s_board);
}
