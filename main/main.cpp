#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_memory_utils.h"
#include "esp_random.h"
#include "lvgl.h"
#include "bsp/esp-bsp.h"
#include "bsp/display.h"
#include "digit_font.h"
#include "global.h"
#include "snake.h"

extern "C"
{
#include <esp_sntp.h>
#include <sys/time.h>
}
#include <time.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 800

#define CELL_WIDTH (SCREEN_WIDTH / GRID_COLS)
#define CELL_HEIGHT (SCREEN_HEIGHT / GRID_ROWS)
#define BLOCK_W (CELL_WIDTH - 4)
#define BLOCK_H (CELL_HEIGHT - 4)

static int last_hour = -1;
static int last_min = -1;
static bool colon_visible = true;

typedef struct
{
    int start_row;
    int start_col;
} ClockLayout;

ClockLayout get_clock_layout()
{
    ClockLayout layout;
    layout.start_row = (GRID_ROWS - 5) / 2;
    int total_width = 3 * 2 + 1 + 3 * 2 + 4;
    layout.start_col = (GRID_COLS - total_width) / 2;
    return layout;
}

lv_color_t random_color()
{
    uint8_t r = esp_random() % 256;
    uint8_t g = esp_random() % 256;
    uint8_t b = esp_random() % 256;
    return lv_color_make(r, g, b);
}

static void anim_delete_tile_cb(lv_anim_t *a)
{
    lv_obj_del((lv_obj_t *)a->var);
}

static void spawn_falling_tile(lv_obj_t *parent, lv_coord_t x, lv_coord_t y)
{
    lv_obj_t *tile = lv_obj_create(parent);
    lv_obj_set_size(tile, BLOCK_W, BLOCK_H);
    lv_obj_set_style_radius(tile, 2, 0);
    lv_obj_set_style_border_width(tile, 0, 0);
    lv_obj_clear_flag(tile, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(tile, random_color(), 0);
    lv_obj_set_pos(tile, x, y);

    lv_obj_set_style_transform_angle(tile, 0, 0);
    lv_obj_set_style_transform_width(tile, 1, 0);
    lv_obj_set_style_transform_height(tile, 1, 0);
    lv_obj_set_style_transform_pivot_x(tile, BLOCK_W / 2, 0);
    lv_obj_set_style_transform_pivot_y(tile, BLOCK_H / 2, 0);

    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, tile);
    lv_anim_set_exec_cb(&anim, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&anim, y, y + SCREEN_HEIGHT);
    lv_anim_set_time(&anim, 700);
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_in);
    lv_anim_set_ready_cb(&anim, anim_delete_tile_cb);
    lv_anim_start(&anim);
}

static void grid_block_event_cb(lv_event_t *e)
{
    lv_obj_t *obj = (lv_obj_t *)lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_PRESSED)
    {
        lv_coord_t x = lv_obj_get_x(obj);
        lv_coord_t y = lv_obj_get_y(obj);
        spawn_falling_tile(lv_obj_get_parent(obj), x, y);
    }
}

