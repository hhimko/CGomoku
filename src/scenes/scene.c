#include "./scene.h"

#include <assert.h>
#include <stdio.h>

#include "../app.h"
#include "./menu.h"
#include "./game.h"
#include "./options.h"


void defaultSceneUpdateCallback(uint64_t dt) { 
    (void) dt; 
    fprintf(stderr, "Default scene update callback implementation has not been overriden");
}

void defaultSceneRenderCallback(RenderContext* ctx) { 
    (void) ctx; 
    fprintf(stderr, "Default scene render callback implementation has not been overriden");
}

SDL_bool defaultSceneHandleInputCallback(SDL_Event* event, AppState* state) { 
    (void) event;
    (void) state; 
    fprintf(stderr, "Default scene handleInput callback implementation has not been overriden");
    return SDL_FALSE;
}

void setDefaultSceneCallbacks(AppState* state) {
    state->scene.update = defaultSceneUpdateCallback;
    state->scene.render = defaultSceneRenderCallback;
    state->scene.handle_input = defaultSceneHandleInputCallback;
}

int setScene(AppState* state, SceneState new_scene_state) {
    if (state->scene.destroy != NULL) // scene destroy callback has to be null-initialized before setScene
        state->scene.destroy(); 
    state->scene.state = new_scene_state;
    setDefaultSceneCallbacks(state);

    switch (new_scene_state) {
        case SCENE_MENU:
            return menuPrepare(state);
        case SCENE_OPTIONS:
            return optionsPrepare(state);
        case SCENE_GAME:
            return gamePrepare(state);
    }

    fprintf(stderr, "Failed to change scene. Unexpected SceneState - %d\n", (int)new_scene_state);
    return -1;
}
