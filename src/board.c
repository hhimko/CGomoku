#include "./board.h"

#include <math.h>
#include <stdio.h>

#include "./sdl/texture.h"
#include "./sdl/render.h"
#include "./ui.h"

#define LINE_WIDTH 2.0f
#define LINE_WIDTH_HALF LINE_WIDTH / 2.0f
#define DOT_RADIUS 4.0f
#define BORDER_WIDTH 6.0f


Board* createBoard(int x, int y, uint32_t size) {
    Board* board = malloc(sizeof(Board));

    if (board != NULL) {
        board->cell_count = BOARD_CELL_COUNT;
        board->selected_row = 0;
        board->selected_col = 0;
        board->pos_x = x;
        board->pos_y = y;
        board->size = size;
    }

    return board;
}

SDL_bool boardHandleMouseMotion(Board* board, int32_t mx, int32_t my) {
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

SDL_bool boardHandleKeyDown(Board* board, SDL_Keycode key) {
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

    return SDL_FALSE;
}

void renderBoard(RenderContext* ctx, Board* board) {
    SDL_Renderer* rend = ctx->renderer;
    int pos_x = board->pos_x;
    int pos_y = board->pos_y;
    uint32_t size = board->size;

    // render the board bounding box bg
    SDL_Rect board_rect = {pos_x, pos_y, size, size};

    SDL_Texture* tex = generateSolidTexture(rend, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
    // SDL_Texture* tex = loadTextureBMP(rend, "../assets/board.bmp");
    SDL_RenderCopy(rend, tex, NULL, &board_rect);
    SDL_DestroyTexture(tex);

    // render vertical grid lines 
    float line_gap = size / ((float)board->cell_count + 2.0f);

    FRect frect = {
        .x = line_gap - LINE_WIDTH_HALF + pos_x, 
        .y = line_gap - LINE_WIDTH_HALF + pos_y, 
        .w = LINE_WIDTH, 
        .h = (float)size - 2*line_gap + LINE_WIDTH
    };

    SDL_SetRenderDrawColor(rend, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
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
    frect.x = line_gap - BORDER_WIDTH + pos_x;
    frect.y = line_gap - BORDER_WIDTH + pos_y;
    frect.w = (float)size - 2*line_gap + 2*BORDER_WIDTH;
    frect.h = (float)size - 2*line_gap + 2*BORDER_WIDTH;    
    drawFRectBorder(rend, &frect, BORDER_WIDTH, BORDER_TYPE_OUTER);

    // render dots 
    int cx = (int)roundf(pos_x + size / 2.0f - LINE_WIDTH_HALF);
    int cy = (int)roundf(pos_y + size / 2.0f - LINE_WIDTH_HALF);
    int dot_offset = (int)roundf(line_gap * 4); // ofset from center dot

    drawFilledCircleAA(rend, cx, cy, 4);
    drawFilledCircleAA(rend, cx + dot_offset, cy + dot_offset, 4);
    drawFilledCircleAA(rend, cx - dot_offset, cy + dot_offset, 4);
    drawFilledCircleAA(rend, cx - dot_offset, cy - dot_offset, 4);
    drawFilledCircleAA(rend, cx + dot_offset, cy - dot_offset, 4);

    // render selection cursor
    frect.x = pos_x + (board->selected_col + 0.5f) * line_gap;
    frect.y = pos_y + (board->selected_row + 0.5f) * line_gap;
    frect.w = line_gap;
    frect.h = line_gap;
    
    renderSelectionCursorF(rend, &frect);
}

void destroyBoard(Board* board){
    free(board);
}
