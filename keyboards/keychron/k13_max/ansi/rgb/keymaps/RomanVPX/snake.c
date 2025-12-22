#include QMK_KEYBOARD_H

#define SNAKE_WIDTH 12
#define SNAKE_HEIGHT 4
#define SNAKE_MAX_LENGTH (SNAKE_WIDTH * SNAKE_HEIGHT)
#define SNAKE_SPEED_MS 400

// Matrix Cols is 17 for K13 Max
#define M_COLS 17

// Logical map to Matrix Index (row * 17 + col)
// 0xFF = invalid/empty
static const uint8_t PROGMEM snake_map[SNAKE_HEIGHT][SNAKE_WIDTH] = {
    // Row 0 -> Physical Row 1 (1, 2... =)
    {
        1*M_COLS+1, 1*M_COLS+2, 1*M_COLS+3, 1*M_COLS+4, 1*M_COLS+5, 1*M_COLS+6,
        1*M_COLS+7, 1*M_COLS+8, 1*M_COLS+9, 1*M_COLS+10, 1*M_COLS+11, 1*M_COLS+12
    },
    // Row 1 -> Physical Row 2 (Q, W... ])
    {
        2*M_COLS+1, 2*M_COLS+2, 2*M_COLS+3, 2*M_COLS+4, 2*M_COLS+5, 2*M_COLS+6,
        2*M_COLS+7, 2*M_COLS+8, 2*M_COLS+9, 2*M_COLS+10, 2*M_COLS+11, 2*M_COLS+12
    },
    // Row 2 -> Physical Row 3 (A, S... ') - Last entries invalid to match width 12
    {
        3*M_COLS+1, 3*M_COLS+2, 3*M_COLS+3, 3*M_COLS+4, 3*M_COLS+5, 3*M_COLS+6,
        3*M_COLS+7, 3*M_COLS+8, 3*M_COLS+9, 3*M_COLS+10, 3*M_COLS+11, 0xFF
    },
    // Row 3 -> Physical Row 4 (Z, X... /) - Last entries invalid
    {
        4*M_COLS+2, 4*M_COLS+3, 4*M_COLS+4, 4*M_COLS+5, 4*M_COLS+6, 4*M_COLS+7,
        4*M_COLS+8, 4*M_COLS+9, 4*M_COLS+10, 4*M_COLS+11, 0xFF, 0xFF
    }
};

typedef struct {
    int8_t x;
    int8_t y;
} SnakePoint;

static bool snake_active = false;

bool snake_is_active(void) {
    return snake_active;
}
static uint32_t snake_timer = 0;
static SnakePoint snake_body[SNAKE_MAX_LENGTH];
static uint8_t snake_len = 0;
static SnakePoint snake_food;
static int8_t snake_dir_x = 1;
static int8_t snake_dir_y = 0;
static int8_t snake_next_dir_x = 1;
static int8_t snake_next_dir_y = 0;
static bool snake_game_over = false;

// Helper to get LED index from matrix index
static uint8_t get_led_index_from_matrix(uint8_t matrix_idx) {
    if (matrix_idx == 0xFF) return NO_LED;
    uint8_t r = matrix_idx / M_COLS;
    uint8_t c = matrix_idx % M_COLS;
    // Safety check
    if (r >= MATRIX_ROWS || c >= MATRIX_COLS) return NO_LED;
    return g_led_config.matrix_co[r][c];
}

static void snake_spawn_food(void) {
    while (true) {
        snake_food.x = timer_read() % SNAKE_WIDTH;
        snake_food.y = (timer_read() / 13) % SNAKE_HEIGHT;

        // Check if valid position (not 0xFF in map)
        if (pgm_read_byte(&snake_map[snake_food.y][snake_food.x]) == 0xFF) continue;

        // Check collision with snake
        bool collision = false;
        for (uint8_t i = 0; i < snake_len; i++) {
            if (snake_body[i].x == snake_food.x && snake_body[i].y == snake_food.y) {
                collision = true;
                break;
            }
        }
        if (!collision) break;
    }
}

void snake_game_start(void) {
    snake_active = true;
    snake_game_over = false;
    snake_len = 3;
    snake_body[0] = (SnakePoint){2, 1};
    snake_body[1] = (SnakePoint){1, 1};
    snake_body[2] = (SnakePoint){0, 1};
    snake_dir_x = 1; snake_dir_y = 0;
    snake_next_dir_x = 1; snake_next_dir_y = 0;
    snake_spawn_food();
    snake_timer = timer_read();

    // Disable other RGB effects and clear
    // We rely on our render hook to draw over whatever is there,
    // but setting solid black helps if our hook doesn't cover everything.
    // However, we want to respect user's RGB mode when we exit.
    // So we just override in the render function.
}

