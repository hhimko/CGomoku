#include "./gomoku.h"

#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>

#include "./render.h"


struct GameState{
    int x;
} gamestate;

void menuRender() {

}

void menuUpdate(int dt) {
    (void) dt;
}

typedef void (*updateCallback)(int);
typedef void (*renderCallback)(void);

void mainloop() {
    bool running = true;

    updateCallback update = menuUpdate;
    renderCallback render = menuRender;

    while (running) {
        update(100);
        render();
        running = false;
    }
}

void Gomoku_run(int argc, char* argv[]) {
    (void) argc;
    (void) argv;

    if (!initSDL()) {
        fprintf(stderr, "Failed to initialize SDL.\n SDL_Error: %s\n", SDL_GetError()); 
        exit(1);
    }

    mainloop();

    /* free up resources when the mainloop quits */
    quitSDL();
}
