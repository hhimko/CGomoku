#ifndef BOARD_H_
#define BOARD_H_

#include <SDL.h>


/* 
 * Render a tatami mat background on screen with given zoom level and offset values from the center. 
 * Zoom should be a float value bigger than 1. Smaller values are automatically mapped to 1. 
 */
void renderTatamiBackground(SDL_Renderer* rend, double zoom, double offset_x, double offset_y);

void renderBoard(SDL_Renderer* rend);

#endif BOARD_H_
