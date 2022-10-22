#ifndef RENDER_H_
#define RENDER_H_

#include <SDL.h>

int initSDL(SDL_Renderer* rend, SDL_Window* win);
void quitSDL(SDL_Renderer* rend, SDL_Window* win);

#endif /* RENDER_H_ */
