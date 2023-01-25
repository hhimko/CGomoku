// Copyright (c) Dawid "hhimko" Cyganek <dawid.cyganek1@gmail.com>. Licensed under the MIT Licence.
// See the LICENCE file in the repository root for full licence text.

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <SDL.h>


/* Helper function for reading the texture dimenstions. */
static inline int getTextureSize(SDL_Texture* tex, int* w, int* h) {
    return SDL_QueryTexture(tex, NULL, NULL, w, h);
}

uint32_t* getTexturePixelArray(SDL_Renderer* rend, SDL_Texture* tex, int* w, int* h);

/* Generate a 2x2 pixel "missing" texture. */
SDL_Texture* loadMissingTexture(SDL_Renderer* rend);

/* 
 *  Load a SDL_Texture from a BMP image. 
 *  A default "missing" texture is loaded on fail. 
 */
SDL_Texture* loadTextureBMP(SDL_Renderer* rend, const char* const file);

/* 
 * Generate a solid color 1x1 pixel texture.
 * Used for debugging purposes.
 */
SDL_Texture* generateSolidTexture(SDL_Renderer* rend, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
SDL_Texture* generateSolidCircleTexture(SDL_Renderer* rend, int size, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
SDL_Texture* generateSeigaihaTexture(SDL_Renderer* rend, uint32_t size, uint8_t ring_count, double ring_thickness, SDL_Color* bg_color,  SDL_Color* fg_color);

/* 
 * Generates a drop-shadow texture from an existing texture, based on its alpha channel.
 * The function is not efficient enough to be called frequently, because of the inner call to `getTexturePixelArray`.
 * The separated box blur kernel is used with accumulation for optimization.  
 */
SDL_Texture* generateShadowFromTexture(SDL_Renderer* rend, SDL_Texture* tex, int blur_radius, uint8_t strength);

void renderTextureRepeat(SDL_Renderer* rend, SDL_Texture* tex, const SDL_Rect* dstrect);

#endif /* TEXTURE_H_ */
