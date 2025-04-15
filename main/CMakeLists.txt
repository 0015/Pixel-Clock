# List of source files
set(SOURCES
    "main.cpp"
    "ui_resources/global.c"
    "ui_resources/digit_font.c"
    "ui_resources/snake.c"
)

# List of include directories
set(INCLUDE_DIRS
    "include"
)

# Required components
set(REQUIRED_COMPONENTS
    nvs_flash
    lvgl   
    esp32_p4_nano
    esp_lcd_touch
    esp_lcd_touch_gt911
)

idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS ${INCLUDE_DIRS}
    REQUIRES ${REQUIRED_COMPONENTS}
)
