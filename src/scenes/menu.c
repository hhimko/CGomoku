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

#define BUTTON_WIDTH  700
#define BUTTON_HEIGHT 120
#define BUTTON_GAP    80

static Button* s_buttons[MENU_BUTTONS_COUNT] = { NULL, NULL, NULL };
static size_t s_selected_button = MENU_BUTTON_PLAY;

static int s_parallax_x = 0;
static int s_parallax_y = 0;

static double s_background_offset = 0.0;
static SDL_Texture* s_background_texture = NULL;


void updateBackgroundParallax(int32_t mouse_x, int32_t mouse_y, int win_w, int win_h) {
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
            updateBackgroundParallax(e->motion.x, e->motion.y, state->context->win_w, state->context->win_h);
            break;

        case SDL_KEYDOWN:
            switch (e->key.keysym.sym) {
                case SDLK_UP:
                    // decrement selected button index on key up
                    s_selected_button = (--s_selected_button > MENU_BUTTONS_COUNT) ? MENU_BUTTONS_COUNT - 1 : s_selected_button;
                    return SDL_TRUE;

                case SDLK_DOWN:
                    // increment selected button index on key down
                    s_selected_button = (++s_selected_button) % MENU_BUTTONS_COUNT;
                    return SDL_TRUE;

                case SDLK_RETURN:
                    // fire selected button callback on key enter
                    s_buttons[s_selected_button]->callback(state);
                    return SDL_TRUE;
            }
    }

    return SDL_FALSE;
}

void buttonPlayCallback(AppState* state) {
    (void) state;
    printf("btn play\n");
}

void buttonOptionsCallback(AppState* state) {
    (void) state;
    printf("btn options\n");
}

void buttonExitCallback(AppState* state) {
    (void) state;
    Gomoku_shutdown();
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

    int w = state->context->win_w;
    int h = state->context->win_h;
    SDL_Rect btn_rect = { 
        .x = w/2 - BUTTON_WIDTH/2, 
        .y = h/2 - BUTTON_HEIGHT/2 - BUTTON_HEIGHT - BUTTON_GAP, 
        .w = BUTTON_WIDTH, 
        .h = BUTTON_HEIGHT 
    };

    // button play
    s_buttons[MENU_BUTTON_PLAY] = createButton(btn_rect, buttonPlayCallback);

    // button options
    btn_rect.y += BUTTON_HEIGHT + BUTTON_GAP;
    s_buttons[MENU_BUTTON_OPTS] = createButton(btn_rect, buttonOptionsCallback);

    // button exit
    btn_rect.y += BUTTON_HEIGHT + BUTTON_GAP;
    s_buttons[MENU_BUTTON_EXIT] = createButton(btn_rect, buttonExitCallback);

    s_selected_button = MENU_BUTTON_PLAY;

    if (!s_buttons[MENU_BUTTON_PLAY] || !s_buttons[MENU_BUTTON_OPTS] || !s_buttons[MENU_BUTTON_EXIT]) 
        goto fail;

    setMenuSceneCallbacks(state);
    return 0;

fail:
    fprintf(stderr, "Failed to prepare main menu scene\n");
    menuDestroy();
    return -1;
}

void menuDestroy() {
    SDL_DestroyTexture(s_background_texture);
    destroyButton(s_buttons[MENU_BUTTON_PLAY]);
    destroyButton(s_buttons[MENU_BUTTON_OPTS]);
    destroyButton(s_buttons[MENU_BUTTON_EXIT]);
}
