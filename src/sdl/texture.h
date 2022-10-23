#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <SDL.h>

/* Generate a 2x2 pixel "missing" texture. */
SDL_Texture* loadMissingTexture(SDL_Renderer* rend);

/* 
 *  Load a SDL_Texture from a BMP image. 
 *  A default "missing" texture is loaded on fail. 
 */
SDL_Texture* loadTextureBMP(SDL_Renderer* rend, const char* const file);

#endif /* TEXTURE_H_ */