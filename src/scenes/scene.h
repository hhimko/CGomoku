#ifndef SCENE_H_
#define SCENE_H_

#include <SDL.h>

#include "../sdl/render.h"


struct AppState; // \skip AppState redeclaration to avoid circular imports 

typedef enum SceneState {
    SCENE_MENU,
    SCENE_GAME
} SceneState;

/* Holds all callbacks used for handling the current app scene */
typedef struct Scene Scene;

typedef void (*sceneUpdateCallback)(uint64_t dt);
typedef void (*sceneRenderCallback)(RenderContext* ctx);
typedef int (*sceneHandleInputCallback)(SDL_Event* event, struct AppState* state);

struct Scene {
    SceneState state;
    sceneUpdateCallback update;
    sceneRenderCallback render;
    sceneHandleInputCallback handle_input;
};

void setScene(struct AppState* state, SceneState new_scene_state);

#endif /* SCENE_H_ */
