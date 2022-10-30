#include "./menu.h"

#include <assert.h>
#include <SDL.h>

// #include "../ui/button.h"
#include "../board.h"
#include "../app.h"


static double s_parallax_x = 0;
static double s_parallax_y = 0;

// static Button s_button = {.rect = {.x = 100, .y = 100, .w = 200, .h = 50}};
static Board* s_board = NULL;


void updateParallax(int32_t mouse_x, int32_t mouse_y, int win_w, int win_h) {
    s_parallax_x = (mouse_x / (double)win_w) * 2 - 1; // |
    s_parallax_y = (mouse_y / (double)win_h) * 2 - 1; // | values normalized into [-1, 1] range

    // rescale the longer dimension to remap the parallax effect area to a square
    if (win_w > win_h) {
        double factor = 1. - (win_w - win_h) / (double)win_h;
        s_parallax_y *= factor;
    } else {
        double factor = 1. - (win_h - win_w) / (double)win_w;
        s_parallax_x *= factor;
    }
}

void menuPrepare() {
    s_parallax_x = 0;
    s_parallax_y = 0;

    s_board = createBoard();

    assert(s_board != NULL);
}

void menuUpdate(uint64_t dt) {
    (void) dt;
}

void menuRender(RenderContext* ctx) {
    renderTatamiBackground(ctx, 1.2, s_parallax_x + 0.5, s_parallax_y);
}

SDL_bool menuHandleInput(SDL_Event* e, AppState* state) {
    switch (e->type) {
        case SDL_MOUSEMOTION:
            int32_t mx = e->motion.x;
            int32_t my = e->motion.y;
            updateParallax(mx, my, state->context->win_w, state->context->win_h);
            return SDL_TRUE;
    }

    return SDL_FALSE;
}

void setMenuSceneCallbacks(AppState* state) {
    state->scene.update = menuUpdate;
    state->scene.render = menuRender;
    state->scene.handle_input = menuHandleInput;
}

void menuDestroy() {
    destroyBoard(s_board);
}
