#pragma once

#include QMK_KEYBOARD_H  // IWYU pragma: keep

// Iterate over all matrix positions with LED index in specified range
// Variables available in body: row, col, led_index
#define FOR_EACH_LED_IN_RANGE(led_min, led_max) \
    for (uint8_t row = 0; row < MATRIX_ROWS; ++row) \
        for (uint8_t col = 0; col < MATRIX_COLS; ++col) \
            for (uint8_t led_index = g_led_config.matrix_co[row][col], _once = 1; \
                 _once && led_index != NO_LED && led_index >= (led_min) && led_index < (led_max); \
                 _once = 0)

// Iterate over all matrix positions (no LED filtering)
// Variables available in body: row, col
#define FOR_EACH_MATRIX_POS() \
    for (uint8_t row = 0; row < MATRIX_ROWS; ++row) \
        for (uint8_t col = 0; col < MATRIX_COLS; ++col)

// Get LED index for current matrix position (use inside FOR_EACH_MATRIX_POS)
#define MATRIX_LED_INDEX(row, col) g_led_config.matrix_co[row][col]

// Get keycode at matrix position for a layer
#define KEYCODE_AT(layer, row, col) keymap_key_to_keycode((layer), (keypos_t){(col), (row)})

