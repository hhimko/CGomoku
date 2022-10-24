#include "./board.h"

#include <stdio.h>

#include "./sdl/texture.h"

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))


void renderTatamiBackground(SDL_Renderer* rend, double zoom, double offset_x, double offset_y) {
    zoom = MAX(zoom, 1); // zoom value must be greater than 1
    offset_x = MAX(MIN(offset_x, 1), -1); // |
    offset_y = MAX(MIN(offset_y, 1), -1); // | offset values are normalized and must be in [-1, 1] range

    SDL_Texture* tex = loadTextureBMP(rend, "../assets/tatami.bmp");

    int tex_w, tex_h;
    if (getTextureSize(tex, &tex_w, &tex_h) < 0) {
        tex_w = 600; // |
        tex_h = 600; // | quietly set an arbitrarily size manually
    }

    int win_w = 1920, win_h = 1080;

    // bg is scaled in order to fill the entire screen
    double scale_ratio = MAX(win_w * zoom / tex_w, win_h * zoom / tex_h);  
    int bg_size_w = (int)(tex_w * scale_ratio);
    int bg_size_h = (int)(tex_h * scale_ratio);
    tex_w = (int)(bg_size_w / 3.);
    tex_h = (int)(bg_size_h / 3.);

    int ox = (win_w - bg_size_w) / 2;
    int oy = (win_h - bg_size_h) / 2;
    ox += (int)(ox * offset_x);
    oy += (int)(oy * offset_y);


    SDL_Rect dstrect;
    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 3; ++x) {
            dstrect.x = tex_w * x + ox;
            dstrect.y = tex_h * y + oy; 
            dstrect.w = tex_w;
            dstrect.h = tex_h;

            SDL_RenderCopy(rend, tex, NULL, &dstrect);
        }
    }

    SDL_DestroyTexture(tex);
}

void renderBoard(SDL_Renderer* rend) {
    (void) rend;
}
