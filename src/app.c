#include "./app.h"

#include <stdio.h>
#include <math.h>
#include <SDL.h>

#include "./scenes/scene.h"
#include "./sdl/render.h"


AppState* createAppState(SDL_Renderer* rend, uint16_t fps_cap) {
    AppState* state = malloc(sizeof(AppState));

    state->renderer = rend;
    state->fps_cap = fps_cap;

    state->update = NULL;
    state->render = NULL;
    state->handle_input = NULL;
    state->scene = -1;
    setScene(state, SCENE_MENU); // sets members: scene, update, render, handle_input

    return state;
}

void destroyAppState(AppState* state) {
    free(state);
}

int handleEvents(AppState* state) {
    static SDL_Event e;

    while(SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            return -1;
        }
        
        state->handle_input(&e);
    }

    return 0;
}

void mainloop(AppState* state) {
    SDL_bool running = SDL_TRUE;
    SDL_Renderer* renderer = state->renderer; 

    const uint64_t ms_per_frame = (uint64_t)round(1000.0 / (double)state->fps_cap);

    uint64_t f_start = SDL_GetTicks64() - ms_per_frame;
    while (running)
    {
        uint64_t f_current = SDL_GetTicks64();
        uint64_t f_elapsed = f_current - f_start;
        f_start = f_current;

        const uint64_t fps = (uint64_t)round(1000.0 / (double)f_elapsed);
        printf("%lld\n", fps);

        // handle user input
        if (handleEvents(state) < 0) {
            running = SDL_FALSE;
        }

        // update logic state
        state->update(f_elapsed);

        // render game to screen
        SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        state->render(renderer);
        SDL_RenderPresent(renderer);

        // wait the remaining frame time to minimalize CPU load
        uint64_t f_end = SDL_GetTicks64();
        if (f_start + ms_per_frame > f_end) {
            SDL_Delay((uint32_t)(f_start + ms_per_frame - f_end));
        }
    }
}

int Gomoku_run(int argc, char* argv[]) {
    (void) argc; // |
    (void) argv; // | unused

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (initSDL(&renderer, &window) < 0) {
        fprintf(stderr, "Failed to initialize SDL.\n SDL_Error: %s\n", SDL_GetError()); 
        return -1;
    }

    uint16_t fps = 60;
    AppState* state = createAppState(renderer, fps);
    mainloop(state);

    // free up resources after the mainloop quits
    destroyAppState(state);
    quitSDL(renderer, window);
    return 0;
}
