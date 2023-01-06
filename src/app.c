#include "./app.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <SDL.h>

#include "./scenes/scene.h"
#include "./animation.h"
#include "./ui.h"

#define DEFAULT_FPS 60
#define DEFAULT_SCREEN_WIDTH 1920 - 600
#define DEFAULT_SCREEN_HEIGHT 1080 - 200

static SDL_bool s_running = SDL_FALSE;


AppState* createAppState(RenderContext* ctx, uint16_t fps_cap) {
    AppState* state = malloc(sizeof(AppState));
    
    if (state != NULL) {
        state->context = ctx;
        state->fps_cap = fps_cap;
        state->scene.destroy = NULL; // scene destroy callback has to be null-initialized before setScene
        if (setScene(state, SCENE_MENU) < 0) {  // initialize scene to main menu
            destroyAppState(state);
            return NULL;
        }
    }

    return state;
}

void destroyAppState(AppState* state) {
    state->scene.destroy();
    free(state);
}

int handleEvents(AppState* state) {
    static SDL_Event e;

    while(SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) return -1; // signal app quit
        
        if (state->scene.handle_input(&e, state) == SDL_TRUE) continue;
    }

    return 0;
}

void mainloop(AppState* state, SDL_Window* win) {
    s_running = SDL_TRUE;
    RenderContext* context = state->context; 
    SDL_Renderer* renderer = context->renderer; 

    const uint64_t ms_per_frame = (uint64_t)round(1000.0 / (double)state->fps_cap);
    char fpsbuffer[9];

    uint64_t f_start = SDL_GetTicks() - ms_per_frame;
    while (s_running)
    {
        uint64_t f_current = SDL_GetTicks();
        uint64_t f_elapsed = f_current - f_start;
        f_start = f_current;

        const uint64_t fps = (uint64_t)round(1000.0 / (double)f_elapsed);
        snprintf(fpsbuffer, 9, "fps: %llu\n", fps);
        SDL_SetWindowTitle(win, fpsbuffer);
        
        // handle user input
        if (handleEvents(state) < 0)
            Gomoku_shutdown();

        // update logic state
        updateUI(f_elapsed); // TODO: remove updateUI
        updateAnimations(f_elapsed);
        state->scene.update(f_elapsed);

        // render game to screen
        SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        state->scene.render(context);
        SDL_RenderPresent(renderer);

        // wait the remaining frame time to minimalize CPU load
        uint64_t f_end = SDL_GetTicks();
        if (f_start + ms_per_frame > f_end) {
            SDL_Delay((uint32_t)(f_start + ms_per_frame - f_end));
        }
    }
}

int Gomoku_run(int argc, char* argv[]) {
    (void) argc; // |
    (void) argv; // | unused
    srand((unsigned)time(NULL));
    uint16_t fps = DEFAULT_FPS;

    SDL_Window* window = NULL;
    RenderContext* context = createRenderContext(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);

    if (initSDL(context, &window) < 0) {
        fprintf(stderr, "Failed to initialize SDL.\n SDL_Error: %s\n", SDL_GetError()); 
        quitSDL(context, window);
        destroyRenderContext(context);
        return -1;
    }
    
    if (initializeAnimationModule() < 0) {
        fprintf(stderr, "Failed to initialize Animation Module.\n"); 
        destroyAnimationModule();
        quitSDL(context, window);
        destroyRenderContext(context);
        return -1;
    }

    AppState* state = createAppState(context, fps);
    if (state == NULL) {
        fprintf(stderr, "Failed to create AppState.\n"); 
        destroyAnimationModule();
        quitSDL(context, window);
        destroyRenderContext(context);
        return -1;
    }

    //
    // all actual game logic and rendering happens in mainloop
    mainloop(state, window);
    //
    //

    // free up resources after the mainloop quits
    destroyAppState(state);
    destroyAnimationModule();
    destroyUI();
    quitSDL(context, window);
    destroyRenderContext(context);
    return 0;
}

void Gomoku_shutdown() {
    s_running = SDL_FALSE;
}
