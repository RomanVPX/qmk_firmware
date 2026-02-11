#pragma once

#include QMK_KEYBOARD_H  // IWYU pragma: keep
#include "rgb_utils.h"  // IWYU pragma: keep

// --- Random Number Generator ---
uint8_t game_random(uint8_t max);

// --- Game Speed ---
uint16_t game_speed(uint16_t score, uint16_t per, uint16_t step, uint16_t base, uint16_t min_speed);

// --- High Scores (EEPROM) ---

#define SCORE_FIELD_void(name)
#define SCORE_FIELD_uint16_t(name) uint16_t name;
#define GET_SCORE_FIELD(type, name) SCORE_FIELD_##type(name)

typedef struct {
    uint16_t magic;
#define GAME_X(keycode, prefix, type) GET_SCORE_FIELD(type, prefix)
#include "games_x.inc"
} high_scores_t;

// Magic number depends on struct size to auto-reset on layout changes
#define HIGH_SCORE_MAGIC (0x616D + sizeof(high_scores_t)) // 'gm' + size

high_scores_t high_scores_read(void);
void high_scores_write(high_scores_t hs);

// Score bar on F-key row (matrix row 0, cols 2–13 = F1–F12)
#define SCORE_BAR_ROW       0
#define SCORE_BAR_COL_START 2
#define SCORE_BAR_LENGTH    12
#define SCORE_TIER_COUNT    6
#define SCORE_BLINK_SPEED   2
#define SCORE_BAR_MAX_VALUE RGB_MATRIX_MAXIMUM_BRIGHTNESS

extern const uint8_t PROGMEM score_tier_colors[][3];

void render_score_bar(uint16_t score, bool game_over);

// --- Firework Animation (new high score celebration) ---

#define FIREWORK_MAX_SPARKS     8
#define FIREWORK_SPAWN_MS       80
#define FIREWORK_FADE_SPEED     6
#define FIREWORK_RND_ATTEMPTS   5
#define FIREWORK_MAX_BRIGHTNESS RGB_MATRIX_MAXIMUM_BRIGHTNESS

typedef struct {
    uint8_t x, y;
    uint8_t brightness;
    uint8_t color_idx;
} FireworkSpark;

extern FireworkSpark fw_sparks[FIREWORK_MAX_SPARKS];
extern uint16_t fw_spawn_timer;
extern bool fw_active;

void firework_start(void);
void firework_stop(void);

// Render firework sparks on the game grid (call after drawing background)
// Depends on grid_map.h being included and configured in the source file
#ifdef GRID_MAP_INCLUDED
static inline void firework_render(void) {
    if (!fw_active) return;

    if (timer_elapsed(fw_spawn_timer) >= FIREWORK_SPAWN_MS) {
        fw_spawn_timer = timer_read();
        for (uint8_t i = 0; i < FIREWORK_MAX_SPARKS; i++) {
            if (fw_sparks[i].brightness == 0) {
                uint8_t x = 0, y = 0;
                for (uint8_t attempt = 0; attempt < FIREWORK_RND_ATTEMPTS; attempt++) {
                    x = game_random(GRID_WIDTH);
                    y = game_random(GRID_HEIGHT);
                    if (grid_get_led(y, x) != NO_LED) break;
                }
                fw_sparks[i].x = x;
                fw_sparks[i].y = y;
                fw_sparks[i].brightness = FIREWORK_MAX_BRIGHTNESS;
                fw_sparks[i].color_idx = game_random(SCORE_TIER_COUNT);
                break;
            }
        }
    }

    for (uint8_t i = 0; i < FIREWORK_MAX_SPARKS; i++) {
        if (fw_sparks[i].brightness > 0) {
            uint8_t l_idx = grid_get_led(fw_sparks[i].y, fw_sparks[i].x);
            if (l_idx != NO_LED) {
                RGB c = {
                    .r = pgm_read_byte(&score_tier_colors[fw_sparks[i].color_idx][0]),
                    .g = pgm_read_byte(&score_tier_colors[fw_sparks[i].color_idx][1]),
                    .b = pgm_read_byte(&score_tier_colors[fw_sparks[i].color_idx][2]),
                };
                c = rgb_scale(c, fw_sparks[i].brightness);
                rgb_matrix_set_color(l_idx, c.r, c.g, c.b);
            }
            fw_sparks[i].brightness = qsub8(fw_sparks[i].brightness, FIREWORK_FADE_SPEED);
        }
    }
}
#endif // GRID_MAP_INCLUDED

// Helper macro to update high score
#define UPDATE_HIGH_SCORE(SCORE, HIGH_SCORE_VAR, RECORD_FLAG, STRUCT_FIELD) \
    do { \
        if ((SCORE) > (HIGH_SCORE_VAR)) { \
            (HIGH_SCORE_VAR) = (SCORE); \
            (RECORD_FLAG) = true; \
            high_scores_t hs = high_scores_read(); \
            hs.STRUCT_FIELD = (HIGH_SCORE_VAR); \
            high_scores_write(hs); \
            firework_start(); \
        } \
    } while (0)
