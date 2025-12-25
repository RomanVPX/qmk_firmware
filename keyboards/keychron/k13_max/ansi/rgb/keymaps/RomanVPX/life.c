#include "grid_map.h"
#include "rgb_utils.h"

#define LIFE_WIDTH GRID_WIDTH
#define LIFE_HEIGHT GRID_HEIGHT
#define LIFE_SPEED_MS 300

#define LIFE_COLOR_ALIVE    RGB_SPRINGGREEN
#define LIFE_COLOR_DYING    RGB_RED
#define LIFE_COLOR_BG       10, 5, 8
#define LIFE_FADE_SUB       2

#define DISPLAY_DEAD  0
#define DISPLAY_ALIVE 255

static bool life_active = false;
static uint32_t life_timer = 0;
static bool life_paused = false;
static uint8_t life_display_grid[LIFE_HEIGHT][LIFE_WIDTH];
static bool life_state_grid[LIFE_HEIGHT][LIFE_WIDTH];
static bool life_next_state_grid[LIFE_HEIGHT][LIFE_WIDTH];

bool life_is_active(void) {
    return life_active;
}

static inline void init_random_pattern(void) {
    // Initialize with a random pattern
    FOR_EACH_GRID_POS() {
        if (rand() % 3 == 0) { // 1/3 chance to be alive
            life_state_grid[gy][gx] = true;
            life_display_grid[gy][gx] = DISPLAY_ALIVE;
        } else {
            life_state_grid[gy][gx] = false;
            life_display_grid[gy][gx] = DISPLAY_DEAD;
        }
    }
}

void life_game_start(void) {
    life_active = true;
    life_timer = timer_read();

    // Clear grids
    memset(life_display_grid, DISPLAY_DEAD, sizeof(life_display_grid));
    memset(life_state_grid, 0, sizeof(life_state_grid));

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

            if (life_state_grid[ny][nx]) count++;
        }
    }
    return count;
}

static void life_update(void) {
    FOR_EACH_GRID_POS() {
        uint8_t neighbors = count_neighbors(gx, gy);
        bool is_alive = life_state_grid[gy][gx];
        bool will_be_alive = false;

        if (is_alive) {
            // Survival: 2 or 3 neighbors
            will_be_alive = (neighbors == 2 || neighbors == 3);
        } else {
            // Birth: 3 neighbors
            will_be_alive = (neighbors == 3);
        }

        life_next_state_grid[gy][gx] = will_be_alive;
    }

    memcpy(life_state_grid, life_next_state_grid, sizeof(life_state_grid));
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
            memset(life_state_grid, 0, sizeof(life_state_grid));
            return false;
        }

        // Find which key in grid corresponds to keycode
        uint8_t r = record->event.key.row;
        uint8_t c = record->event.key.col;
        uint8_t matrix_idx = r * M_COLS + c;

        FOR_EACH_GRID_POS() {
            if (pgm_read_byte(&GRID_MAP[gy][gx]) == matrix_idx) {
                // Toggle state
                bool new_state = !life_state_grid[gy][gx];
                life_state_grid[gy][gx] = new_state;
                life_display_grid[gy][gx] = new_state ? DISPLAY_ALIVE : life_display_grid[gy][gx];
                return false; // Consume key
            }
        }
    } else {
        return true;
    }

    return false;
}

void life_game_render(void) {
    if (!life_active) return;

    rgb_matrix_set_color_all(RGB_OFF);

    RGB bg_color = rgb_make(LIFE_COLOR_BG);
    RGB target_color = rgb_make(LIFE_COLOR_DYING);

    FOR_EACH_GRID_POS() {
        if (life_state_grid[gy][gx]) {
            life_display_grid[gy][gx] = DISPLAY_ALIVE;
        } else {
            life_display_grid[gy][gx] = qsub8(life_display_grid[gy][gx], LIFE_FADE_SUB);
        }

        uint8_t l_idx = grid_get_led(gy, gx);
        if (l_idx != NO_LED) {
            uint8_t val = life_display_grid[gy][gx];
            if (val == DISPLAY_ALIVE) {
                rgb_matrix_set_color(l_idx, LIFE_COLOR_ALIVE);
            } else if (val > DISPLAY_DEAD) {
                RGB dying_color = rgb_lerp(bg_color, target_color, val);
                rgb_matrix_set_color(l_idx, dying_color.r, dying_color.g, dying_color.b);
            } else {
                rgb_matrix_set_color(l_idx, LIFE_COLOR_BG);
            }
        }
    }
}
