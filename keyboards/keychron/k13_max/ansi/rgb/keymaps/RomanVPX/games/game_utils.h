#pragma once

#include QMK_KEYBOARD_H  // IWYU pragma: keep
#include "rgb_utils.h"

static uint16_t rng_state = 0xACE1;

static inline uint8_t game_random(uint8_t max) {
    if (max == 0) return 0;
    // 16-bit LFSR with taps at 16, 14, 13, 11
    uint16_t bit = ((rng_state >> 0) ^ (rng_state >> 2) ^ (rng_state >> 3) ^ (rng_state >> 5)) & 1;
    rng_state = (rng_state >> 1) | (bit << 15);
    // Mix in timer for extra entropy
    rng_state ^= (uint16_t)timer_read();
    return rng_state % max;
}

// Score bar on F-key row (matrix row 0, cols 2–13 = F1–F12)
#define SCORE_BAR_ROW       0
#define SCORE_BAR_COL_START 2
#define SCORE_BAR_LENGTH    12
#define SCORE_TIER_COUNT    6
#define SCORE_BLINK_SPEED   2

static const uint8_t PROGMEM score_tier_colors[][3] = {
    {RGB_GREEN},
    {RGB_CYAN},
    {RGB_YELLOW},
    {RGB_ORANGE},
    {RGB_RED},
    {RGB_MAGENTA},
};

// Render score bar on F-key row
// score: current score value
// game_over: if true, bar pulses
static inline void render_score_bar(uint16_t score, bool game_over) {
    if (score == 0) return;

    uint8_t total_leds = (score > SCORE_BAR_LENGTH * SCORE_TIER_COUNT * 2)
                       ? SCORE_BAR_LENGTH * SCORE_TIER_COUNT * 2
                       : (uint8_t)score;

    uint8_t filled = ((total_leds - 1) % SCORE_BAR_LENGTH) + 1;
    uint8_t tier   = (total_leds - 1) / SCORE_BAR_LENGTH;

    uint8_t color_idx = tier % SCORE_TIER_COUNT;
    bool blink = tier >= SCORE_TIER_COUNT;

    RGB color = {
        .r = pgm_read_byte(&score_tier_colors[color_idx][0]),
        .g = pgm_read_byte(&score_tier_colors[color_idx][1]),
        .b = pgm_read_byte(&score_tier_colors[color_idx][2]),
    };

    // Blink effect: toggle visibility using timer
    bool blink_visible = !blink || ((timer_read() >> (8 + SCORE_BLINK_SPEED)) & 1);

    // Game over: pulsing effect
    if (game_over) {
        color = rgb_pulsing_raw(color, PULSING_CONFIG_DEFAULT, 0);
        blink_visible = true; // Override blink during game over
    }

    for (uint8_t i = 0; i < SCORE_BAR_LENGTH; i++) {
        uint8_t led = g_led_config.matrix_co[SCORE_BAR_ROW][SCORE_BAR_COL_START + i];
        if (led != NO_LED) {
            if (i < filled && blink_visible) {
                rgb_matrix_set_color(led, color.r, color.g, color.b);
            } else {
                rgb_matrix_set_color(led, 0, 0, 0);
            }
        }
    }
}
