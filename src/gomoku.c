#include "./gomoku.h"

#include <stdlib.h>


SDL_bool gomokuHandleMouseMotion(GomokuGame* game, int32_t mx, int32_t my) {
    Board* board = game->board;

    float line_gap = board->size / ((float)board->cell_count + 2.0f);
    int x = (mx - board->pos_x - (int)line_gap/2) / (int)line_gap;
    if (0 <= x && x <= BOARD_CELL_COUNT) {
        int y = (my - board->pos_y - (int)line_gap/2) / (int)line_gap;
        if (0 <= y && y <= BOARD_CELL_COUNT) {
            board->selected_col = x;
            board->selected_row = y;
        }
    }
    return SDL_FALSE;
}

SDL_bool gomokuHandleKeyDown(GomokuGame* game, SDL_Keycode key) {
    Board* board = game->board;

    if (key == SDLK_LEFT) {
        board->selected_col = board->selected_col == 0 ? BOARD_CELL_COUNT : board->selected_col - 1;
        return SDL_TRUE;
    } 
    if (key == SDLK_RIGHT)
    {
        board->selected_col = board->selected_col == BOARD_CELL_COUNT ? 0 : board->selected_col + 1;
        return SDL_TRUE;
    }
    if (key == SDLK_UP)
    {
        board->selected_row = board->selected_row == 0 ? BOARD_CELL_COUNT : board->selected_row - 1;
        return SDL_TRUE;
    }
    if (key == SDLK_DOWN)
    {
        board->selected_row = board->selected_row == BOARD_CELL_COUNT ? 0 : board->selected_row + 1;
        return SDL_TRUE;
    }
    if (key == SDLK_RETURN) { // enter key
        if (boardSetCell(board, board->selected_col, board->selected_row, CELL_BLACK_PIECE)) {
            boardCheckWin(board, CELL_BLACK_PIECE, board->selected_col, board->selected_row);
            return SDL_TRUE;
        }
    }

    return SDL_FALSE;
}

SDL_bool gomokuHandlePlayerInput(GomokuGame* game, SDL_Event* event) {
    switch (event->type) {
        case SDL_MOUSEMOTION:
            if (gomokuHandleMouseMotion(game, event->motion.x, event->motion.y)) return SDL_TRUE;
            break;

        case SDL_KEYDOWN:
            if (gomokuHandleKeyDown(game, event->key.keysym.sym)) return SDL_TRUE;
            break;
    }

    return SDL_FALSE;
}

GomokuGame* createGomokuGame(Board* board) {
    GomokuGame* game = (GomokuGame*)malloc(sizeof(GomokuGame));

    if (game != NULL) {
        game->turn = 1;
        game->state = GAME_STATE_PLAYERS_TURN;
        game->board = board;
        game->handleInput = gomokuHandlePlayerInput;
    }

    return game;
}

void destroyGomokuGame(GomokuGame* game) {
    destroyBoard(game->board);
    free(game);
}
