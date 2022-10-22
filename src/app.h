#ifndef APP_H_
#define APP_H_

#include <SDL.h>

#include "./scenes/scene.h"


typedef void (*sceneUpdateCallback)(uint64_t dt);
typedef void (*sceneRenderCallback)(SDL_Renderer* rend);
typedef void (*sceneHandleInputCallback)(SDL_Event* event);

typedef struct AppState AppState;

struct AppState {
    SDL_Renderer* renderer;
    Scene scene;
    sceneUpdateCallback update;
    sceneRenderCallback render;
    sceneHandleInputCallback handle_input;
    uint16_t fps_cap;
};

AppState* createAppState(SDL_Renderer* rend, uint16_t fps);
void destroyAppState(AppState* state);

/* Gomoku app entry point */
int Gomoku_run(int argc, char* argv[]);

#endif /* APP_H_ */
