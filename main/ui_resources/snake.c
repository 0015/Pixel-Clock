#include "snake.h"
#include "esp_random.h"
#include "lvgl.h"
#include "global.h"

#define SNAKE_MAX_LENGTH 16

typedef struct
{
    int row, col;
} GridPos;

bool snake_game_just_ended = false;
static GridPos snake[SNAKE_MAX_LENGTH];
static int snake_length = 3;
static GridPos target;
static bool snake_active = false;
static bool first_move_done = false;
static GridPos initial_direction;

static lv_color_t prev_colors[GRID_ROWS][GRID_COLS];
static bool prev_colors_valid[GRID_ROWS][GRID_COLS];

static lv_color_t get_safe_restore_color(int row, int col)
{
    return prev_colors_valid[row][col] ? prev_colors[row][col] : COLOR_GRID_DEFAULT;
}

void clear_snake_and_target(void)
{
    for (int i = 0; i < snake_length; ++i)
    {
        lv_obj_set_style_bg_color(
            grid_cells[snake[i].row][snake[i].col],
            get_safe_restore_color(snake[i].row, snake[i].col), 0);
    }

    lv_obj_set_style_bg_color(
        grid_cells[target.row][target.col],
        get_safe_restore_color(target.row, target.col), 0);

    for (int i = 0; i < GRID_ROWS; ++i)
        for (int j = 0; j < GRID_COLS; ++j)
            prev_colors_valid[i][j] = false;
}

void draw_snake_and_target(void)
{
    for (int i = 0; i < snake_length; ++i)
    {
        GridPos pos = snake[i];
        if (!prev_colors_valid[pos.row][pos.col])
        {
            prev_colors[pos.row][pos.col] = lv_obj_get_style_bg_color(grid_cells[pos.row][pos.col], LV_PART_MAIN);
            prev_colors_valid[pos.row][pos.col] = true;
        }
        lv_obj_set_style_bg_color(grid_cells[pos.row][pos.col], lv_color_make(100, 255, 100), 0);
        lv_obj_move_foreground(grid_cells[pos.row][pos.col]);
    }

    if (!prev_colors_valid[target.row][target.col])
    {
        prev_colors[target.row][target.col] = lv_obj_get_style_bg_color(grid_cells[target.row][target.col], LV_PART_MAIN);
        prev_colors_valid[target.row][target.col] = true;
    }

    lv_obj_set_style_bg_color(grid_cells[target.row][target.col], lv_color_make(255, 50, 50), 0);
    lv_obj_move_foreground(grid_cells[target.row][target.col]);
}

static int sign(int x)
{
    return (x > 0) - (x < 0);
}

static bool will_collide(GridPos next)
{
    for (int i = 0; i < snake_length; ++i)
    {
        if (snake[i].row == next.row && snake[i].col == next.col)
            return true;
    }
    return false;
}

void move_snake_cb(void)
{
    if (!snake_active)
        return;

    GridPos head = snake[0];
    int d_row = 0, d_col = 0;

    if (!first_move_done)
    {
        d_row = initial_direction.row;
        d_col = initial_direction.col;
        first_move_done = true;
    }
    else
    {
        if (target.col != head.col)
        {
            d_col = sign(target.col - head.col);
            d_row = 0;
        }
        else if (target.row != head.row)
        {
            d_row = sign(target.row - head.row);
            d_col = 0;
        }

        GridPos candidate = {head.row + d_row, head.col + d_col};
        if (will_collide(candidate))
        {
            if (d_row == 0)
            {
                d_row = (target.row > head.row) ? 1 : -1;
                d_col = 0;
            }
            else
            {
                d_col = (target.col > head.col) ? 1 : -1;
                d_row = 0;
            }

            candidate.row = head.row + d_row;
            candidate.col = head.col + d_col;

            if (will_collide(candidate))
                return;
        }
    }

    GridPos new_head = {head.row + d_row, head.col + d_col};

    GridPos tail = snake[snake_length - 1];
    lv_obj_set_style_bg_color(grid_cells[tail.row][tail.col], get_safe_restore_color(tail.row, tail.col), 0);

    for (int i = snake_length - 1; i > 0; --i)
        snake[i] = snake[i - 1];
    snake[0] = new_head;

    draw_snake_and_target();

    if (new_head.row == target.row && new_head.col == target.col)
    {
        for (int i = 0; i < snake_length; ++i)
        {
            GridPos p = snake[i];
            lv_obj_set_style_bg_color(grid_cells[p.row][p.col], get_safe_restore_color(p.row, p.col), 0);
        }
        lv_obj_set_style_bg_color(grid_cells[target.row][target.col], get_safe_restore_color(target.row, target.col), 0);
        snake_active = false;
        snake_length = 0;
        snake_game_just_ended = true;
    }
}

void start_snake_game(void)
{
    if (snake_active)
        return;

    clear_snake_and_target();
    snake_length = 3;
    first_move_done = false;

    int head_row = esp_random() % GRID_ROWS;
    int head_col = esp_random() % (GRID_COLS - snake_length - 1);

    do
    {
        target.row = esp_random() % GRID_ROWS;
        target.col = esp_random() % GRID_COLS;

        bool in_time_area =
            target.row >= 2 && target.row <= 6 &&
            target.col >= 2 && target.col <= 16;

        if (in_time_area)
            continue;

        bool overlap = false;
        if (target.row == head_row && target.col >= head_col && target.col <= head_col + 2)
            overlap = true;

        if (!overlap)
            break;

    } while (true);

    GridPos head = {head_row, head_col};

    if (target.col > head.col)
    {
        initial_direction.row = 0;
        initial_direction.col = 1;
        for (int i = 0; i < snake_length; ++i)
        {
            snake[i].row = head_row;
            snake[i].col = head_col + (snake_length - 1 - i);
        }
    }
    else
    {
        initial_direction.row = 0;
        initial_direction.col = -1;
        for (int i = 0; i < snake_length; ++i)
        {
            snake[i].row = head_row;
            snake[i].col = head_col + i;
        }
    }

    draw_snake_and_target();
    snake_active = true;
}