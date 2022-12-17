#include "./ui.h"

#include <SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "./sdl/render.h"
#include "./sdl/texture.h"


/* SMOOTHSTEP FUNCTION  */
#define SMOOTHSTEP_RESOLUTION 100
#define CURSOR_ANIMATION_STEP 0.0005
#define CURSOR_ANIMATION_STRENGTH 15.0f

static double* __s_smoothstep_arr = NULL; 
static double s_cursor_animation_t = 0.0;

/* SIGAIHA BACKGROUND */
#define SEIGAIHA_RADIUS 80
#define SEIGAIHA_ANIMATION_STEP 0.0003

static int s_seigaiha_parallax_x = 0;
static int s_seigaiha_parallax_y = 0;
static int s_seigaiha_direction_x = 1;
static int s_seigaiha_direction_y = 1;

static double s_background_offset = 0.0;
static SDL_Texture* s_background_texture = NULL;

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define CLAMP(a, b, x) (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))


double smoothstep(double x) {
    int t = (int)(x * SMOOTHSTEP_RESOLUTION + 0.5);
    return __s_smoothstep_arr[CLAMP(0, SMOOTHSTEP_RESOLUTION - 1, t)];
}

double smoothstep_inout(double x) {
    int t = (int)((1 - fabs((x - 0.5)*2)) * SMOOTHSTEP_RESOLUTION + 0.5);
    return __s_smoothstep_arr[CLAMP(0, SMOOTHSTEP_RESOLUTION - 1, t)];
}

int precomputeSmoothstep() {
    __s_smoothstep_arr = malloc(sizeof(double) * SMOOTHSTEP_RESOLUTION);
    if (__s_smoothstep_arr == NULL)
        return -1;

    for (int i = 0; i < SMOOTHSTEP_RESOLUTION; ++i) {
        double t = (double)i / (SMOOTHSTEP_RESOLUTION - 1);
        __s_smoothstep_arr[i] = t * t * (3.0 - t * 2.0);
    }

    return 0;
}

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

void renderSelectionCursor(SDL_Renderer* rend, SDL_Rect* rect) {
    static const float thickness = 10.0f; 
    static const float length = 30; 
    float offset = 10.0f + CURSOR_ANIMATION_STRENGTH * (float)smoothstep_inout(s_cursor_animation_t); 

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
    float offset = 10.0f + CURSOR_ANIMATION_STRENGTH * (float)smoothstep_inout(s_cursor_animation_t); 

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

Button* createButton(SDL_Rect rect, buttonCallback callback){
    Button* btn = malloc(sizeof(Button));

    if (btn != NULL) {
        btn->rect = rect;
        btn->callback = callback;
    }

    return btn;
}

void destroyButton(Button* btn) {
    free(btn);
}

void renderButton(SDL_Renderer* rend, Button* btn) {
    uint8_t r,g,b,a;
    SDL_GetRenderDrawColor(rend, &r, &g, &b, &a);

    SDL_SetRenderDrawColor(rend, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(rend, &btn->rect);

    SDL_SetRenderDrawColor(rend, r, g, b, a); // restore original color
}

int initializeUI() {
    s_cursor_animation_t = 0.0;

    if (precomputeSmoothstep() < 0) {
        fprintf(stderr, "Failed to precompute smoothstep array.\n");
        return -1;
    }

    return 0;   
}

void updateUI(uint64_t dt) {
    s_cursor_animation_t = s_cursor_animation_t + CURSOR_ANIMATION_STEP*dt;
    if (s_cursor_animation_t >= 1.0) 
        s_cursor_animation_t -= (int)s_cursor_animation_t;

    s_background_offset = s_background_offset + SEIGAIHA_ANIMATION_STEP*dt;
    if (s_background_offset >= 1.0) 
        s_background_offset -= (int)s_background_offset;
}

void destroyUI() {
    free(__s_smoothstep_arr);
    SDL_DestroyTexture(s_background_texture);
}
