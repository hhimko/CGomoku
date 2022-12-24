#include "./sdl/render.h"

#include <math.h>
#include <assert.h>

#define SQRT2 1.41421356237


void drawFilledFRect(SDL_Renderer* rend, FRect* frect) {
    uint8_t r,g,b,a,alpha;
    SDL_GetRenderDrawColor(rend, &r, &g, &b, &a);

    float sx = frect->w < 0 ? frect->x + frect->w : frect->x;
    float sy = frect->h < 0 ? frect->y + frect->h : frect->y;
    float w = frect->w < 0 ? -frect->w : frect->w;
    float h = frect->h < 0 ? -frect->h : frect->h;
    float ex = sx + w;
    float ey = sy + h;

    // top
    alpha = (uint8_t)((1.0 - (sy - (int)sy)) * 255.0);
    SDL_SetRenderDrawColor(rend, r, g, b, alpha);
    SDL_RenderDrawLine(rend, (int)sx, (int)sy - 1, (int)ex - 1, (int)sy - 1);

    // bottom
    alpha = (uint8_t)((ey - (int)ey) * 255.0);
    SDL_SetRenderDrawColor(rend, r, g, b, alpha);
    SDL_RenderDrawLine(rend, (int)sx, (int)ey, (int)ex - 1, (int)ey);

    // left 
    alpha = (uint8_t)((1.0 - (sx - (int)sx)) * 255.0);
    SDL_SetRenderDrawColor(rend, r, g, b, alpha);
    SDL_RenderDrawLine(rend, (int)sx - 1, (int)sy, (int)sx - 1, (int)ey - 1);

    // right
    alpha = (uint8_t)((ex - (int)ex) * 255.0);
    SDL_SetRenderDrawColor(rend, r, g, b, alpha);
    SDL_RenderDrawLine(rend, (int)ex, (int)sy, (int)ex, (int)ey - 1);


    // center
    SDL_Rect crect = {
        .x = (int)sx,
        .y = (int)sy,
        .w = (int)ex - (int)sx,
        .h = (int)ey - (int)sy
    };
    SDL_SetRenderDrawColor(rend, r, g, b, a); // restore original color
    SDL_RenderFillRect(rend, &crect);
}

void drawFRectBorder(SDL_Renderer* rend, FRect* frect, float thickness, BorderType border_type) {
    const float off = thickness * (float)border_type / 2.0f; // offset relies on BorderType integer values 
    const float off2 = off * 2.0f;

    FRect line_frect = {
        .x = frect->x - off,
        .y = frect->y - off,
        .w = frect->w + off2,
        .h = thickness
    };

    // top
    drawFilledFRect(rend, &line_frect);

    // bottom
    line_frect.y += frect->h + off2 - thickness; 
    drawFilledFRect(rend, &line_frect);

    // left
    line_frect.y = frect->y - off;
    line_frect.w = thickness;
    line_frect.h = frect->w + off2;
    drawFilledFRect(rend, &line_frect);

    // right
    line_frect.x += frect->w + off2 - thickness;
    drawFilledFRect(rend, &line_frect);
}

void drawCircleAA(SDL_Renderer* rend, int x, int y, double rad) {
    /* 
     * Slightly modified algorithm based on Xiaolin Wu's method. 
     * https://yellowsplash.wordpress.com/2009/10/23/fast-antialiased-circles-and-ellipses-from-xiaolin-wus-concepts/
     */
    const double radsq = rad*rad; 
    const int ffd = (int)round(rad / SQRT2); // 45 degree pixel x coordinate

    uint8_t r,g,b,a,alpha;
    SDL_GetRenderDrawColor(rend, &r, &g, &b, &a);

    for (int xi = 0; xi <= ffd; ++xi) {
        double yi = sqrt(radsq - xi*xi); // y coordinate on the circle for the iterated x

        int ipart = (int)yi; // integer part of the calculated real
        double fpart = yi - ipart; // fractional part of the calculated real

        // draw the first point in 8-symmetry
        alpha = (uint8_t)((1.0 - fpart) * 255.0);
        SDL_SetRenderDrawColor(rend, r, g, b, alpha);

        SDL_RenderDrawPoint(rend, x + xi, y + ipart);
        SDL_RenderDrawPoint(rend, x - xi, y + ipart);
        SDL_RenderDrawPoint(rend, x - xi, y - ipart);
        SDL_RenderDrawPoint(rend, x + xi, y - ipart);

        SDL_RenderDrawPoint(rend, x + ipart, y + xi);
        SDL_RenderDrawPoint(rend, x - ipart, y + xi);
        SDL_RenderDrawPoint(rend, x - ipart, y - xi);
        SDL_RenderDrawPoint(rend, x + ipart, y - xi);

        // draw the offset point in 8-symmetry
        ipart++; 
        alpha = (uint8_t)(fpart * 255.0);
        SDL_SetRenderDrawColor(rend, r, g, b, alpha);

        SDL_RenderDrawPoint(rend, x + xi, y + ipart);
        SDL_RenderDrawPoint(rend, x - xi, y + ipart);
        SDL_RenderDrawPoint(rend, x - xi, y - ipart);
        SDL_RenderDrawPoint(rend, x + xi, y - ipart);

        SDL_RenderDrawPoint(rend, x + ipart, y + xi);
        SDL_RenderDrawPoint(rend, x - ipart, y + xi);
        SDL_RenderDrawPoint(rend, x - ipart, y - xi);
        SDL_RenderDrawPoint(rend, x + ipart, y - xi);
    }

    SDL_SetRenderDrawColor(rend, r, g, b, a); // restore original color
}

