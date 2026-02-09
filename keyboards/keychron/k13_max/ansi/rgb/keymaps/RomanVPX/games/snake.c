#define GRID_SNAKE
#include "grid_map.h"
#include "game_utils.h"

#define SNAKE_WIDTH GRID_WIDTH
#define SNAKE_HEIGHT GRID_HEIGHT
#define SNAKE_MAX_LENGTH (SNAKE_WIDTH * SNAKE_HEIGHT)
#define SNAKE_SPEED_MS 400

#define SNAKE_COLOR_HEAD     RGB_CYAN
#define SNAKE_COLOR_BODY     RGB_GREEN
#define SNAKE_COLOR_FOOD     RGB_RED
#define SNAKE_COLOR_DEATH    RGB_RED
#define SNAKE_COLOR_BG       22, 10, 30

#define INPUT_BUFFER_SIZE 5

typedef struct {
    int8_t x;
    int8_t y;
} SnakePoint;

static bool snake_active = false;

bool snake_is_active(void) {
    return snake_active;
}

static uint16_t snake_timer = 0;
static SnakePoint snake_body[SNAKE_MAX_LENGTH];
static uint8_t snake_len = 0;
static SnakePoint snake_food;
static int8_t snake_dir_x = 1;
static int8_t snake_dir_y = 0;
static int8_t snake_next_dir_x = 1;
static int8_t snake_next_dir_y = 0;
static bool snake_game_over = false;

static uint8_t input_buffer[INPUT_BUFFER_SIZE];
static uint8_t input_head = 0;
static uint8_t input_tail = 0;

static void snake_push_input(uint8_t dir_code) {
    uint8_t next = (input_head + 1) % INPUT_BUFFER_SIZE;
    if (next != input_tail) {
        input_buffer[input_head] = dir_code;
        input_head = next;
    }
}

static uint8_t snake_pop_input(void) {
    if (input_head == input_tail) return 0;
    uint8_t val = input_buffer[input_tail];
    input_tail = (input_tail + 1) % INPUT_BUFFER_SIZE;
    return val;
}

static void snake_spawn_food(void) {
    // Fast path: random placement
    for (uint8_t attempt = 0; attempt < 20; attempt++) {
        snake_food.x = game_random(SNAKE_WIDTH);
        snake_food.y = game_random(SNAKE_HEIGHT);

        if (pgm_read_byte(&GRID_MAP[snake_food.y][snake_food.x]) == 0xFF) continue;

        bool collision = false;
        for (uint8_t i = 0; i < snake_len; i++) {
            if (snake_body[i].x == snake_food.x && snake_body[i].y == snake_food.y) {
                collision = true;
                break;
            }
        }
        if (!collision) return;
    }

    // Deterministic fallback: linear scan from random offset
    uint8_t total = SNAKE_WIDTH * SNAKE_HEIGHT;
    uint8_t start = game_random(total);
    for (uint8_t i = 0; i < total; i++) {
        uint8_t idx = (start + i) % total;
        uint8_t fx = idx % SNAKE_WIDTH;
        uint8_t fy = idx / SNAKE_WIDTH;
        if (pgm_read_byte(&GRID_MAP[fy][fx]) == 0xFF) continue;

        bool collision = false;
        for (uint8_t j = 0; j < snake_len; j++) {
            if (snake_body[j].x == fx && snake_body[j].y == fy) {
                collision = true;
                break;
            }
        }
        if (!collision) {
            snake_food.x = fx;
            snake_food.y = fy;
            return;
        }
    }
}

void snake_game_start(void) {
    snake_active = true;

    // Clear input buffer
    input_head = 0;
    input_tail = 0;

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

    uint8_t next_move = snake_pop_input();

    if (next_move != 0) {
        if (next_move == 1 && snake_dir_y == 0) { snake_dir_x = 0; snake_dir_y = -1; }
        else if (next_move == 2 && snake_dir_y == 0) { snake_dir_x = 0; snake_dir_y = 1; }
        else if (next_move == 3 && snake_dir_x == 0) { snake_dir_x = -1; snake_dir_y = 0; }
        else if (next_move == 4 && snake_dir_x == 0) { snake_dir_x = 1; snake_dir_y = 0; }
    }

    SnakePoint new_head = {
        snake_body[0].x + snake_dir_x,
        snake_body[0].y + snake_dir_y
    };

    // Wall collision (die)
    if (new_head.x < 0 || new_head.x >= SNAKE_WIDTH ||
        new_head.y < 0 || new_head.y >= SNAKE_HEIGHT ||
        pgm_read_byte(&GRID_MAP[new_head.y][new_head.x]) == 0xFF) {
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

    switch (keycode) {
        case KC_UP:
            if (record->event.pressed) snake_push_input(1);
            return false;
        case KC_DOWN:
            if (record->event.pressed) snake_push_input(2);
            return false;
        case KC_LEFT:
            if (record->event.pressed) snake_push_input(3);
            return false;
        case KC_RIGHT:
            if (record->event.pressed) snake_push_input(4);
            return false;
        case KC_ESC:
            if (record->event.pressed) snake_game_stop();
            return false;
    }

    if (record->event.pressed) {
        return false;
    }

    return true;
}

// Hook for rgb_matrix_indicators_advanced_user
void snake_game_render(void) {
    if (!snake_active) return;

    rgb_matrix_set_color_all(RGB_OFF);

    // Draw Background
    FOR_EACH_GRID_LED() {
        rgb_matrix_set_color(grid_led_index, SNAKE_COLOR_BG);
    }

    if (snake_game_over) {
        for (uint8_t i = 0; i < snake_len; i++) {
            uint8_t l_idx = grid_get_led(snake_body[i].y, snake_body[i].x);
            if (l_idx != NO_LED) rgb_matrix_set_color(l_idx, SNAKE_COLOR_DEATH);
        }
        return;
    }

    // Draw Food
    uint8_t l_idx = grid_get_led(snake_food.y, snake_food.x);
    if (l_idx != NO_LED) rgb_matrix_set_color(l_idx, SNAKE_COLOR_FOOD);

    // Draw Snake
    for (uint8_t i = 0; i < snake_len; i++) {
        l_idx = grid_get_led(snake_body[i].y, snake_body[i].x);
        if (l_idx != NO_LED) {
            if (i == 0) {
                rgb_matrix_set_color(l_idx, SNAKE_COLOR_HEAD);
            } else {
                rgb_matrix_set_color(l_idx, SNAKE_COLOR_BODY);
            }
        }
    }
}
