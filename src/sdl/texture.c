#include "./texture.h"

#include <stdio.h>

#include "./render.h"


SDL_Texture* loadMissingTexture(SDL_Renderer* rend){
    SDL_Texture* tex = SDL_CreateTexture(rend, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 2, 2);
    
    if (tex != NULL && SDL_SetRenderTarget(rend, tex) >= 0) {
        SDL_SetRenderDrawColor(rend, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawPoint(rend, 0, 0);
        SDL_RenderDrawPoint(rend, 1, 1);

        SDL_SetRenderDrawColor(rend, 0xFF, 0x00, 0xFF, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawPoint(rend, 0, 1);
        SDL_RenderDrawPoint(rend, 1, 0);

        // detach texture from renderer 
        if (SDL_SetRenderTarget(rend, NULL) >= 0) {
            return tex;
        }
    }

    fprintf(stderr, "Failed to generate the \"missing\" texture. \nSDL_Error: %s\n", SDL_GetError());
    SDL_DestroyTexture(tex);
    return NULL;
}

SDL_Texture* loadTextureBMP(SDL_Renderer* rend, const char* const file) {
    SDL_Surface* surf = SDL_LoadBMP(file);
    if (surf == NULL) {
        fprintf(stderr, "Failed to load BMP \"%s\". \nSDL_Error: %s\n", file, SDL_GetError());
        return loadMissingTexture(rend);
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surf);
    if (tex == NULL) {
        fprintf(stderr, "Failed to create texture from surface. \nSDL_Error: %s\n", SDL_GetError());
    }

    SDL_FreeSurface(surf); 

    return tex;
}

SDL_Texture* generateSolidTexture(SDL_Renderer* rend, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_Texture* tex = SDL_CreateTexture(rend, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 1, 1);
    
    if (tex != NULL && SDL_SetRenderTarget(rend, tex) >= 0) {
        SDL_SetRenderDrawColor(rend, r, g, b, a);
        SDL_RenderDrawPoint(rend, 0, 0);

        // detach texture from renderer 
        if (SDL_SetRenderTarget(rend, NULL) >= 0) {
            return tex;
        }
    }

    fprintf(stderr, "Failed to generate solid color texture. \nSDL_Error: %s\n", SDL_GetError());
    SDL_DestroyTexture(tex);
    return NULL;
}

void __drawSeigaihaSegemntRings(SDL_Renderer* rend, int x, int y, double rad, uint8_t ring_count, double ring_thickness) {
    double radoff = rad / ring_count; 

    for (int ring = 0; ring < ring_count; ++ring) {
        drawCircleBorderAA(rend, x, y, rad, ring_thickness, BORDER_TYPE_INNER);

        rad -= radoff;
    }
}

SDL_Texture* generateSeigaihaTexture(SDL_Renderer* rend, uint32_t size, uint8_t ring_count, double ring_thickness, SDL_Color* bg_color,  SDL_Color* fg_color) {
    SDL_Texture* tex = SDL_CreateTexture(rend, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, size * 2, size);

    uint8_t r,g,b,a;
    SDL_GetRenderDrawColor(rend, &r, &g, &b, &a);

    if (tex != NULL && SDL_SetRenderTarget(rend, tex) >= 0) {
        uint8_t bg_r, bg_g, bg_b; // |
        uint8_t fg_r, fg_g, fg_b; // | alpha is ignored

        double thickness = size * ring_thickness;
        double rad = (double)size;
        
        bg_r = bg_color->r;
        bg_g = bg_color->g;
        bg_b = bg_color->b;

        fg_r = fg_color->r;
        fg_g = fg_color->g;
        fg_b = fg_color->b;

        SDL_SetRenderDrawColor(rend, bg_r, bg_g, bg_b, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(rend);

        // top corners
        SDL_SetRenderDrawColor(rend, fg_r, fg_g, fg_b, SDL_ALPHA_OPAQUE);
        __drawSeigaihaSegemntRings(rend, 0, size / 2, rad, ring_count, thickness);
        __drawSeigaihaSegemntRings(rend, size * 2, size / 2, rad, ring_count, thickness);
        
        // center
        SDL_SetRenderDrawColor(rend, bg_r, bg_g, bg_b, SDL_ALPHA_OPAQUE);
        drawFilledCircleAA(rend, size, size, rad - thickness);

        SDL_SetRenderDrawColor(rend, fg_r, fg_g, fg_b, SDL_ALPHA_OPAQUE);
        __drawSeigaihaSegemntRings(rend, size, size, rad, ring_count, thickness);

        // bottom corners 
        SDL_SetRenderDrawColor(rend, bg_r, bg_g, bg_b, SDL_ALPHA_OPAQUE);
        drawFilledCircleAA(rend, 0, size + size / 2, rad - thickness);
        drawFilledCircleAA(rend, size * 2, size + size / 2, rad - thickness);

        SDL_SetRenderDrawColor(rend, fg_r, fg_g, fg_b, SDL_ALPHA_OPAQUE);
        __drawSeigaihaSegemntRings(rend, 0, size + size / 2, rad, ring_count, thickness);
        __drawSeigaihaSegemntRings(rend, size * 2, size + size / 2, rad, ring_count, thickness);


        SDL_SetRenderDrawColor(rend, r, g, b, a); // restore original color

        // detach texture from renderer 
        if (SDL_SetRenderTarget(rend, NULL) >= 0) {
            return tex;
        }
    }

    fprintf(stderr, "Failed to generate the \"seigaiha\" texture. \nSDL_Error: %s\n", SDL_GetError());
    SDL_DestroyTexture(tex);
    return NULL;
}

void renderTextureRepeat(SDL_Renderer* rend, SDL_Texture* tex, const SDL_Rect* dstrect) {
    int tex_w, tex_h;
    
    if (getTextureSize(tex, &tex_w, &tex_h) < 0) {
        fprintf(stderr, "Failed to query texture size. \nSDL_Error: %s\n", SDL_GetError());
        return;
    }

    int x, y, w, h;
    x = dstrect->x;
    y = dstrect->y;
    w = dstrect->w;
    h = dstrect->h;

    SDL_Rect dest = { .x = x, .y = y, .w = tex_w, .h = tex_h };
    int xiters = (int)(w / tex_w);
    int yiters = (int)(h / tex_h);

    int remaining_w = w - tex_w * xiters;
    int remaining_h = h - tex_h * yiters;
    SDL_Rect src = { .x = 0, .y = 0, .w = remaining_w, .h = tex_h };

    for (int yi = 0; yi < yiters; ++yi) {
        dest.x = x;

        for (int xi = 0; xi < xiters; ++xi) {
            SDL_RenderCopy(rend, tex, NULL, &dest);
            dest.x += tex_w;
        }

        // render the remaining texture, cut in width (right-most column)
        if (remaining_w > 0) {
            dest.w = remaining_w;
            SDL_RenderCopy(rend, tex, &src, &dest);
            dest.w = tex_w;
        }

        dest.y += tex_h;
    }

    // render the remaining texture, cut in height (bottom row)
    if (remaining_h > 0) {
        dest.x = x;
        dest.h = remaining_h;

        src.w = tex_w;
        src.h = remaining_h;

        for (int xi = 0; xi < xiters; ++xi) {
            SDL_RenderCopy(rend, tex, &src, &dest);
            dest.x += tex_w;
        }

        // render the remaining texture, cut in both width and height (bottom-right corner)
        if (remaining_w > 0) {
            src.w = remaining_w;
            dest.w = remaining_w;
            SDL_RenderCopy(rend, tex, &src, &dest);
        }
    }
}
