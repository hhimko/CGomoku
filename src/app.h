#ifndef APP_H_
#define APP_H_

#include <SDL.h>

#include "./scenes/scene.h"


typedef void (*sceneUpdateCallback)(int);
typedef void (*sceneRenderCallback)(SDL_Renderer*);

typedef struct AppState AppState;

struct AppState {
    SDL_Renderer* renderer;
    Scene scene;
    sceneUpdateCallback update;
    sceneRenderCallback render;
    uint8_t fps;
};

AppState* createAppState(SDL_Renderer* rend, uint8_t fps);
void destroyAppState(AppState* state);

/* Gomoku app entry point */
int Gomoku_run(int argc, char* argv[]);

#endif /* APP_H_ */
