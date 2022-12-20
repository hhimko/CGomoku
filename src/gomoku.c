#include "./gomoku.h"

#include <stdlib.h>


SDL_bool gomokuPlayerRegisterMove(GomokuGame* game, size_t col, size_t row) {
    Board* board = game->board;

    if (boardSetCell(board, col, row, CELL_BLACK_PIECE)) {
        boardCheckWin(board, CELL_BLACK_PIECE, board->selected_col, board->selected_row);
        return SDL_TRUE;
    }

    return SDL_FALSE;
}

SDL_bool gomokuHandleMouseMotion(GomokuGame* game, int32_t mx, int32_t my) {
    Board* board = game->board;

    size_t col, row;
    if (boardWindowCoordinateToCell(board, mx, my, &col, &row)) {
        board->selected_col = col;
        board->selected_row = row;
    }

    return SDL_FALSE;
}

SDL_bool gomokuHandleMouseDown(GomokuGame* game, int32_t mx, int32_t my, uint8_t button) {
    if (button != SDL_BUTTON_LEFT) return SDL_FALSE;
    Board* board = game->board;

    size_t col, row;
    if (boardWindowCoordinateToCell(board, mx, my, &col, &row)) {
        board->selected_col = col;
        board->selected_row = row;
        if (gomokuPlayerRegisterMove(game, col, row)) return SDL_TRUE;
    }

    return SDL_FALSE;
}

SDL_bool gomokuHandleKeyDown(GomokuGame* game, SDL_Keycode key) {
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
        if (gomokuPlayerRegisterMove(game, board->selected_col, board->selected_row)) return SDL_TRUE;
    }

    return SDL_FALSE;
}

SDL_bool gomokuHandlePlayerInput(GomokuGame* game, SDL_Event* event) {
    switch (event->type) {
        case SDL_MOUSEMOTION:
            if (gomokuHandleMouseMotion(game, event->motion.x, event->motion.y)) return SDL_TRUE;
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (gomokuHandleMouseDown(game, event->button.x, event->button.y, event->button.button)) return SDL_TRUE;
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