void create_grid(lv_obj_t *parent)
{
    for (int row = 0; row < GRID_ROWS; row++)
    {
        for (int col = 0; col < GRID_COLS; col++)
        {
            lv_obj_t *cell = lv_obj_create(parent);
            lv_obj_set_size(cell, CELL_WIDTH - 4, CELL_HEIGHT - 4);
            lv_obj_set_style_radius(cell, 1, 0);
            lv_obj_set_style_bg_color(cell, COLOR_GRID_DEFAULT, 0);
            lv_obj_set_style_border_width(cell, 0, 0);
            lv_obj_clear_flag(cell, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_pos(cell, col * CELL_WIDTH + 2, row * CELL_HEIGHT + 2);
            lv_obj_add_event_cb(cell, grid_block_event_cb, LV_EVENT_ALL, NULL);
            grid_cells[row][col] = cell;
        }
    }
}

void draw_digit(uint8_t digit_index, int start_row, int start_col)
{
    if (digit_index > 12)
        return;
    const uint8_t (*pattern)[3] = DIGITS[digit_index];
    for (int r = 0; r < 5; r++)
    {
        for (int c = 0; c < 3; c++)
        {
            int row = start_row + r;
            int col = start_col + c;
            if (row >= 0 && row < GRID_ROWS && col >= 0 && col < GRID_COLS)
            {
                lv_color_t color = pattern[r][c] ? lv_color_white() : COLOR_GRID_DEFAULT;
                lv_obj_set_style_bg_color(grid_cells[row][col], color, 0);
            }
        }
    }
}

void draw_time_digits(int hour, int minute)
{
    ClockLayout layout = get_clock_layout();
    int r = layout.start_row;
    int c = layout.start_col;

    draw_digit(hour / 10, r, c + 0);
    draw_digit(hour % 10, r, c + 4);
    draw_digit(minute / 10, r, c + 10);
    draw_digit(minute % 10, r, c + 14);
}

void draw_colon(bool colon_visible)
{
    ClockLayout layout = get_clock_layout();
    draw_digit(colon_visible ? 10 : 11, layout.start_row, layout.start_col + 7);
}

void update_time_cb(lv_timer_t *timer)
{
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    bool time_changed = (timeinfo.tm_hour != last_hour || timeinfo.tm_min != last_min);
    if (time_changed)
    {
        draw_time_digits(timeinfo.tm_hour, timeinfo.tm_min);
        last_hour = timeinfo.tm_hour;
        last_min = timeinfo.tm_min;
    }

    colon_visible = !colon_visible;
    draw_colon(colon_visible);

    if (snake_game_just_ended)
    {
        draw_time_digits(last_hour, last_min);
        snake_game_just_ended = false;
    }

    if (esp_random() % 100 < 10 && !lv_anim_count_running())
    {
        start_snake_game();
    }

    if (!lv_anim_count_running())
    {
        move_snake_cb();
    }
}

void set_start_time(int hour, int minute)
{
    time_t now = time(NULL);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);

    timeinfo.tm_hour = hour;
    timeinfo.tm_min = minute;
    timeinfo.tm_sec = 0;

    time_t new_time = mktime(&timeinfo);
    struct timeval tv = {
        .tv_sec = new_time,
        .tv_usec = 0,
    };

    settimeofday(&tv, NULL);
}

void draw_main_ui()
{
    lv_obj_t *screen = lv_obj_create(lv_screen_active());
    lv_obj_set_size(screen, BSP_LCD_V_RES, BSP_LCD_H_RES);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(screen, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_radius(screen, 0, 0);
    lv_obj_set_style_pad_all(screen, 0, 0);
    lv_obj_set_style_outline_width(screen, 0, 0);
    lv_obj_set_style_border_width(screen, 0, 0);

    create_grid(screen);
    set_start_time(22, 19);
    lv_timer_create(update_time_cb, 1000, NULL);
}

void sync_time_from_network()
{
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();

    // Wait for time to be set
    time_t now = 0;
    struct tm timeinfo = {0};
    int retry = 0;
    const int retry_count = 10;

    while (timeinfo.tm_year < (2020 - 1900) && ++retry < retry_count)
    {
        printf("Waiting for system time to be set... (%d/%d)\\n", retry, retry_count);
        vTaskDelay(pdMS_TO_TICKS(2000));
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    if (timeinfo.tm_year >= (2020 - 1900))
    {
        printf("Time synchronized: %s", asctime(&timeinfo));
    }
    else
    {
        printf("Failed to synchronize time\\n");
    }
}

extern "C" void app_main(void)
{
    bsp_display_cfg_t cfg = {
        .lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),
        .buffer_size = BSP_LCD_DRAW_BUFF_SIZE,
        .double_buffer = BSP_LCD_DRAW_BUFF_DOUBLE,
        .flags = {
            .buff_dma = true,
            .buff_spiram = false,
            .sw_rotate = true,
        }};
    bsp_display_start_with_config(&cfg);
    bsp_display_backlight_on();

    lv_disp_t *disp = lv_disp_get_default();
    bsp_display_rotate(disp, LV_DISP_ROTATION_90);

    bsp_display_lock(0);
    draw_main_ui();
    bsp_display_unlock();

    // After Wi-Fi or Ethernet is connected...
    // sync_time_from_network();
}