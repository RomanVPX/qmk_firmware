#pragma once

#include QMK_KEYBOARD_H  // IWYU pragma: keep

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
