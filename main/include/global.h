#pragma once

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GRID_ROWS 9
#define GRID_COLS 19

extern lv_obj_t *grid_cells[GRID_ROWS][GRID_COLS];
extern const lv_color_t COLOR_GRID_DEFAULT;
extern bool snake_game_just_ended;

#ifdef __cplusplus
}
#endif