// Copyright (c) Dawid "hhimko" Cyganek <dawid.cyganek1@gmail.com>. Licensed under the MIT Licence.
// See the LICENCE file in the repository root for full licence text.

#include "./game.h"

#include <stdio.h>
#include <math.h>
#include <SDL.h>

#include "../sdl/texture.h"
#include "../gomoku.h"
#include "../board.h"
#include "../app.h"
#include "../ui.h"

static GomokuGame* s_game = NULL;

#define MIN(x, y) ((x) < (y) ? (x) : (y))


void gameUpdate(uint64_t dt) {
    s_game->update(s_game, dt);
}

void gameRender(RenderContext* ctx) {
    renderSeigaihaBackground(ctx);
    renderBoard(ctx, s_game->board);
}

SDL_bool gameHandleInput(SDL_Event* e, AppState* state) {
    switch (e->type) {
        case SDL_MOUSEMOTION:
            updateSeigaihaBackgroundParallax(state->context, e->motion.x, e->motion.y);
            break;

        case SDL_KEYDOWN:
            switch (e->key.keysym.sym) {
                case SDLK_ESCAPE:
                    setScene(state, SCENE_MENU);
                    return SDL_TRUE;
            }
    }

    if (s_game->handleInput(s_game, e)) return SDL_TRUE;
    return SDL_FALSE;
}

void setGameSceneCallbacks(AppState* state) {
    state->scene.update = gameUpdate;
    state->scene.render = gameRender;
    state->scene.handle_input = gameHandleInput;
    state->scene.destroy = gameDestroy;
}

int gamePrepare(AppState* state) {
    SDL_Color bg = { .r=0xD5, .g=0x24, .b=0x3D }; 
    SDL_Color fg = { .r=0x9E, .g=0x1A, .b=0x26 };
    if (loadSeigaihaBackgroundTexture(state->context, &bg, &fg) < 0) goto fail;
    randomizeSeigaihaBackgroundDirection();
    pushSeigaihaAnimation();

    uint32_t size = MIN(state->context->win_w, state->context->win_h) - 300;
    Board* board = createBoard(state->context, state->context->win_w/2 - size/2, state->context->win_h/2 - size/2, size);
    if (board == NULL) goto fail; // board and textures get freed in destroyGomokuGame() 

    s_game = createGomokuGame(board);
    if (s_game == NULL) goto fail;

    setGameSceneCallbacks(state);
    return 0;

fail:
    fprintf(stderr, "Failed to prepare game scene\n");
    gameDestroy();
    return -1;
}

void gameDestroy() {
    destroyGomokuGame(s_game);
}
