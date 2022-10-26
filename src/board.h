#ifndef BOARD_H_
#define BOARD_H_

#include <SDL.h>

#include "./sdl/render.h"


/* 
 * Render a tatami mat background on screen with given zoom level and offset values from the center. 
 * Zoom should be a float value bigger than 1. Smaller values are automatically mapped to 1. 
 */
void renderTatamiBackground(RenderContext* ctx, double zoom, double offset_x, double offset_y);

void renderBoard(RenderContext* ctx);

#endif BOARD_H_
