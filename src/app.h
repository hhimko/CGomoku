#ifndef APP_H_
#define APP_H_

#include <SDL.h>

#include "./scenes/scene.h"
#include "./sdl/render.h"


/* Structure containing application state variables, shared among all game scenes. */
typedef struct AppState AppState;

typedef void (*sceneUpdateCallback)(uint64_t dt);
typedef void (*sceneRenderCallback)(RenderContext* ctx);
typedef int (*sceneHandleInputCallback)(SDL_Event* event, AppState* state);

struct AppState {
    RenderContext* context;
    Scene scene;
    sceneUpdateCallback update;
    sceneRenderCallback render;
    sceneHandleInputCallback handle_input;
    uint16_t fps_cap;
};

AppState* createAppState(RenderContext* ctx, uint16_t fps);
void destroyAppState(AppState* state);

/* Gomoku app entry point */
int Gomoku_run(int argc, char* argv[]);

#endif /* APP_H_ */
