#ifndef GOMOKU_H_
#define GOMOKU_H_

#include <SDL.h>

#include "./board.h"


typedef enum GameState {
    GAME_STATE_PLAYERS_TURN,
    GAME_STATE_AIS_TURN
} GameState;

typedef struct GomokuGame GomokuGame;

typedef SDL_bool (*gameHandleInputCallback)(GomokuGame* game, SDL_Event* event);

struct GomokuGame {
    uint16_t turn;
    GameState state;
    Board* board;
    gameHandleInputCallback handleInput;
};

GomokuGame* createGomokuGame(Board* board);
void destroyGomokuGame(GomokuGame* game);

#endif /* GOMOKU_H_ */
