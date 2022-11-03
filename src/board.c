#include "./board.h"

#include "./sdl/texture.h"
#include "./sdl/render.h"

#define LINE_WIDTH 2.0f
#define LINE_WIDTH_HALF LINE_WIDTH / 2.0f
#define BORDER_WIDTH 4.0f

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))


Board* createBoard() {
    Board* board = malloc(sizeof(Board));

    if (board != NULL) {
        board->cell_count = BOARD_CELL_COUNT;
    }

    return board;
}

void renderBoard(RenderContext* ctx, Board* board, int pos_x, int pos_y, unsigned int size) {
    (void) board;
    SDL_Renderer* rend = ctx->renderer;

    // render the board bounding box bg
    SDL_Rect board_rect = {pos_x, pos_y, size, size};

    SDL_SetRenderDrawColor(rend, 161, 132, 94, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(rend, &board_rect);

    // render vertical grid lines 
    float line_gap = size / ((float)board->cell_count + 2.0f);

    SDL_FRect line_frect = {
        .x = line_gap - LINE_WIDTH_HALF + pos_x, 
        .y = line_gap - LINE_WIDTH_HALF + pos_y, 
        .w = LINE_WIDTH, 
        .h = (float)size - 2*line_gap + LINE_WIDTH
    };

    SDL_SetRenderDrawColor(rend, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
    for (size_t vline = 0; vline < board->cell_count + 1; ++vline)
    {
        SDL_RenderFillRectF(rend, &line_frect);
        line_frect.x += line_gap;
    }

    // render horizontal grid lines 
    line_frect.x = line_gap - LINE_WIDTH_HALF + pos_x;
    line_frect.w = line_frect.h;
    line_frect.h = LINE_WIDTH;
    for (size_t hline = 0; hline < board->cell_count + 1; ++hline)
    {
        SDL_RenderFillRectF(rend, &line_frect);
        line_frect.y += line_gap;
    }

    drawCircleAA(rend, pos_x + size/2, pos_y + size/2, 100.0);
}

void destroyBoard(Board* board){
    free(board);
}

void renderTatamiBackground(RenderContext* ctx, double zoom, double offset_x, double offset_y) {
    zoom = MAX(zoom, 1); // zoom value must be greater than 1
    offset_x = MAX(MIN(offset_x, 1), -1); // |
    offset_y = MAX(MIN(offset_y, 1), -1); // | offset values must be normalized to [-1, 1] range

    SDL_Renderer* rend = ctx->renderer;
    int win_w = ctx->win_w; 
    int win_h = ctx->win_h;

    SDL_Texture* tatami = loadTextureBMP(rend, "../assets/tatami.bmp");
    SDL_Texture* tatami_half = loadTextureBMP(rend, "../assets/tatami_half.bmp");

    int tex_w, tex_h;
    // logic assumes both tatami textures are the same size 
    if (getTextureSize(tatami, &tex_w, &tex_h) < 0) {
        tex_w = 800; // |
        tex_h = 800; // | quietly set an arbitrarily size manually
    }

    // bg is scaled in order to fill the entire screen
    double scale_ratio = MAX(win_w * zoom / tex_w, win_h * zoom / tex_h);  
    int bg_size_w = (int)(tex_w * scale_ratio);
    int bg_size_h = (int)(tex_h * scale_ratio);
    tex_w = (int)(bg_size_w / 3.);
    tex_h = (int)(bg_size_h / 3.);

    // calculate the final offsets with centering
    int ox = (win_w - bg_size_w) / 2;
    int oy = (win_h - bg_size_h) / 2;
    ox += (int)(ox * offset_x);
    oy += (int)(oy * offset_y);

    SDL_Rect dstrect = { .w = tex_w, .h = tex_h };
    /*  drawn pattern reference:
     *      +------+------+------+
     *      | half   half | half |
     *      |  TL     TC  |  TR  |
     *      +------+------+      +
     *      | half | full | half |
     *      |  CL  |  CC  |  CR  |
     *      +      +------+------+
     *      | half | half   half |
     *      |  BL  |  BC     BR  |
     *      +------+------+------+
     */  

    // top-left
    dstrect.x = ox;
    dstrect.y = oy; 
    SDL_RenderCopy(rend, tatami_half, NULL, &dstrect);

    // top-center
    dstrect.x += tex_w;
    SDL_RenderCopyEx(rend, tatami_half, NULL, &dstrect, 0., NULL, SDL_FLIP_HORIZONTAL);

    // top-right
    dstrect.x += tex_w;
    SDL_RenderCopyEx(rend, tatami_half, NULL, &dstrect, 90., NULL, SDL_FLIP_NONE);

    // center-left
    dstrect.y += tex_h;
    dstrect.x = ox;
    SDL_RenderCopyEx(rend, tatami_half, NULL, &dstrect, 90., NULL, SDL_FLIP_NONE);

    // center-center
    dstrect.x += tex_w;
    SDL_RenderCopy(rend, tatami, NULL, &dstrect);

    // center-right
    dstrect.x += tex_w;
    SDL_RenderCopyEx(rend, tatami_half, NULL, &dstrect, -90., NULL, SDL_FLIP_NONE);

    // bottom-left
    dstrect.y += tex_h;
    dstrect.x = ox;
    SDL_RenderCopyEx(rend, tatami_half, NULL, &dstrect, -90., NULL, SDL_FLIP_NONE);

    // bottom-center
    dstrect.x += tex_w;
    SDL_RenderCopy(rend, tatami_half, NULL, &dstrect); 

    // bottom-right
    dstrect.x += tex_w;
    SDL_RenderCopyEx(rend, tatami_half, NULL, &dstrect, 0., NULL, SDL_FLIP_HORIZONTAL);

    SDL_DestroyTexture(tatami);
    SDL_DestroyTexture(tatami_half);
}

