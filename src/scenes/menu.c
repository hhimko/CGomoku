#include "./menu.h"

#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <SDL.h>

#include "../sdl/texture.h"
#include "../board.h"
#include "../app.h"
#include "../ui.h"

#define SEIGAIHA_RADIUS 80
#define BACKGROUND_ANIMATION_STEP 0.0003

#define MENU_BUTTON_PLAY   0
#define MENU_BUTTON_OPTS   1
#define MENU_BUTTON_EXIT   2
#define MENU_BUTTONS_COUNT 3

static Button* s_buttons[MENU_BUTTONS_COUNT] = { NULL, NULL, NULL };
static size_t s_selected_button = MENU_BUTTON_PLAY;

static int s_parallax_x = 0;
static int s_parallax_y = 0;

static double s_background_offset = 0.0;
static SDL_Texture* s_background_texture = NULL;


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

void menuUpdate(uint64_t dt) {
    s_background_offset = s_background_offset + BACKGROUND_ANIMATION_STEP*dt;
    if (s_background_offset >= 1.0) 
        s_background_offset -= (int)s_background_offset;
}

void menuRender(RenderContext* ctx) {
    renderMenuBackground(ctx);

    for (size_t i = 0; i < MENU_BUTTONS_COUNT; ++i) {
        renderButton(ctx->renderer, s_buttons[i]);
        if (i == s_selected_button)
            renderSelectionCursor(ctx->renderer, &s_buttons[i]->rect);
    }
}

SDL_bool menuHandleInput(SDL_Event* e, AppState* state) {
    switch (e->type) {
        case SDL_MOUSEMOTION:
            updateParallax(e->motion.x, e->motion.y, state->context->win_w, state->context->win_h);
            return SDL_TRUE;
    }

    return SDL_FALSE;
}

void buttonPlayCallback() {
    printf("btn play\n");
}

void buttonOptionsCallback() {
    printf("btn options\n");
}

void buttonExitCallback() {
    printf("btn exit\n");
}

void setMenuSceneCallbacks(AppState* state) {
    state->scene.update = menuUpdate;
    state->scene.render = menuRender;
    state->scene.handle_input = menuHandleInput;
    state->scene.destroy = menuDestroy;
}

int menuPrepare(AppState* state) {
    s_background_offset = 0.0;
    s_parallax_x = 0;
    s_parallax_y = 0;

    s_background_texture = generateSeigaihaTexture(
        state->context->renderer, SEIGAIHA_RADIUS, 3, 0.1, 
        &(SDL_Color){ .r=0x4B, .g=0x67, .b=0x9C }, 
        &(SDL_Color){ .r=0x2A, .g=0x4B, .b=0x74 }
    );

    s_buttons[MENU_BUTTON_PLAY] = createButton(&(SDL_Rect){ .x=600, .y=300, .w=500, .h=100 }, buttonPlayCallback);
    s_buttons[MENU_BUTTON_OPTS] = createButton(&(SDL_Rect){ .x=600, .y=500, .w=500, .h=100 }, buttonOptionsCallback);
    s_buttons[MENU_BUTTON_EXIT] = createButton(&(SDL_Rect){ .x=600, .y=700, .w=500, .h=100 }, buttonExitCallback);
    s_selected_button = MENU_BUTTON_PLAY;

    if (!s_buttons[MENU_BUTTON_PLAY] || !s_buttons[MENU_BUTTON_OPTS] || !s_buttons[MENU_BUTTON_EXIT]) 
        goto fail;

    setMenuSceneCallbacks(state);
    return 0;

fail:
    fprintf(stderr, "Failed to prepare main menu scene\n");
    return -1;
}

void menuDestroy() {
    SDL_DestroyTexture(s_background_texture);
}
