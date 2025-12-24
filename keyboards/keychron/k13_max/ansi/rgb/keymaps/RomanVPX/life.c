#include "grid_map.h"

#define LIFE_WIDTH GRID_WIDTH
#define LIFE_HEIGHT GRID_HEIGHT
#define LIFE_SPEED_MS 500

#define LIFE_COLOR_ALIVE    RGB_SPRINGGREEN
#define LIFE_COLOR_DYING    15, 20, 120
#define LIFE_COLOR_DEAD     10, 5, 10

#define STATE_DEAD  0
#define STATE_ALIVE 1
#define STATE_DYING 2

static bool life_active = false;
static uint32_t life_timer = 0;
static bool life_paused = false;
static uint8_t life_grid[LIFE_HEIGHT][LIFE_WIDTH];
static uint8_t life_next_grid[LIFE_HEIGHT][LIFE_WIDTH];

bool life_is_active(void) {
    return life_active;
}

void life_game_start(void) {
    life_active = true;
    life_timer = timer_read();

    // Clear grid
    memset(life_grid, STATE_DEAD, sizeof(life_grid));

    // Initial pattern
    life_grid[1][5] = STATE_ALIVE;
    life_grid[1][6] = STATE_ALIVE;
    life_grid[1][7] = STATE_ALIVE;
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
            uint8_t state = life_grid[y][x];
            bool alive = (state == STATE_ALIVE);

            if (alive) {
                // Survival: 2 or 3 neighbors
                if (neighbors == 2 || neighbors == 3) {
                    life_next_grid[y][x] = STATE_ALIVE;
                } else {
                    life_next_grid[y][x] = STATE_DYING;
                }
            } else {
                // Birth: 3 neighbors
                if (neighbors == 3) {
                    life_next_grid[y][x] = STATE_ALIVE;
                } else {
                    life_next_grid[y][x] = STATE_DEAD;
                }
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
                     if (life_grid[y][x] == STATE_ALIVE) {
                         rgb_matrix_set_color(l_idx, LIFE_COLOR_ALIVE);
                     } else if (life_grid[y][x] == STATE_DYING) {
                         rgb_matrix_set_color(l_idx, LIFE_COLOR_DYING);
                     } else {
                         rgb_matrix_set_color(l_idx, LIFE_COLOR_DEAD);
                     }
                 }
            }
        }
    }
}
