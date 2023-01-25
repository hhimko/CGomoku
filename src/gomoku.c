// Copyright (c) Dawid "hhimko" Cyganek <dawid.cyganek1@gmail.com>. Licensed under the MIT Licence.
// See the LICENCE file in the repository root for full licence text.

#include "./gomoku.h"

#include <stdlib.h>
#include <assert.h>


void ggSetGameState(GomokuGame* game, GameState new_state);

int __emptyCellIndexToBoardPosition(Board* board, size_t index, size_t* col, size_t* row) {
    for (size_t j=0; j <= board->cell_count; ++j) {
        for (size_t i=0; i <= board->cell_count; ++i) {
            if (board->cells[j][i] == CELL_EMPTY) {
                if (index == 0) {
                    *col = i;
                    *row = j;
                    return 1;
                }

                index--;
            }
        }
    }

    return 0;
}

void ggAIMakeMove(GomokuGame* game) {
    Board* board = game->board;

    size_t empty_spaces = 0;
    for (size_t j=0; j <= board->cell_count; ++j) {
        for (size_t i=0; i <= board->cell_count; ++i) {
            if (board->cells[j][i] == CELL_EMPTY) empty_spaces++;
        }
    }

    size_t x, y;
    size_t choice = (size_t)rand() % empty_spaces;
    __emptyCellIndexToBoardPosition(board, choice, &x, &y);
    
    boardSetCell(board, x, y, CELL_WHITE_PIECE);
    if (boardCheckWin(board, CELL_WHITE_PIECE, x, y)){

    }
    else {
        game->turn++;
        ggSetGameState(game, GAME_STATE_PLAYERS_TURN);
    }
}

SDL_bool ggPlayerRegisterMove(GomokuGame* game, size_t col, size_t row) {
    Board* board = game->board;

    if (boardSetCell(board, col, row, CELL_BLACK_PIECE)) {
        if (boardCheckWin(board, CELL_BLACK_PIECE, board->selected_col, board->selected_row)){

        }
        else {
            game->turn++;
            ggSetGameState(game, GAME_STATE_AIS_TURN);
        }

        return SDL_TRUE;
    }

    return SDL_FALSE;
}

SDL_bool ggHandleMouseMotion(GomokuGame* game, int32_t mx, int32_t my) {
    Board* board = game->board;

    size_t col, row;
    if (boardWindowCoordinateToCell(board, mx, my, &col, &row)) {
        board->selected_col = col;
        board->selected_row = row;
    }

    return SDL_FALSE;
}

SDL_bool ggHandleMouseDown(GomokuGame* game, int32_t mx, int32_t my, uint8_t button) {
    if (button != SDL_BUTTON_LEFT) return SDL_FALSE;
    Board* board = game->board;

    size_t col, row;
    if (boardWindowCoordinateToCell(board, mx, my, &col, &row)) {
        board->selected_col = col;
        board->selected_row = row;
        if (ggPlayerRegisterMove(game, col, row)) return SDL_TRUE;
    }

    return SDL_FALSE;
}

SDL_bool ggHandleKeyDown(GomokuGame* game, SDL_Keycode key) {
    Board* board = game->board;

    if (key == SDLK_LEFT) {
        board->selected_col = board->selected_col == 0 ? board->cell_count : board->selected_col - 1;
        return SDL_TRUE;
    } 
    if (key == SDLK_RIGHT)
    {
        board->selected_col = board->selected_col == board->cell_count ? 0 : board->selected_col + 1;
        return SDL_TRUE;
    }
    if (key == SDLK_UP)
    {
        board->selected_row = board->selected_row == 0 ? board->cell_count : board->selected_row - 1;
        return SDL_TRUE;
    }
    if (key == SDLK_DOWN)
    {
        board->selected_row = board->selected_row == board->cell_count ? 0 : board->selected_row + 1;
        return SDL_TRUE;
    }
    if (key == SDLK_RETURN) { // enter key
        if (ggPlayerRegisterMove(game, board->selected_col, board->selected_row)) return SDL_TRUE;
    }

    return SDL_FALSE;
}

SDL_bool ggHandlePlayerInput_playing(GomokuGame* game, SDL_Event* event) {
    switch (event->type) {
        case SDL_MOUSEMOTION:
            if (ggHandleMouseMotion(game, event->motion.x, event->motion.y)) return SDL_TRUE;
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (ggHandleMouseDown(game, event->button.x, event->button.y, event->button.button)) return SDL_TRUE;
            break;

        case SDL_KEYDOWN:
            if (ggHandleKeyDown(game, event->key.keysym.sym)) return SDL_TRUE;
            break;
    }

    return SDL_FALSE;
}

SDL_bool ggHandlePlayerInput_waiting(GomokuGame* game, SDL_Event* event) {
    switch (event->type) {
        case SDL_MOUSEMOTION:
            if (ggHandleMouseMotion(game, event->motion.x, event->motion.y)) return SDL_TRUE;
            break;
    }

    return SDL_FALSE;
}

void ggUpdate_playersTurn(GomokuGame* game, uint64_t dt) {
    (void) game, dt;
}

void ggUpdate_AIsTurn(GomokuGame* game, uint64_t dt) {
    (void) dt;
    ggAIMakeMove(game);
}

void ggSetGameState(GomokuGame* game, GameState new_state) {
    switch (new_state) {
        case GAME_STATE_PLAYERS_TURN:
            game->update = ggUpdate_playersTurn;
            game->handleInput = ggHandlePlayerInput_playing;
            break;
        
        case GAME_STATE_AIS_TURN:
            game->update = ggUpdate_AIsTurn;
            game->handleInput = ggHandlePlayerInput_waiting;
            break;

        default:
            assert(0 && "unhandled GameState value in ggSetGameState");
            return;
    }
}

GomokuGame* createGomokuGame(Board* board) {
    GomokuGame* game = (GomokuGame*)malloc(sizeof(GomokuGame));

    if (game != NULL) {
        game->turn = 1;
        game->state = GAME_STATE_PLAYERS_TURN;
        game->board = board;
        ggSetGameState(game, GAME_STATE_PLAYERS_TURN); // initializes update and handleInput callbacks 
    }

    return game;
}

void destroyGomokuGame(GomokuGame* game) {
    destroyBoard(game->board);
    free(game);
}
