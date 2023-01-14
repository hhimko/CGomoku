#include "./board.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "./sdl/texture.h"
#include "./sdl/render.h"
#include "./ui.h"

#define BOARD_SHADOW_RADIUS 32
#define BOARD_SHADOW_STRENGHT 160
#define BOARD_SHADOW_OFFSET_X -16
#define BOARD_SHADOW_OFFSET_Y 16

#define PIECE_SHADOW_RADIUS 2
#define PIECE_SHADOW_STRENGHT 120
#define PIECE_SHADOW_OFFSET_X 0
#define PIECE_SHADOW_OFFSET_Y 1

#define LINE_WIDTH 1.0f
#define LINE_WIDTH_HALF LINE_WIDTH / 2.0f
#define DOT_RADIUS 5.0f
#define BORDER_WIDTH 4.0f
#define BORDER_OFFSET 10.0f
#define LINE_RGB 0x29, 0x1B, 0x12

#define MAX(x, y) ((x) > (y) ? (x) : (y))


void initializeBoardCellArray(Board* board) {
    for (size_t y = 0; y <= board->cell_count; ++y) {
        for (size_t x = 0; x <= board->cell_count; ++x) {
            board->cells[y][x] = CELL_EMPTY;
        }
    }
}

Board* createBoard(RenderContext* ctx, int x, int y, uint32_t size) {
    Board* board = (Board*)malloc(sizeof(Board));

    if (board != NULL) {
        board->cell_count = BOARD_CELL_COUNT;
        board->selected_row = 0;
        board->selected_col = 0;
        
        board->pos_x = x;
        board->pos_y = y;
        board->size = size;

        board->board_tex = loadTextureBMP(ctx->renderer, "../assets/board.bmp");
        board->board_shadow_tex = generateShadowFromTexture(
            ctx->renderer, board->board_tex, BOARD_SHADOW_RADIUS, BOARD_SHADOW_STRENGHT
        );

        board->black_piece_tex = loadTextureBMP(ctx->renderer, "../assets/black_piece.bmp");
        board->black_piece_shadow_tex = generateShadowFromTexture(
            ctx->renderer, board->black_piece_tex, PIECE_SHADOW_RADIUS, PIECE_SHADOW_STRENGHT
        );

        board->white_piece_tex = loadTextureBMP(ctx->renderer, "../assets/white_piece.bmp");
        board->white_piece_shadow_tex = generateShadowFromTexture(
            ctx->renderer, board->white_piece_tex, PIECE_SHADOW_RADIUS, PIECE_SHADOW_STRENGHT
        );

        initializeBoardCellArray(board);
    }

    return board;
}

int boardWindowCoordinateToCell(Board* board, int32_t x, int32_t y, size_t* col, size_t* row) {
    int line_gap = (int)roundf(board->size / ((float)board->cell_count + 2.0f));

    int c = (x - board->pos_x - line_gap/2) / line_gap;
    if (c < 0 || c > board->cell_count) return 0;

    int r = (y - board->pos_y - line_gap/2) / line_gap;
    if (r < 0 || r > board->cell_count) return 0;

    *col = (size_t)c;
    *row = (size_t)r;
    return 1;
}

int boardSetCell(Board* board, size_t x, size_t y, BoardCell cell) {
    if (x > board->cell_count || y > board->cell_count || cell == CELL_EMPTY)
        return 0;

    if (board->cells[y][x] != CELL_EMPTY)
        return 0;

    board->cells[y][x] = cell;
    return 1;
}

size_t boardCountConnectivity(Board* board, BoardCell piece, size_t x, size_t y) {
    if (x > board->cell_count || y > board->cell_count || board->cells[y][x] != piece)
        return 0;

    size_t acc = 1;
    size_t _max = 0;

    // horizontal
    int xi = (int)x;
    while (--xi >= 0 && board->cells[y][xi] == piece) acc++;
    xi = (int)x;
    while (++xi <= board->cell_count && board->cells[y][xi] == piece) acc++;
    _max = acc;

    // vertical
    acc = 1;
    int yi = (int)y;
    while (--yi >= 0 && board->cells[yi][x] == piece) acc++;
    yi = (int)y;
    while (++yi <= board->cell_count && board->cells[yi][x] == piece) acc++;
    _max = MAX(_max, acc);

    // rising diagonal
    acc = 1;
    xi = (int)x;
    yi = (int)y;
    while (--xi >= 0 && --yi >= 0 && board->cells[yi][xi] == piece) acc++;
    xi = (int)x;
    yi = (int)y;
    while (++xi <= board->cell_count && ++yi <= board->cell_count && board->cells[yi][xi] == piece) acc++;
    _max = MAX(_max, acc);

    // falling diagonal
    acc = 1;
    xi = (int)x;
    yi = (int)y;
    while (--xi >= 0 && ++yi <= board->cell_count && board->cells[yi][xi] == piece) acc++;
    xi = (int)x;
    yi = (int)y;
    while (++xi <= board->cell_count && --yi >= 0 && board->cells[yi][xi] == piece) acc++;

    return MAX(_max, acc);
}

int boardCheckWin(Board* board, BoardCell piece, size_t x, size_t y) {
    size_t conn = boardCountConnectivity(board, piece, x, y);
    return conn >= 5;
}

