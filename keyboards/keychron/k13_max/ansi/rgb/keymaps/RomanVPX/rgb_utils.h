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

// Scale all RGB components by a single value (like nscale8x3 from FastLED)
static inline RGB rgb_scale(RGB color, uint8_t val) {
    color.r = scale8(color.r, val);
    color.g = scale8(color.g, val);
    color.b = scale8(color.b, val);
    return color;
}

// Get effective saturation scaled by global RGB matrix saturation
static inline uint8_t rgb_get_effective_sat(uint8_t sat) {
    return scale8(sat, rgb_matrix_get_sat());
}

// Convert HSV to RGB with automatic effective saturation calculation
static inline RGB hsv_to_rgb_effective(HSV hsv) {
    hsv.s = rgb_get_effective_sat(hsv.s);
    return hsv_to_rgb(hsv);
}

// Configuration for pulsing effect
typedef struct {
    uint8_t speed_divisor;    // Right shift amount for timer (higher = slower)
    uint8_t min_value_divisor; // Divisor for minimum brightness (higher = darker minimum)
} PulsingConfig;

// Default pulsing parameters
#define PULSING_SPEED_DIV       2
#define PULSING_MIN_VALUE_DIV   3

// Default pulsing configuration
#define PULSING_CONFIG_DEFAULT (PulsingConfig){.speed_divisor = PULSING_SPEED_DIV, .min_value_divisor = PULSING_MIN_VALUE_DIV}

// Calculate pulsing brightness value using sine wave with phase offset
static inline uint8_t pulsing_value(uint8_t max_value, PulsingConfig config, uint8_t phase_offset) {
    uint8_t sin_wave = sin8((timer_read() >> config.speed_divisor) + phase_offset);
    uint8_t min_v = max_value / config.min_value_divisor;
    return min_v + scale8(sin_wave, max_value - min_v);
}

// Apply pulsing effect to raw RGB values
static inline RGB rgb_pulsing_raw(RGB color, uint8_t max_value, PulsingConfig config, uint8_t phase_offset) {
    return rgb_scale(color, pulsing_value(max_value, config, phase_offset));
}

// Create pulsing RGB color from HSV (with effective saturation) and phase offset
static inline RGB rgb_pulsing(HSV hsv, uint8_t max_value, PulsingConfig config, uint8_t phase_offset) {
    hsv.v = pulsing_value(max_value, config, phase_offset);
    return hsv_to_rgb_effective(hsv);
}
