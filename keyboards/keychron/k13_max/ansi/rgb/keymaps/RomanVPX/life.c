#include "grid_map.h"
#include <lib/lib8tion/lib8tion.h>

#define LIFE_WIDTH GRID_WIDTH
#define LIFE_HEIGHT GRID_HEIGHT
#define LIFE_SPEED_MS 300
#define LIFE_FADE_SUB 60

#define LIFE_COLOR_ALIVE    RGB_SPRINGGREEN
#define LIFE_COLOR_DYING    RGB_BLUE
#define LIFE_COLOR_BG       10, 5, 8

#define STATE_DEAD  0
#define STATE_ALIVE 255

static bool life_active = false;
static uint32_t life_timer = 0;
static bool life_paused = false;
static uint8_t life_grid[LIFE_HEIGHT][LIFE_WIDTH];
static uint8_t life_next_grid[LIFE_HEIGHT][LIFE_WIDTH];

bool life_is_active(void) {
    return life_active;
}

static inline void init_random_pattern(void) {
    // Initialize with a random pattern
    for (uint8_t y = 0; y < LIFE_HEIGHT; y++) {
        for (uint8_t x = 0; x < LIFE_WIDTH; x++) {
            if (rand() % 3 == 0) { // 1/3 chance to be alive
                life_grid[y][x] = STATE_ALIVE;
            }
        }
    }
}

void life_game_start(void) {
    life_active = true;
    life_timer = timer_read();

    // Clear grid
    memset(life_grid, STATE_DEAD, sizeof(life_grid));

    // Initialize with a random pattern
    init_random_pattern();
}

void life_game_stop(void) {
    life_paused = false;
    life_active = false;
}

// Check neighbors.
static uint8_t count_neighbors(int8_t x, int8_t y) {
    uint8_t count = 0;
    for (int8_t dy = -1; dy <= 1; dy++) {
        for (int8_t dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;

            // Cyclic boundary conditions - toroidal grid
            int8_t nx = (x + dx + LIFE_WIDTH) % LIFE_WIDTH;
            int8_t ny = (y + dy + LIFE_HEIGHT) % LIFE_HEIGHT;

            if (life_grid[ny][nx] == STATE_ALIVE) count++;
        }
    }
    return count;
}

static void life_update(void) {
    for (uint8_t y = 0; y < LIFE_HEIGHT; y++) {
        for (uint8_t x = 0; x < LIFE_WIDTH; x++) {
            uint8_t neighbors = count_neighbors(x, y);
            uint8_t current_val = life_grid[y][x];
            bool is_alive_now = (current_val == STATE_ALIVE);
            bool will_be_alive = false;

            if (is_alive_now) {
                will_be_alive = (neighbors == 2 || neighbors == 3);
            } else {
                will_be_alive = (neighbors == 3);
            }

            if (will_be_alive) {
                life_next_grid[y][x] = STATE_ALIVE;
            } else {
                life_next_grid[y][x] = qsub8(current_val, LIFE_FADE_SUB);
            }
        }
    }

    memcpy(life_grid, life_next_grid, sizeof(life_grid));
}

void life_game_task(void) {
    if (!life_active) return;

    if (!life_paused && timer_elapsed(life_timer) > LIFE_SPEED_MS) {
        life_update();
        life_timer = timer_read();
    }
}

bool life_game_process_record(uint16_t keycode, keyrecord_t *record) {
    if (!life_active) return true;

    if (record->event.pressed) {
        if (keycode == KC_ESC) {
            life_game_stop();
            return false;
        }

        if (keycode == KC_SPC) {
            life_paused = !life_paused;
            return false;
        }

        if (keycode == KC_BSPC) {
            memset(life_grid, STATE_DEAD, sizeof(life_grid));
            return false;
        }

        // Find which key in grid corresponds to keycode
        uint8_t r = record->event.key.row;
        uint8_t c = record->event.key.col;
        uint8_t matrix_idx = r * M_COLS + c;

        for (uint8_t y = 0; y < LIFE_HEIGHT; y++) {
            for (uint8_t x = 0; x < LIFE_WIDTH; x++) {
                 if (pgm_read_byte(&GRID_MAP[y][x]) == matrix_idx) {
                     if (life_grid[y][x] == STATE_ALIVE) {
                         life_grid[y][x] = STATE_DEAD;
                     } else {
                         life_grid[y][x] = STATE_ALIVE;
                     }
                     return false; // Consume key - toggle cell state
                 }
            }
        }
    } else {
         return true;
    }

    return false;
}


static inline RGB life_rgb_lerp(RGB a, RGB b, uint8_t frac) {
    RGB res;
    res.r = lerp8by8(a.r, b.r, frac);
    res.g = lerp8by8(a.g, b.g, frac);
    res.b = lerp8by8(a.b, b.b, frac);
    return res;
}

void life_game_render(void) {
    if (!life_active) return;

    // Clear "screen"
    for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        rgb_matrix_set_color(i, 0, 0, 0);
    }

    for (uint8_t y = 0; y < LIFE_HEIGHT; y++) {
        for (uint8_t x = 0; x < LIFE_WIDTH; x++) {
            uint8_t m_idx = pgm_read_byte(&GRID_MAP[y][x]);
            if (m_idx != 0xFF) {
                uint8_t l_idx = get_led_index_from_matrix(m_idx);
                if (l_idx != NO_LED) {
                    uint8_t val = life_grid[y][x];
                    if (val == STATE_ALIVE) {
                        rgb_matrix_set_color(l_idx, LIFE_COLOR_ALIVE);
                    } else if (val > STATE_DEAD) {
                        RGB dying_color = life_rgb_lerp((RGB){LIFE_COLOR_BG}, (RGB){LIFE_COLOR_DYING}, val);
                        rgb_matrix_set_color(l_idx, dying_color.r, dying_color.g, dying_color.b);
                    } else {
                        rgb_matrix_set_color(l_idx, LIFE_COLOR_BG);
                    }
                }
            }
        }
    }
}