void snake_game_stop(void) {
    snake_active = false;
}

static void snake_update(void) {
    if (snake_game_over) return;

    // Apply next direction
    if (snake_next_dir_x != -snake_dir_x || snake_next_dir_y != -snake_dir_y) {
        snake_dir_x = snake_next_dir_x;
        snake_dir_y = snake_next_dir_y;
    }

    SnakePoint new_head = {
        snake_body[0].x + snake_dir_x,
        snake_body[0].y + snake_dir_y
    };

    // Wall collision (die)
    if (new_head.x < 0 || new_head.x >= SNAKE_WIDTH ||
        new_head.y < 0 || new_head.y >= SNAKE_HEIGHT ||
        pgm_read_byte(&snake_map[new_head.y][new_head.x]) == 0xFF) {
        snake_game_over = true;
        return;
    }

    // Self collision
    for (uint8_t i = 0; i < snake_len; i++) {
        if (snake_body[i].x == new_head.x && snake_body[i].y == new_head.y) {
            snake_game_over = true;
            return;
        }
    }

    // Move snake
    // Shift body
    for (uint8_t i = snake_len; i > 0; i--) {
        snake_body[i] = snake_body[i-1];
    }
    snake_body[0] = new_head;

    // Check food
    if (new_head.x == snake_food.x && new_head.y == snake_food.y) {
        if (snake_len < SNAKE_MAX_LENGTH) {
            snake_len++;
        }
        snake_spawn_food();
    }
}

// Hook for matrix_scan_user
void snake_game_task(void) {
    if (!snake_active || snake_game_over) return;

    if (timer_elapsed(snake_timer) > SNAKE_SPEED_MS) {
        snake_update();
        snake_timer = timer_read();
    }
}

// Hook for process_record_user
bool snake_game_process_record(uint16_t keycode, keyrecord_t *record) {
    if (!snake_active) return true;

    if (record->event.pressed) {
        switch (keycode) {
            case KC_UP:
            case HYPR(KC_UP):
                if (snake_dir_y == 0) { snake_next_dir_x = 0; snake_next_dir_y = -1; }
                return false;
            case KC_DOWN:
            case HYPR(KC_DOWN):
                if (snake_dir_y == 0) { snake_next_dir_x = 0; snake_next_dir_y = 1; }
                return false;
            case KC_LEFT:
            case HYPR(KC_LEFT):
                if (snake_dir_x == 0) { snake_next_dir_x = -1; snake_next_dir_y = 0; }
                return false;
            case KC_RIGHT:
            case HYPR(KC_RIGHT):
                if (snake_dir_x == 0) { snake_next_dir_x = 1; snake_next_dir_y = 0; }
                return false;
            case KC_ESC:
                snake_game_stop();
                return false;
            default:
                // Ignore other keys
                return false;
        }
    }
    return false; // Consume releases too
}

// Hook for rgb_matrix_indicators_advanced_user
void snake_game_render(void) {
    if (!snake_active) return;

    // Clear screen
    for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        rgb_matrix_set_color(i, 0, 0, 0);
    }

    if (snake_game_over) {
        // Red full board or something?
        for (uint8_t i = 0; i < snake_len; i++) {
             uint8_t m_idx = pgm_read_byte(&snake_map[snake_body[i].y][snake_body[i].x]);
             uint8_t l_idx = get_led_index_from_matrix(m_idx);
             if (l_idx != NO_LED) rgb_matrix_set_color(l_idx, 255, 0, 0);
        }
        return;
    }

    // Draw Food (Red)
    uint8_t f_m_idx = pgm_read_byte(&snake_map[snake_food.y][snake_food.x]);
    uint8_t f_l_idx = get_led_index_from_matrix(f_m_idx);
    if (f_l_idx != NO_LED) {
        rgb_matrix_set_color(f_l_idx, 255, 0, 0);
    }

    // Draw Snake (Green)
    for (uint8_t i = 0; i < snake_len; i++) {
        uint8_t m_idx = pgm_read_byte(&snake_map[snake_body[i].y][snake_body[i].x]);
        uint8_t l_idx = get_led_index_from_matrix(m_idx);
        if (l_idx != NO_LED) {
            if (i == 0) {
                 rgb_matrix_set_color(l_idx, 0, 255, 255); // Cyan head
            } else {
                 rgb_matrix_set_color(l_idx, 0, 255, 0);
            }
        }
    }
}