void drawFilledCircleAA(SDL_Renderer* rend, int x, int y, double rad) {
    // Algorithm based on Xiaolin Wu's method, modified and optimized for drawing filled circle borders.
    const double radsq = rad*rad; 
    const int ffd = (int)round(rad / SQRT2); // 45 degree pixel x coordinate

    uint8_t r,g,b,a,alpha;
    SDL_GetRenderDrawColor(rend, &r, &g, &b, &a);

    for (int xi = 0; xi <= ffd; ++xi) {
        double yi = sqrt(radsq - xi*xi); // y coordinate on the circle for the iterated x

        int ipart = (int)yi; // integer part of the calculated real
        double fpart = yi - ipart; // fractional part of the calculated real

        // fill the circle with scan-lines
        SDL_SetRenderDrawColor(rend, r, g, b, a);
        SDL_RenderDrawLine(rend, x + xi, y + ipart, x - xi, y + ipart);
        SDL_RenderDrawLine(rend, x - xi, y - ipart, x + xi, y - ipart);

        SDL_RenderDrawLine(rend, x + ipart, y + xi, x - ipart, y + xi);
        SDL_RenderDrawLine(rend, x - ipart, y - xi, x + ipart, y - xi);

        // draw the offset point in 8-symmetry
        ipart++; 
        alpha = (uint8_t)(fpart * 255.0);
        SDL_SetRenderDrawColor(rend, r, g, b, alpha);

        SDL_RenderDrawPoint(rend, x + xi, y + ipart);
        SDL_RenderDrawPoint(rend, x - xi, y + ipart);
        SDL_RenderDrawPoint(rend, x - xi, y - ipart);
        SDL_RenderDrawPoint(rend, x + xi, y - ipart);

        SDL_RenderDrawPoint(rend, x + ipart, y + xi);
        SDL_RenderDrawPoint(rend, x - ipart, y + xi);
        SDL_RenderDrawPoint(rend, x - ipart, y - xi);
        SDL_RenderDrawPoint(rend, x + ipart, y - xi);
    }

    SDL_SetRenderDrawColor(rend, r, g, b, a); // restore original color
}

