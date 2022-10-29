#ifndef APP_H_
#define APP_H_

#include <SDL.h>

#include "./scenes/scene.h"
#include "./sdl/render.h"


/* Structure containing application state variables, shared among all game scenes. */
typedef struct AppState AppState;

struct AppState {
    RenderContext* context;
    Scene scene;
    uint16_t fps_cap;
};

AppState* createAppState(RenderContext* ctx, uint16_t fps);
void destroyAppState(AppState* state);

/* Gomoku app entry point */
int Gomoku_run(int argc, char* argv[]);

#endif /* APP_H_ */
