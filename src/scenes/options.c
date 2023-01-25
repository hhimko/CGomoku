#include "./options.h"

#include <stdio.h>
#include <math.h>
#include <SDL.h>

#include "../app.h"
#include "../ui.h"

#define BUTTON_WIDTH  600
#define BUTTON_HEIGHT 160

static Button* s_return_button = NULL;


void buttonReturnCallback(AppState* state) {
    setScene(state, SCENE_MENU);
}

void optionsUpdate(uint64_t dt) {
    (void) dt;
}

void optionsRender(RenderContext* ctx) {
    renderSeigaihaBackground(ctx);

    renderButton(ctx->renderer, s_return_button);
    renderSelectionCursor(ctx->renderer, &s_return_button->rect);
}

SDL_bool optionsHandleMouseDown(AppState* state, int32_t mx, int32_t my, uint8_t button) {
    if (button != SDL_BUTTON_LEFT) return SDL_FALSE;

    updateSeigaihaBackgroundParallax(state->context, mx, my);

    SDL_Point mp = { .x = mx, .y = my };
    if (SDL_PointInRect(&mp, &s_return_button->rect)) {
        buttonPress(s_return_button, state);
        return SDL_TRUE;
    }

    return SDL_FALSE;
}

SDL_bool optionsHandleInput(SDL_Event* e, AppState* state) {
    switch (e->type) {
        case SDL_MOUSEMOTION:
            updateSeigaihaBackgroundParallax(state->context, e->motion.x, e->motion.y);
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (optionsHandleMouseDown(state, e->button.x, e->button.y, e->button.button)) return SDL_TRUE;
            break;

        case SDL_KEYDOWN:
            switch (e->key.keysym.sym) {
                case SDLK_ESCAPE:
                case SDLK_RETURN:
                    // fire selected button callback on key enter | escape
                    buttonPress(s_return_button, state);
                    return SDL_TRUE;
            }
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
    SDL_Color bg = { .r=0x2C, .g=0xAD, .b=0x87 };
    SDL_Color fg = { .r=0x31, .g=0x9E, .b=0x76 };
    if (loadSeigaihaBackgroundTexture(state->context, &bg, &fg) < 0) goto fail;
    randomizeSeigaihaBackgroundDirection();
    pushSeigaihaAnimation();

    int w = state->context->win_w;
    int h = state->context->win_h;
    SDL_Rect btn_rect = { 
        .x = w/2 - BUTTON_WIDTH/2, 
        .y = h - BUTTON_HEIGHT - 100,
        .w = BUTTON_WIDTH, 
        .h = BUTTON_HEIGHT 
    };

    s_return_button = createButton(state->context, btn_rect, "RETURN", buttonReturnCallback);
    if (s_return_button == NULL) goto fail;

    setOptionsSceneCallbacks(state);
    return 0;

fail:
    fprintf(stderr, "Failed to prepare options scene\n");
    optionsDestroy();
    return -1;
}

void optionsDestroy() {
    destroyButton(s_return_button);
}
