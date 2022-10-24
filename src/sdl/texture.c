#include "./texture.h"

#include <stdio.h>


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
