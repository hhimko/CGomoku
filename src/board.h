#ifndef BOARD_H_
#define BOARD_H_

#include <SDL.h>

#include "./sdl/render.h"

#define BOARD_CELL_COUNT 14


/* Includes all data necessary for handling and rendering a gomoku board */
typedef struct Board Board;

typedef enum BoardCell {
    CELL_EMPTY,
    CELL_BLACK_PIECE,
    CELL_WHITE_PIECE
} BoardCell;

struct Board {
    size_t cell_count; // number of grid cells in one row/column
    size_t selected_row, selected_col;
    BoardCell cells[BOARD_CELL_COUNT + 1][BOARD_CELL_COUNT + 1];
};

Board* createBoard();

SDL_bool boardHandleMouseMotion(Board* board, int32_t mx, int32_t my);
SDL_bool boardHandleKeyDown(Board* board, SDL_Keycode key);

void renderBoard(RenderContext* ctx, Board* board, int x, int y, unsigned int size);

void destroyBoard(Board* board);

#endif /* BOARD_H_ */
