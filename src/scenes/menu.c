#include "./menu.h"

#include <assert.h>
#include <math.h>
#include <SDL.h>

#include "../sdl/texture.h"
#include "../board.h"
#include "../app.h"
#include "../ui.h"

#define SEIGAIHA_RADIUS 80
#define BACKGROUND_ANIMATION_STEP 0.0003


static int s_parallax_x = 0;
static int s_parallax_y = 0;

static double s_background_offset = 0.0;
static SDL_Texture* s_background_texture = NULL;

// static Button s_button = {.rect = {.x = 100, .y = 100, .w = 200, .h = 50}};


void updateParallax(int32_t mouse_x, int32_t mouse_y, int win_w, int win_h) {
    double xnorm = mouse_x / (double)win_w;
    double ynorm = mouse_y / (double)win_h;

    s_parallax_x = (int)round(2*SEIGAIHA_RADIUS*xnorm);
    s_parallax_y = (int)round(SEIGAIHA_RADIUS*ynorm);
}

void renderMenuBackground(RenderContext* ctx) {
    int animation_offset = 2*SEIGAIHA_RADIUS - (int)round(2*SEIGAIHA_RADIUS*s_background_offset);

    SDL_Rect rect = { 
        .x = -animation_offset - s_parallax_x,
        .y = -animation_offset - s_parallax_y, 
        .w = ctx->win_w + animation_offset + s_parallax_x, 
        .h = ctx->win_h + animation_offset + s_parallax_y
    };

    renderTextureRepeat(ctx->renderer, s_background_texture, &rect);
}

void menuPrepare(AppState* state) {
    s_parallax_x = 0;
    s_parallax_y = 0;

    s_background_offset = 0.0;

    SDL_Color bg = { .r=0x4B, .g=0x67, .b=0x9C };
    SDL_Color fg = { .r=0x2A, .g=0x4B, .b=0x74 };
    s_background_texture = generateSeigaihaTexture(state->context->renderer, SEIGAIHA_RADIUS, 3, 0.1, &bg, &fg);

    assert(s_background_texture != NULL);
}

void menuUpdate(uint64_t dt) {
    s_background_offset = s_background_offset + BACKGROUND_ANIMATION_STEP*dt;
    if (s_background_offset >= 1.0) 
        s_background_offset -= (int)s_background_offset;
}

void menuRender(RenderContext* ctx) {
    renderMenuBackground(ctx);

    SDL_Rect rect = {.x = 100, .y = 100, .w = 200, .h = 50};

    renderSelectionCursor(ctx->renderer, &rect);
}

SDL_bool menuHandleInput(SDL_Event* e, AppState* state) {
    switch (e->type) {
        case SDL_MOUSEMOTION:
            updateParallax(e->motion.x, e->motion.y, state->context->win_w, state->context->win_h);
            return SDL_TRUE;
    }

    return SDL_FALSE;
}

void menuDestroy() {
    SDL_DestroyTexture(s_background_texture);
}

void setMenuSceneCallbacks(AppState* state) {
    state->scene.update = menuUpdate;
    state->scene.render = menuRender;
    state->scene.handle_input = menuHandleInput;
    state->scene.destroy = menuDestroy;
}
