#include "./board.h"

#include "./sdl/texture.h"


void renderBoard(SDL_Renderer* rend) {
    SDL_Texture* tex = loadTextureBMP(rend, "./assets/test.bmp");

    SDL_RenderCopy(rend, tex, NULL, NULL);
}
