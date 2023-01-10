#include "./menu.h"

#include <stdio.h>
#include <math.h>
#include <SDL.h>

#include "../app.h"
#include "../ui.h"

#define MENU_BUTTON_PLAY   0
#define MENU_BUTTON_OPTS   1
#define MENU_BUTTON_EXIT   2
#define MENU_BUTTONS_COUNT 3

#define BUTTON_WIDTH  700
#define BUTTON_HEIGHT 120
#define BUTTON_GAP    80


static Button* s_buttons[MENU_BUTTONS_COUNT] = { NULL, NULL, NULL };
static size_t s_selected_button = MENU_BUTTON_PLAY;


void buttonPlayCallback(AppState* state) {
    setScene(state, SCENE_GAME);
}

void buttonOptionsCallback(AppState* state) {
    setScene(state, SCENE_OPTIONS);
}

void buttonExitCallback(AppState* state) {
    (void) state;
    Gomoku_shutdown();
}

void menuUpdate(uint64_t dt) {
    (void) dt;
}

void menuRender(RenderContext* ctx) {
    renderSeigaihaBackground(ctx);

    for (size_t i = 0; i < MENU_BUTTONS_COUNT; ++i) {
        renderButton(ctx->renderer, s_buttons[i]);
        if (i == s_selected_button)
            renderSelectionCursor(ctx->renderer, &s_buttons[i]->rect);
    }
}

SDL_bool menuHandleInput(SDL_Event* e, AppState* state) {
    switch (e->type) {
        case SDL_MOUSEMOTION:
            updateSeigaihaBackgroundParallax(state->context, e->motion.x, e->motion.y);
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

void setMenuSceneCallbacks(AppState* state) {
    state->scene.update = menuUpdate;
    state->scene.render = menuRender;
    state->scene.handle_input = menuHandleInput;
    state->scene.destroy = menuDestroy;
}

int menuPrepare(AppState* state) {
    SDL_Color bg = { .r=0x4B, .g=0x67, .b=0x9C };
    SDL_Color fg = { .r=0x2A, .g=0x4B, .b=0x74 };
    if (loadSeigaihaBackgroundTexture(state->context, &bg, &fg) < 0) goto fail;
    randomizeSeigaihaBackgroundDirection();
    pushSeigaihaAnimation();

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
    destroyButton(s_buttons[MENU_BUTTON_PLAY]);
    destroyButton(s_buttons[MENU_BUTTON_OPTS]);
    destroyButton(s_buttons[MENU_BUTTON_EXIT]);
}
