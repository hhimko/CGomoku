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
    size_t cell_count; // number of cells in one row/column
    BoardCell cells[BOARD_CELL_COUNT + 1][BOARD_CELL_COUNT + 1];
};

Board* createBoard();

void destroyBoard(Board* board);

void renderBoard(RenderContext* ctx, Board* board, int x, int y, unsigned int size);

/* 
 * Render a tatami mat background on screen with given zoom level and offset values from the center. 
 * Zoom should be a float value bigger than 1. Smaller values are automatically mapped to 1. 
 */
void renderTatamiBackground(RenderContext* ctx, double zoom, double offset_x, double offset_y);

#endif /* BOARD_H_ */