void drawCircleBorderAA(SDL_Renderer* rend, int x, int y, double rad, double thickness, BorderType border_type) {
    // Algorithm based on Xiaolin Wu's method, modified and optimized for drawing filled circles.
    const double rad_off = thickness * (double)border_type / 2.0f; // offset relies on BorderType integer values 

    const double rado = rad + rad_off;         // outer circle radius
    const double radosq = rado*rado;           // outer circle radius squared
    const int ffdo = (int)round(rado / SQRT2); // outer cirlce 45 degree pixel x coordinate

    const double radi = rad + rad_off - thickness; // inner circle radius
    const double radisq = radi*radi;               // inner circle radius squared
    const int ffdi = (int)round(radi / SQRT2);     // inner cirlce 45 degree pixel x coordinate

    // precalculate the last y coordinate on the inner circle
    const double maxiyi = sqrt(radisq - ffdi*ffdi);

    uint8_t r,g,b,a,alpha;
    SDL_GetRenderDrawColor(rend, &r, &g, &b, &a);

    for (int xi = 0; xi <= ffdo; ++xi) {
        double oyi = sqrt(radosq - xi*xi); // y coordinate on the outer circle for the iterated x
        
        // y coordinate on the inner circle for the iterated x
        double iyi = maxiyi; 
        if (xi < ffdi) {
            iyi = sqrt(radisq - xi*xi);
        }

        // integer part of the calculated real
        int oipart = (int)oyi; 
        int iipart = (int)iyi; 

        // fractional part of the calculated real
        double ofpart = oyi - oipart; 
        double ifpart = iyi - iipart; 

        // fill the circle with scan-lines
        SDL_SetRenderDrawColor(rend, r, g, b, a);

        SDL_RenderDrawLine(rend, x + xi, y + oipart, x + xi, y + iipart);
        SDL_RenderDrawLine(rend, x - xi, y + oipart, x - xi, y + iipart);
        SDL_RenderDrawLine(rend, x + xi, y - oipart, x + xi, y - iipart);
        SDL_RenderDrawLine(rend, x - xi, y - oipart, x - xi, y - iipart);

        SDL_RenderDrawLine(rend, x + oipart, y + xi, x + iipart, y + xi);
        SDL_RenderDrawLine(rend, x - oipart, y + xi, x - iipart, y + xi);
        SDL_RenderDrawLine(rend, x + oipart, y - xi, x + iipart, y - xi);
        SDL_RenderDrawLine(rend, x - oipart, y - xi, x - iipart, y - xi);

        // draw the outer offset point in 8-symmetry
        oipart++; 
        alpha = (uint8_t)(ofpart * 255.0);
        SDL_SetRenderDrawColor(rend, r, g, b, alpha);

        SDL_RenderDrawPoint(rend, x + xi, y + oipart);
        SDL_RenderDrawPoint(rend, x - xi, y + oipart);
        SDL_RenderDrawPoint(rend, x - xi, y - oipart);
        SDL_RenderDrawPoint(rend, x + xi, y - oipart);

        SDL_RenderDrawPoint(rend, x + oipart, y + xi);
        SDL_RenderDrawPoint(rend, x - oipart, y + xi);
        SDL_RenderDrawPoint(rend, x - oipart, y - xi);
        SDL_RenderDrawPoint(rend, x + oipart, y - xi);

        //draw the inner offset point in 8-symmetry
        if (xi <= ffdi) {
            iipart--;
            alpha = (uint8_t)((1.0 - ifpart) * 255.0);
            SDL_SetRenderDrawColor(rend, r, g, b, alpha);

            SDL_RenderDrawPoint(rend, x + xi, y + iipart);
            SDL_RenderDrawPoint(rend, x - xi, y + iipart);
            SDL_RenderDrawPoint(rend, x - xi, y - iipart);
            SDL_RenderDrawPoint(rend, x + xi, y - iipart);

            SDL_RenderDrawPoint(rend, x + iipart, y + xi);
            SDL_RenderDrawPoint(rend, x - iipart, y + xi);
            SDL_RenderDrawPoint(rend, x - iipart, y - xi);
            SDL_RenderDrawPoint(rend, x + iipart, y - xi);
        }
    }

    SDL_SetRenderDrawColor(rend, r, g, b, a); // restore original color
}

RenderContext* createRenderContext(int win_w, int win_h) {
    RenderContext* ctx = malloc(sizeof(RenderContext));

    ctx->renderer = NULL; // renderer should only ever be created and assigned in `initSDL`
    ctx->win_w = win_w;
    ctx->win_h = win_h; 

    return ctx;
}

void destroyRenderContext(RenderContext* ctx) {
    assert(ctx->renderer == NULL || "SDL_Renderer* should be freed before calling destroyRenderContext()");
    free(ctx);
}

int initSDL(RenderContext* ctx, SDL_Window** win) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return -1;

    Uint32 winFlags = 0;
    *win = SDL_CreateWindow("CGomoku", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        ctx->win_w, ctx->win_h, winFlags);
    if (win == NULL) return -1;

    Uint32 rendFlags = SDL_RENDERER_ACCELERATED;
    ctx->renderer = SDL_CreateRenderer(*win, -1, rendFlags);
    if (ctx->renderer == NULL) return -1;

    SDL_SetHint("SDL_HINT_RENDER_SCALE_QUALITY", "linear");
    if (SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_BLEND) < 0) return -1;

    return 0;
}

void quitSDL(RenderContext* ctx, SDL_Window* win) {
    SDL_DestroyRenderer(ctx->renderer);
    SDL_DestroyWindow(win);

    SDL_Quit();
}
