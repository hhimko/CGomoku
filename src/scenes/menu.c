#include "./menu.h"

#include <SDL.h>

#include "../board.h"


static double s_parallax_x = 0;
static double s_parallax_y = 0;


void updateParallax(int32_t mouse_x, int32_t mouse_y) {
    int32_t win_w = 1920, win_h = 1080; // TODO: read true window dimensions 

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

void menuUpdate(uint64_t dt) {
    (void) dt;
}

void menuRender(SDL_Renderer* rend) {
    renderTatamiBackground(rend, 1.2, s_parallax_x + 0.5, s_parallax_y);
}

void menuHandleInput(SDL_Event* e) {
    switch (e->type) {
        case SDL_MOUSEMOTION:
            int32_t mx = e->motion.x;
            int32_t my = e->motion.y;
            updateParallax(mx, my);
            break;
        
        default:
            break;
    }
}

void setMenuSceneCallbacks(AppState* state) {
    state->update = menuUpdate;
    state->render = menuRender;
    state->handle_input = menuHandleInput;
}
