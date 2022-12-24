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
    int pos_x, pos_y;
    uint32_t size;
    SDL_Texture* board_tex;
    SDL_Texture* black_piece_tex;
    SDL_Texture* white_piece_tex;
};

Board* createBoard(int x, int y, uint32_t size, SDL_Texture* board_tex, SDL_Texture* black_piece_tex, SDL_Texture* white_piece_tex);

int boardWindowCoordinateToCell(Board* board, int32_t x, int32_t y, size_t* col, size_t* row);
int boardSetCell(Board* board, size_t x, size_t y, BoardCell cell);
int boardCheckWin(Board* board, BoardCell piece, size_t x, size_t y);
void renderBoard(RenderContext* ctx, Board* board);

void destroyBoard(Board* board);

#endif /* BOARD_H_ */
