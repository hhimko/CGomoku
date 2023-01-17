#include "./ui.h"

#include <SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "./animation.h"
#include "./sdl/render.h"
#include "./sdl/texture.h"

#define SEIGAIHA_RADIUS 80
#define CURSOR_ANIMATION_STRENGTH 15.0f

#define BUTTON_SHADOW_RADIUS 24
#define BUTTON_SHADOW_STRENGTH 180
#define BUTTON_SHADOW_OFFSET_X -12
#define BUTTON_SHADOW_OFFSET_Y 12

static int s_seigaiha_parallax_x = 0;
static int s_seigaiha_parallax_y = 0;
static int s_seigaiha_direction_x = 1;
static int s_seigaiha_direction_y = 1;

static double s_background_offset = 0.0;
static SDL_Texture* s_background_texture = NULL;

static double s_cursor_animation_t = 0.0;

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))


int loadSeigaihaBackgroundTexture(RenderContext* ctx, SDL_Color* bg, SDL_Color* fg) {
    SDL_DestroyTexture(s_background_texture);
    s_background_texture = generateSeigaihaTexture(ctx->renderer, SEIGAIHA_RADIUS, 3, 0.1, bg, fg);
    if (s_background_texture == NULL)
        return -1;

    return 0;    
}

void updateSeigaihaBackgroundParallax(RenderContext* ctx, int32_t mouse_x, int32_t mouse_y) {
    double xnorm = mouse_x / (double)ctx->win_w;
    double ynorm = mouse_y / (double)ctx->win_h;

    s_seigaiha_parallax_x = (int)round(2*SEIGAIHA_RADIUS*xnorm);
    s_seigaiha_parallax_y = (int)round(SEIGAIHA_RADIUS*ynorm);
}

void randomizeSeigaihaBackgroundDirection() {
    // current method makes sure at least one axis gets flipped
    if (rand() % 2) {
        s_seigaiha_direction_x *= -1;
        if (rand() % 2) s_seigaiha_direction_y *= -1;
    } else {
        s_seigaiha_direction_y *= -1;
        if (rand() % 2) s_seigaiha_direction_x *= -1;
    }
}

void pushSeigaihaAnimation() {
    pushAnimation(&s_background_offset, 4000, ANIMATION_TYPE_LINEAR, 1);
}

void renderSeigaihaBackground(RenderContext* ctx) {
    int animation_offset = 2*SEIGAIHA_RADIUS - (int)round(2*SEIGAIHA_RADIUS*s_background_offset);

    SDL_Rect rect = { 
        .x = -s_seigaiha_parallax_x - SEIGAIHA_RADIUS*2 - animation_offset*s_seigaiha_direction_x,
        .y = -s_seigaiha_parallax_y - SEIGAIHA_RADIUS*2 - animation_offset*s_seigaiha_direction_y, 
        .w = ctx->win_w + animation_offset + s_seigaiha_parallax_x + SEIGAIHA_RADIUS*2, 
        .h = ctx->win_h + animation_offset + s_seigaiha_parallax_y + SEIGAIHA_RADIUS*2
    };

    renderTextureRepeat(ctx->renderer, s_background_texture, &rect);
}