void renderBoard(RenderContext* ctx, Board* board) {
    SDL_Renderer* rend = ctx->renderer;
    int pos_x = board->pos_x;
    int pos_y = board->pos_y;
    uint32_t size = board->size;

    // render the board shadow
    SDL_Rect shadow_rect = {
        pos_x - BOARD_SHADOW_RADIUS + BOARD_SHADOW_OFFSET_X, 
        pos_y - BOARD_SHADOW_RADIUS + BOARD_SHADOW_OFFSET_Y, 
        size + 2*BOARD_SHADOW_RADIUS, 
        size + 2*BOARD_SHADOW_RADIUS
    };
    SDL_RenderCopy(rend, board->board_shadow_tex, NULL, &shadow_rect);

    // render the board bounding box bg
    SDL_Rect board_rect = {pos_x, pos_y, size, size};
    SDL_RenderCopy(rend, board->board_tex, NULL, &board_rect);

    // render vertical grid lines 
    uint8_t r,g,b,a;
    SDL_GetRenderDrawColor(rend, &r, &g, &b, &a);
    SDL_SetRenderDrawColor(rend, LINE_RGB, SDL_ALPHA_OPAQUE);
    float line_gap = size / ((float)board->cell_count + 2.0f);

    FRect frect = {
        .x = line_gap - LINE_WIDTH_HALF + pos_x, 
        .y = line_gap - LINE_WIDTH_HALF + pos_y, 
        .w = LINE_WIDTH, 
        .h = (float)size - 2*line_gap + LINE_WIDTH
    };

    for (size_t vline = 0; vline < board->cell_count + 1; ++vline)
    {
        drawFilledFRect(rend, &frect);
        frect.x += line_gap;
    }

    // render horizontal grid lines 
    frect.x = line_gap - LINE_WIDTH_HALF + pos_x;
    frect.w = frect.h;
    frect.h = LINE_WIDTH;
    for (size_t hline = 0; hline < board->cell_count + 1; ++hline)
    {
        drawFilledFRect(rend, &frect);
        frect.y += line_gap;
    }

    // render border lines
    frect.x = line_gap - BORDER_OFFSET + pos_x;
    frect.y = line_gap - BORDER_OFFSET + pos_y;
    frect.w = (float)size - 2*line_gap + 2*BORDER_OFFSET;
    frect.h = (float)size - 2*line_gap + 2*BORDER_OFFSET;    
    drawFRectBorder(rend, &frect, BORDER_WIDTH, BORDER_TYPE_OUTER);

    // render dots 
    int cx = (int)roundf(pos_x + size / 2.0f - LINE_WIDTH_HALF - 1);
    int cy = (int)roundf(pos_y + size / 2.0f - LINE_WIDTH_HALF - 1);
    int dot_offset = (int)roundf(line_gap * 4); // ofset from center dot

    drawFilledCircleAA(rend, cx, cy, DOT_RADIUS);
    drawFilledCircleAA(rend, cx + dot_offset, cy + dot_offset, DOT_RADIUS);
    drawFilledCircleAA(rend, cx - dot_offset, cy + dot_offset, DOT_RADIUS);
    drawFilledCircleAA(rend, cx - dot_offset, cy - dot_offset, DOT_RADIUS);
    drawFilledCircleAA(rend, cx + dot_offset, cy - dot_offset, DOT_RADIUS);

    // render board pieces
    SDL_Rect piece_rect = { .w = (int)(line_gap), .h = (int)(line_gap) };
    SDL_Rect piece_shadow_rect = { 
        .w = piece_rect.w + 2*PIECE_SHADOW_RADIUS, 
        .h = piece_rect.h + 2*PIECE_SHADOW_RADIUS 
    };

    for (size_t y = 0; y <= board->cell_count; ++y) {
        for (size_t x = 0; x <= board->cell_count; ++x) {
            BoardCell c = board->cells[y][x];
            if (c == CELL_BLACK_PIECE) 
            {
                piece_rect.x = pos_x + (int)(line_gap*(x + 0.5f));
                piece_rect.y = pos_y + (int)(line_gap*(y + 0.5f));

                piece_shadow_rect.x = piece_rect.x - PIECE_SHADOW_RADIUS + PIECE_SHADOW_OFFSET_X;
                piece_shadow_rect.y = piece_rect.y - PIECE_SHADOW_RADIUS + PIECE_SHADOW_OFFSET_Y;
                
                SDL_RenderCopy(rend, board->black_piece_shadow_tex, NULL, &piece_shadow_rect);
                SDL_RenderCopy(rend, board->black_piece_tex, NULL, &piece_rect);
            } 
            else if (c == CELL_WHITE_PIECE) 
            {
                piece_rect.x = pos_x + (int)(line_gap*(x + 0.5f));
                piece_rect.y = pos_y + (int)(line_gap*(y + 0.5f));

                piece_shadow_rect.x = piece_rect.x - PIECE_SHADOW_RADIUS + PIECE_SHADOW_OFFSET_X;
                piece_shadow_rect.y = piece_rect.y - PIECE_SHADOW_RADIUS + PIECE_SHADOW_OFFSET_Y;
                
                SDL_RenderCopy(rend, board->white_piece_shadow_tex, NULL, &piece_shadow_rect);
                SDL_RenderCopy(rend, board->white_piece_tex, NULL, &piece_rect);
            }
        }
    }

    // render selection cursor
    frect.x = pos_x + (board->selected_col + 0.5f) * line_gap;
    frect.y = pos_y + (board->selected_row + 0.5f) * line_gap;
    frect.w = line_gap;
    frect.h = line_gap;
    renderSelectionCursorF(rend, &frect);

    SDL_SetRenderDrawColor(rend, r, g, b, a); // restore original color
}

void destroyBoard(Board* board){
    SDL_DestroyTexture(board->board_tex);
    SDL_DestroyTexture(board->black_piece_tex);
    SDL_DestroyTexture(board->white_piece_tex);
    free(board);
}
