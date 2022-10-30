#include "./scene.h"

#include <assert.h>
#include <stdio.h>

#include "./menu.h"
#include "../app.h"


inline void defaultSceneUpdateCallback(uint64_t dt) { 
    (void) dt; 
    fprintf(stderr, "Default scene update callback implementation has not been overriden");
}

inline void defaultSceneRenderCallback(RenderContext* ctx) { 
    (void) ctx; 
    fprintf(stderr, "Default scene render callback implementation has not been overriden");
}

inline SDL_bool defaultSceneHandleInputCallback(SDL_Event* event, AppState* state) { 
    (void) event, state; 
    fprintf(stderr, "Default scene handleInput callback implementation has not been overriden");
    return SDL_FALSE;
}

void setDefaultSceneCallbacks(AppState* state) {
    state->scene.update = defaultSceneUpdateCallback;
    state->scene.render = defaultSceneRenderCallback;
    state->scene.handle_input = defaultSceneHandleInputCallback;
}

void setScene(AppState* state, SceneState new_scene_state) {
    state->scene.state = new_scene_state;
    setDefaultSceneCallbacks(state);

    switch (new_scene_state) {
        case SCENE_MENU:
            menuPrepare();
            setMenuSceneCallbacks(state);
            break;
        default:
            assert(!"This line should never be reached.");
            break;
    }
}

void destroyScenes() {
    menuDestroy();
}
