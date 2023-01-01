#include "./texture.h"

#include <stdio.h>
#include <math.h>

#include "./render.h"

#define MIN(x, y) ((x) < (y) ? (x) : (y))


uint32_t* getTexturePixelArray(SDL_Renderer* rend, SDL_Texture* tex, int* w, int* h) {
    int _w, _h;
    if (getTextureSize(tex, &_w, &_h) < 0) return NULL;
    uint32_t* array = malloc(sizeof(uint32_t)*_w*_h);

    if (w != NULL) *w = _w;
    if (h != NULL) *h = _h;

    // force access type TARGET
    SDL_Texture* access_tex = SDL_CreateTexture(rend, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, _w, _h);
    if (access_tex != NULL && SDL_SetRenderTarget(rend, access_tex) >= 0) {
        SDL_RenderCopy(rend, tex, NULL, NULL);
        int pitch = _w*4; // pitch value is generally the array width, multiplied by the amount of channels per pixel

        SDL_RenderReadPixels(rend, NULL, SDL_PIXELFORMAT_ARGB8888, array, pitch);
    }

    SDL_SetRenderTarget(rend, NULL); // detach texture from renderer
    SDL_DestroyTexture(access_tex);
    return array;
}

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

SDL_Texture* generateSolidCircleTexture(SDL_Renderer* rend, int size, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_Texture* tex = SDL_CreateTexture(rend, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, size, size);
    
    if (tex != NULL && SDL_SetRenderTarget(rend, tex) >= 0) {
        SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

        SDL_SetRenderDrawColor(rend, r, g, b, 0x00);
        SDL_RenderClear(rend);
        
        SDL_SetRenderDrawColor(rend, r, g, b, a);
        drawFilledCircleAA(rend, size/2, size/2, size/2 - 2);

        // detach texture from renderer 
        if (SDL_SetRenderTarget(rend, NULL) >= 0) {
            return tex;
        }
    }

    fprintf(stderr, "Failed to generate solid circle texture. \nSDL_Error: %s\n", SDL_GetError());
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

// SDL_Texture* generateShadowFromTexture(SDL_Renderer* rend, SDL_Texture* tex, int blur_radius, uint8_t strength) {
//     int tex_w, tex_h; 
//     uint32_t* tex_pixeldata = getTexturePixelArray(rend, tex, &tex_w, &tex_h);
    
//     if (tex_pixeldata == NULL) {
//         fprintf(stderr, "Failed to generate shadow from texture. \nSDL_Error: %s\n", SDL_GetError());
//         return NULL;
//     }

//     uint8_t r,g,b,a;
//     SDL_GetRenderDrawColor(rend, &r, &g, &b, &a);

//     const int blur_width = 2*blur_radius;
//     const int shadow_w = tex_w + blur_width;
//     const int shadow_h = tex_h + blur_width;

//     SDL_Texture* shadow_tex = SDL_CreateTexture(rend, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, shadow_w, shadow_h);
//     if (shadow_tex != NULL && SDL_SetRenderTarget(rend, shadow_tex) >= 0) {
//         SDL_SetTextureBlendMode(shadow_tex, SDL_BLENDMODE_BLEND);
                
//         const double s = strength / (double)(blur_width + 1);// strength / pow(2.0*blur_radius + 1.0, 2.0);
//         int x,y,array_offset;
//         uint32_t alpha;
//         double acc;

//         // horizontal pass
//         for (y=0; y < tex_h; ++y) {
//             array_offset = y*tex_w;
//             acc = 0.0;

//             for (x=0; x < shadow_w; ++x) {
//                 if (x < tex_w) {
//                     alpha = (tex_pixeldata[array_offset + x] >> 24) & 0xFF;
//                     if (alpha == SDL_ALPHA_OPAQUE) acc += s;
//                 }

//                 SDL_SetRenderDrawColor(rend, 0x00, 0x00, 0x00, (uint8_t)acc);
//                 SDL_RenderDrawPoint(rend, x, y + blur_radius);

//                 // subtract the leftmost kernel value for the next pass
//                 if (x >= blur_width) {
//                     alpha = (tex_pixeldata[array_offset + x - blur_width] >> 24) & 0xFF;
//                     if (alpha == SDL_ALPHA_OPAQUE) acc -= s;
//                 }
//             }
//         }

//         // vertical pass
//         uint32_t* shadow_pixeldata = getTexturePixelArray(rend, shadow_tex, NULL, NULL);
//         if (shadow_pixeldata != NULL && SDL_SetRenderTarget(rend, shadow_tex) >= 0) {
            
//             int blur_array_offset = blur_radius*shadow_w;
//             for (x=0; x < shadow_w; ++x) {
//                 acc = 0.0;

//                 for (y=0; y < shadow_h; ++y) {
//                     array_offset = y*shadow_w;

//                     if (y < shadow_h - blur_radius) {
//                         alpha = (shadow_pixeldata[array_offset + blur_array_offset + x] >> 24) & 0xFF;
//                         acc += alpha/strength;
//                     }

//                     SDL_SetRenderDrawColor(rend, 0x00, 0x00, 0x00, (uint8_t)acc);
//                     SDL_RenderDrawPoint(rend, x, y);

//                     // subtract the topmost kernel value for the next pass
//                     if (y >= blur_radius) {
//                         alpha = (shadow_pixeldata[array_offset - blur_array_offset + x] >> 24) & 0xFF;
//                         acc -= alpha/strength;
//                     }
//                 }
//             }

//             // detach texture from renderer 
//             if (SDL_SetRenderTarget(rend, NULL) >= 0) {
//                 SDL_SetRenderDrawColor(rend, r, g, b, a); // restore original color
//                 free(shadow_pixeldata);
//                 free(tex_pixeldata);
//                 return shadow_tex;
//             }
//         }

//         free(shadow_pixeldata);
//     }

//     fprintf(stderr, "Failed to generate shadow from texture. \nSDL_Error: %s\n", SDL_GetError());
//     SDL_SetRenderDrawColor(rend, r, g, b, a); // restore original color
//     SDL_SetRenderTarget(rend, NULL);
//     SDL_DestroyTexture(shadow_tex);
//     free(tex_pixeldata);
//     return NULL;
// }

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
