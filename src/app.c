#include "./app.h"

#include <stdio.h>
#include <SDL.h>

#include "./scenes/scene.h"
#include "./sdl/render.h"


AppState* createAppState(SDL_Renderer* rend, uint8_t fps) {
    AppState* state = malloc(sizeof(AppState));

    state->renderer = rend;
    state->fps = fps;

    state->update = NULL;
    state->render = NULL;
    state->scene = -1;
    setScene(state, SCENE_MENU); // sets members: scene, update, render

    return state;
}

void destroyAppState(AppState* state) {
    free(state);
}

void mainloop(AppState* state) {
    SDL_bool running = SDL_TRUE;

    while (running) {
        state->update(1);
        running = SDL_FALSE;
    }
}

int Gomoku_run(int argc, char* argv[]) {
    (void) argc; // |
    (void) argv; // | unused

    SDL_Window* const window = NULL;
    SDL_Renderer* const renderer = NULL;

    if (initSDL(renderer, window) < 0) {
        fprintf(stderr, "Failed to initialize SDL.\n SDL_Error: %s\n", SDL_GetError()); 
        return -1;
    }

    uint8_t fps = 60;
    AppState* state = createAppState(renderer, fps);
    mainloop(state);

    /* free up resources after the mainloop quits */
    destroyAppState(state);
    quitSDL(renderer, window);
    return 0;
}
