#pragma once

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void start_snake_game(void);
void move_snake_cb(void);
void clear_snake_and_target(void);
void draw_snake_and_target(void);

#ifdef __cplusplus
}
#endif