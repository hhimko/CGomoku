#include "./ui.h"

#include <SDL.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>

#include "./sdl/render.h"

#define CLAMP(a, b, x) (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))

#define SMOOTHSTEP_RESOLUTION 100
#define CURSOR_ANIMATION_STEP 0.0005
#define CURSOR_ANIMATION_STRENGTH 15.0f


static double* __s_smoothstep_arr = NULL; 
static double s_cursor_animation_t = 0.0;


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

Button* createButton(SDL_Rect* rect, buttonCallback callback){
    Button* btn = malloc(sizeof(Button));

    if (btn != NULL) {
        btn->rect = *rect;
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
}

void destroyUI() {
    free(__s_smoothstep_arr);
}