void renderTatamiBackground(RenderContext* ctx, double zoom, double offset_x, double offset_y) {
    zoom = MAX(zoom, 1); // zoom value must be greater than 1
    offset_x = MAX(MIN(offset_x, 1), -1); // |
    offset_y = MAX(MIN(offset_y, 1), -1); // | offset values must be normalized to [-1, 1] range

    SDL_Renderer* rend = ctx->renderer;
    int win_w = ctx->win_w; 
    int win_h = ctx->win_h;

    SDL_Texture* tatami = loadTextureBMP(rend, "../assets/tatami.bmp");
    SDL_Texture* tatami_half = loadTextureBMP(rend, "../assets/tatami_half.bmp");

    int tex_w, tex_h;
    // logic assumes both tatami textures are the same size 
    if (getTextureSize(tatami, &tex_w, &tex_h) < 0) {
        tex_w = 800; // |
        tex_h = 800; // | quietly set an arbitrarily size manually
    }

    // bg is scaled in order to fill the entire screen
    double scale_ratio = MAX(win_w * zoom / tex_w, win_h * zoom / tex_h);  
    int bg_size_w = (int)(tex_w * scale_ratio);
    int bg_size_h = (int)(tex_h * scale_ratio);
    tex_w = (int)(bg_size_w / 3.);
    tex_h = (int)(bg_size_h / 3.);

    // calculate the final offsets with centering
    int ox = (win_w - bg_size_w) / 2;
    int oy = (win_h - bg_size_h) / 2;
    ox += (int)(ox * offset_x);
    oy += (int)(oy * offset_y);

    SDL_Rect dstrect = { .w = tex_w, .h = tex_h };
    /*  drawn pattern reference:
     *      +------+------+------+
     *      | half   half | half |
     *      |  TL     TC  |  TR  |
     *      +------+------+      +
     *      | half | full | half |
     *      |  CL  |  CC  |  CR  |
     *      +      +------+------+
     *      | half | half   half |
     *      |  BL  |  BC     BR  |
     *      +------+------+------+
     */  

    // top-left
    dstrect.x = ox;
    dstrect.y = oy; 
    SDL_RenderCopy(rend, tatami_half, NULL, &dstrect);

    // top-center
    dstrect.x += tex_w;
    SDL_RenderCopyEx(rend, tatami_half, NULL, &dstrect, 0., NULL, SDL_FLIP_HORIZONTAL);

    // top-right
    dstrect.x += tex_w;
    SDL_RenderCopyEx(rend, tatami_half, NULL, &dstrect, 90., NULL, SDL_FLIP_NONE);

    // center-left
    dstrect.y += tex_h;
    dstrect.x = ox;
    SDL_RenderCopyEx(rend, tatami_half, NULL, &dstrect, 90., NULL, SDL_FLIP_NONE);

    // center-center
    dstrect.x += tex_w;
    SDL_RenderCopy(rend, tatami, NULL, &dstrect);

    // center-right
    dstrect.x += tex_w;
    SDL_RenderCopyEx(rend, tatami_half, NULL, &dstrect, -90., NULL, SDL_FLIP_NONE);

    // bottom-left
    dstrect.y += tex_h;
    dstrect.x = ox;
    SDL_RenderCopyEx(rend, tatami_half, NULL, &dstrect, -90., NULL, SDL_FLIP_NONE);

    // bottom-center
    dstrect.x += tex_w;
    SDL_RenderCopy(rend, tatami_half, NULL, &dstrect); 

    // bottom-right
    dstrect.x += tex_w;
    SDL_RenderCopyEx(rend, tatami_half, NULL, &dstrect, 0., NULL, SDL_FLIP_HORIZONTAL);

    SDL_DestroyTexture(tatami);
    SDL_DestroyTexture(tatami_half);
}

void pushCursorAnimation() {
    pushAnimation(&s_cursor_animation_t, 2000, ANIMATION_TYPE_SMOOTHSTEP_INOUT, 1);
}

void renderSelectionCursor(SDL_Renderer* rend, SDL_Rect* rect) {
    static const float thickness = 10.0f; 
    static const float length = 30; 
    float offset = 10.0f + CURSOR_ANIMATION_STRENGTH * (float)s_cursor_animation_t; 

    uint8_t r,g,b,a;
    SDL_GetRenderDrawColor(rend, &r, &g, &b, &a);

    SDL_SetRenderDrawColor(rend, 0xFF, 0x00, 0x00, SDL_ALPHA_OPAQUE);

    FRect drawrect = {
        .x = rect->x - offset - thickness,
        .y = rect->y - offset - thickness,
        .w = thickness,
        .h = length
    };

    // top-left
    drawFilledFRect(rend, &drawrect);
    drawrect.w = length;
    drawrect.h = thickness;
    drawFilledFRect(rend, &drawrect);

    // top-right
    drawrect.x = rect->x + rect->w + offset + thickness;
    drawrect.w = -length;
    drawFilledFRect(rend, &drawrect);
    drawrect.w = -thickness;
    drawrect.h = length;
    drawFilledFRect(rend, &drawrect);    

    // bottom-right
    drawrect.y = rect->y + rect->h + offset + thickness;
    drawrect.h = -length;
    drawFilledFRect(rend, &drawrect);
    drawrect.w = -length;
    drawrect.h = -thickness;
    drawFilledFRect(rend, &drawrect);
    
    // bottom-left
    drawrect.x = rect->x - offset - thickness,
    drawrect.w = length;
    drawFilledFRect(rend, &drawrect);
    drawrect.w = thickness;
    drawrect.h = -length;
    drawFilledFRect(rend, &drawrect);

    SDL_SetRenderDrawColor(rend, r, g, b, a); // restore original color
}

