# Pixel Clock

**Pixel Clock** is a minimalist digital clock UI built using **LVGL v9** on the **ESP32-P4**.  
It displays pixel-style digits over a grid layout and includes a hidden snake animation mode to bring life to your embedded screen — all optimized for real-time performance and ease of integration.

> Designed for LVGL-ready environments. Just configure your display and grid, and you're ready to run.

---

## Features

- **Pixel-style digits** using a 3×5 font layout on a tile grid
- **Blinking colon** that toggles every second
- **Optimized redraw** — updates only the digits that change
- **Tap interaction** — trigger random colorful falling tiles
- **Snake game mode**:
  - Triggers randomly during idle
  - Green snake moves toward red tile
  - Snake disappears on contact, restoring clock UI

---

## Built With

- **ESP32-P4** (ESP-IDF framework)
- **LVGL v9**
- Modular C code
- Simple grid logic (19×9 by default)

---

## Project Structure

```plaintext
main/
├── include/
│   ├── digit_font.h
│   ├── global.h
│   └── snake.h
├── ui_resources/
│   ├── digit_font.c
│   ├── global.c
│   └── snake.c
├── main.cpp
```

---

## Screenshots & Demo

*(Add your screenshots or YouTube demo link here)*

---

## Customization

- Adjust grid resolution:
  ```c
  #define GRID_ROWS 9
  #define GRID_COLS 19
  ```
- Modify fonts in `digit_font.h`
- Change snake speed, style, or colors in `snake.c`
- Use LVGL’s animation features to add your own flair

---

## Requirements

- ESP32 board (This project was tested on the ESP32-P4 board.)
- Display compatible with LVGL (800×1280 tested)
- Optional: GT911 capacitive touch panel

---

## License

MIT License  
(c) 2025 Eric Nam

---

*Simple. Playful. Pixel-perfect.*
