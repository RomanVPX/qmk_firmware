#include "game_utils.h"
#include "rgb_utils.h"
#include <string.h>

// --- Random Number Generator ---
static uint16_t rng_state = 0xACE1;

uint8_t game_random(uint8_t max) {
    if (max == 0) return 0;
    // 16-bit LFSR with taps at 16, 14, 13, 11
    uint16_t bit = ((rng_state >> 0) ^ (rng_state >> 2) ^ (rng_state >> 3) ^ (rng_state >> 5)) & 1;
    rng_state = (rng_state >> 1) | (bit << 15);
    // Mix in timer for extra entropy
    rng_state ^= (uint16_t)timer_read();
    return rng_state % max;
}

// --- Game Speed ---
uint16_t game_speed(uint16_t score, uint16_t per, uint16_t step, uint16_t base, uint16_t min_speed) {
    uint16_t reduction = (score / per) * step;
    return (reduction >= base - min_speed) ? min_speed : base - reduction;
}

// --- High Scores ---
high_scores_t high_scores_read(void) {
    high_scores_t hs;
    eeprom_read_block(&hs, (void*)EECONFIG_USER_DATABLOCK, sizeof(high_scores_t));
    if (hs.magic != HIGH_SCORE_MAGIC) {
        memset(&hs, 0, sizeof(high_scores_t));
        hs.magic = HIGH_SCORE_MAGIC;
        high_scores_write(hs); // Initialize EEPROM
    }
    return hs;
}

void high_scores_write(high_scores_t hs) {
    hs.magic = HIGH_SCORE_MAGIC; // Ensure magic is set
    eeprom_update_block(&hs, (void*)EECONFIG_USER_DATABLOCK, sizeof(high_scores_t));
}

// --- Score Bar ---
const uint8_t PROGMEM score_tier_colors[][3] = {
    {RGB_GREEN},
    {RGB_CYAN},
    {RGB_YELLOW},
    {RGB_ORANGE},
    {RGB_RED},
    {RGB_MAGENTA},
};

void render_score_bar(uint16_t score, bool game_over) {
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
        color = rgb_pulsing_raw(color, SCORE_BAR_MAX_VALUE, PULSING_CONFIG_DEFAULT, 0);
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

// --- Firework State ---
FireworkSpark fw_sparks[FIREWORK_MAX_SPARKS];
uint16_t fw_spawn_timer;
bool fw_active = false;

void firework_start(void) {
    fw_active = true;
    memset(fw_sparks, 0, sizeof(fw_sparks));
    fw_spawn_timer = timer_read();
}

void firework_stop(void) {
    fw_active = false;
}