void renderSelectionCursorF(SDL_Renderer* rend, FRect* frect) {
    static const float thickness = 10.0f; 
    static const float length = 30; 
    float offset = 10.0f + CURSOR_ANIMATION_STRENGTH * (float)s_cursor_animation_t; 

    uint8_t r,g,b,a;
    SDL_GetRenderDrawColor(rend, &r, &g, &b, &a);

    SDL_SetRenderDrawColor(rend, 0xFF, 0x00, 0x00, SDL_ALPHA_OPAQUE);

    FRect drawrect = {
        .x = frect->x - offset - thickness,
        .y = frect->y - offset - thickness,
        .w = thickness,
        .h = length
    };

    // top-left
    drawFilledFRect(rend, &drawrect);
    drawrect.w = length;
    drawrect.h = thickness;
    drawFilledFRect(rend, &drawrect);

    // top-right
    drawrect.x = frect->x + frect->w + offset + thickness;
    drawrect.w = -length;
    drawFilledFRect(rend, &drawrect);
    drawrect.w = -thickness;
    drawrect.h = length;
    drawFilledFRect(rend, &drawrect);    

    // bottom-right
    drawrect.y = frect->y + frect->h + offset + thickness;
    drawrect.h = -length;
    drawFilledFRect(rend, &drawrect);
    drawrect.w = -length;
    drawrect.h = -thickness;
    drawFilledFRect(rend, &drawrect);
    
    // bottom-left
    drawrect.x = frect->x - offset - thickness,
    drawrect.w = length;
    drawFilledFRect(rend, &drawrect);
    drawrect.w = thickness;
    drawrect.h = -length;
    drawFilledFRect(rend, &drawrect);

    SDL_SetRenderDrawColor(rend, r, g, b, a); // restore original color
}

SDL_Texture* createButtonTexture(SDL_Renderer* rend, SDL_Rect* rect) {
    SDL_Texture* tex = SDL_CreateTexture(rend, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, rect->w, rect->h);

    uint8_t r,g,b,a;
    SDL_GetRenderDrawColor(rend, &r, &g, &b, &a);

    if (tex == NULL || SDL_SetRenderTarget(rend, tex) < 0) goto button_tex_fail;
    if (SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND) < 0) goto button_tex_fail;

    // fill texture with transparent pixels
    SDL_SetRenderDrawColor(rend, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(rend);

    // render button center
    static const int o = 14;
    SDL_Rect drawrect = { .x = o, .y = o, .w = rect->w - 2*o, .h = rect->h - 2*o };

    SDL_SetRenderDrawColor(rend, 0xE7, 0xE6, 0xDE, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(rend, &drawrect);
    
    // render button frame 
    SDL_SetRenderDrawColor(rend, 0x52, 0x47, 0x3C, SDL_ALPHA_OPAQUE);
    
    drawrect.x = 0;
    drawrect.y = o;
    drawrect.w = rect->w;
    drawrect.h = o;
    SDL_RenderFillRect(rend, &drawrect);

    drawrect.y = rect->h - 2*o;
    SDL_RenderFillRect(rend, &drawrect);

    drawrect.x = o;
    drawrect.y = 0;
    drawrect.w = o;
    drawrect.h = rect->h;
    SDL_RenderFillRect(rend, &drawrect);

    drawrect.x = rect->w - 2*o;
    SDL_RenderFillRect(rend, &drawrect);
    
    SDL_SetRenderDrawColor(rend, r, g, b, a); // restore original color

    // detach texture from renderer 
    if (SDL_SetRenderTarget(rend, NULL) >= 0) {
        return tex;
    }

button_tex_fail:
    fprintf(stderr, "Failed to create the button texture. \nSDL_Error: %s\n", SDL_GetError());
    SDL_SetRenderTarget(rend, NULL);
    SDL_DestroyTexture(tex);
    return NULL;
}

Button* createButton(RenderContext* ctx, SDL_Rect rect, buttonCallback callback){
    Button* btn = malloc(sizeof(Button));
    if (btn != NULL) {
        btn->rect = rect;
        btn->callback = callback;
        btn->tex = createButtonTexture(ctx->renderer, &rect);
        btn->shadow_tex = generateShadowFromTexture(ctx->renderer, btn->tex, BUTTON_SHADOW_RADIUS, BUTTON_SHADOW_STRENGTH);
    }

    return btn;
}

void destroyButton(Button* btn) {
    free(btn);
}

void renderButton(SDL_Renderer* rend, Button* btn) {
    SDL_Rect shadow_rect = { 
        .x = btn->rect.x + BUTTON_SHADOW_OFFSET_X - BUTTON_SHADOW_RADIUS,
        .y = btn->rect.y + BUTTON_SHADOW_OFFSET_Y - BUTTON_SHADOW_RADIUS, 
        .w = btn->rect.w + 2*BUTTON_SHADOW_RADIUS, 
        .h = btn->rect.h + 2*BUTTON_SHADOW_RADIUS 
    };

    SDL_RenderCopy(rend, btn->shadow_tex, NULL, &shadow_rect); // render button shadow

    SDL_RenderCopy(rend, btn->tex, NULL, &btn->rect); // render button texture
}

void destroyUI() {
    SDL_DestroyTexture(s_background_texture);
}
