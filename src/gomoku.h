// Copyright (c) Dawid "hhimko" Cyganek <dawid.cyganek1@gmail.com>. Licensed under the MIT Licence.
// See the LICENCE file in the repository root for full licence text.

#ifndef GOMOKU_H_
#define GOMOKU_H_

#include <SDL.h>

#include "./board.h"


typedef enum GameState {
    GAME_STATE_PLAYERS_TURN,
    GAME_STATE_AIS_TURN
} GameState;

typedef struct GomokuGame GomokuGame;

typedef void (*gomokuGameUpdateCallback)(GomokuGame* game, uint64_t dt);
typedef SDL_bool (*gomokuGameHandleInputCallback)(GomokuGame* game, SDL_Event* event);

struct GomokuGame {
    uint16_t turn;
    GameState state;
    Board* board;
    gomokuGameUpdateCallback update;
    gomokuGameHandleInputCallback handleInput;
};

GomokuGame* createGomokuGame(Board* board);
void destroyGomokuGame(GomokuGame* game);

#endif /* GOMOKU_H_ */
