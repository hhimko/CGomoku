#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <SDL.h>


/* Helper function for reading the texture dimenstions. */
static inline int getTextureSize(SDL_Texture* tex, int* w, int* h) {
    return SDL_QueryTexture(tex, NULL, NULL, w, h);
}

/* Generate a 2x2 pixel "missing" texture. */
SDL_Texture* loadMissingTexture(SDL_Renderer* rend);

/* 
 *  Load a SDL_Texture from a BMP image. 
 *  A default "missing" texture is loaded on fail. 
 */
SDL_Texture* loadTextureBMP(SDL_Renderer* rend, const char* const file);

SDL_Texture* generateSeigaihaTexture(SDL_Renderer* rend, uint32_t size, uint8_t ring_count, double ring_thickness, SDL_Color* bg_color,  SDL_Color* fg_color);

#endif /* TEXTURE_H_ */
