#pragma once

#include QMK_KEYBOARD_H  // IWYU pragma: keep
#include <lib/lib8tion/lib8tion.h>

// Create RGB from separate r, g, b values (useful for macro-defined colors)
static inline RGB rgb_make(uint8_t r, uint8_t g, uint8_t b) {
    return (RGB){ .r = r, .g = g, .b = b };
}

// Linear interpolation between two RGB colors
static inline RGB rgb_lerp(RGB a, RGB b, uint8_t frac) {
    return (RGB){
        .r = lerp8by8(a.r, b.r, frac),
        .g = lerp8by8(a.g, b.g, frac),
        .b = lerp8by8(a.b, b.b, frac)
    };
}

// Clear all LEDs to black
static inline void rgb_clear_all(void) {
    for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        rgb_matrix_set_color(i, 0, 0, 0);
    }
}
