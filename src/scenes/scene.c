#include "./scene.h"

#include <assert.h>

#include "./menu.h"
#include "../app.h"


void setScene(AppState* state, SceneState new_scene_state) {
    state->scene.state = new_scene_state;

    switch (new_scene_state) {
        case SCENE_MENU:
            setMenuSceneCallbacks(state);
            break;
        default:
            assert(!"This line should never be reached.");
            break;
    }

    assert(state->scene.update);       // |
    assert(state->scene.render);       // | 
    assert(state->scene.handle_input); // | all callbacks should always be set after calling the function
}
