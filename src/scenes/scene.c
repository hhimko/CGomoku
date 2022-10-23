#include "./scene.h"

#include <assert.h>

#include "./menu.h"
#include "../app.h"


void setScene(AppState* state, Scene scene) {
    state->scene = scene;

    switch (scene) {
        case SCENE_MENU:
            setMenuSceneCallbacks(state);
            break;
        default:
            assert(!"This line should never be reached.");
            break;
    }

    assert(state->update);       // |
    assert(state->render);       // | 
    assert(state->handle_input); // | all callbacks should always be set after calling the function
}
