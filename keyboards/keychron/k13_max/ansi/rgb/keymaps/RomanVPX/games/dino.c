#include "grid_map.h"
#include "rgb_utils.h"
#include "game_utils.h"

#define DINO_WIDTH GRID_WIDTH
#define DINO_HEIGHT GRID_HEIGHT
#define DINO_SPEED_MS        120
#define DINO_SPEED_MIN       60
#define DINO_SPEED_STEP      5
#define DINO_SPEED_PER       3
#define DINO_GROUND_SPEED_MS 80

// Grid rows (without F_ROW): 0=numbers, 1=QWERTY, 2=ASDF, 3=ZXCV
#define DINO_GROUND_Y   (DINO_HEIGHT - 1)  // Physical Row 4 (Z,X...) - ground
#define DINO_RUN_Y      (DINO_HEIGHT - 2)  // Physical Row 3 (A,S...) - dino runs here
#define DINO_JUMP_Y     (DINO_HEIGHT - 3)  // Physical Row 2 (Q,W...) - jump height
#define DINO_START_X    1                  // Dino's home X position

// Colors
#define DINO_COLOR_DINO         RGB_GREEN
#define DINO_COLOR_OBSTACLE     RGB_RED
#define DINO_COLOR_GROUND_DIM   15, 15, 12
#define DINO_COLOR_GROUND_BRIGHT 35, 35, 28
#define DINO_COLOR_BG           5, 3, 28

// Jump phases
#define JUMP_GROUND     0
#define JUMP_UP         1  // Moving up and right
#define JUMP_PEAK       2  // At peak, moving right
#define JUMP_DOWN       3  // Moving down and right
#define JUMP_LAND       4  // Landing back to start

// Obstacles
#define MAX_OBSTACLES   3
#define OBSTACLE_SPAWN_MIN 4
#define OBSTACLE_SPAWN_MAX 14

typedef struct {
    int8_t x;
    bool active;
} Obstacle;

static bool dino_active = false;
static uint16_t dino_timer = 0;
static uint16_t ground_timer = 0;
static bool dino_game_over = false;
static uint16_t dino_score = 0;

// Dino state
static uint8_t dino_x = DINO_START_X;
static uint8_t dino_y = DINO_RUN_Y;
static uint8_t jump_phase = JUMP_GROUND;

// Obstacles
static Obstacle obstacles[MAX_OBSTACLES];
static uint8_t spawn_counter = 0;
static uint8_t next_spawn = OBSTACLE_SPAWN_MIN;

// Ground animation offset
static uint8_t ground_offset = 0;

// Jump input buffer (queued jumps)
#define JUMP_BUFFER_SIZE 3
static uint8_t jump_buffer = 0;

bool dino_is_active(void) {
    return dino_active;
}

static void spawn_obstacle(void) {
    for (uint8_t i = 0; i < MAX_OBSTACLES; i++) {
        if (!obstacles[i].active) {
            obstacles[i].x = DINO_WIDTH - 1;
            obstacles[i].active = true;
            next_spawn = OBSTACLE_SPAWN_MIN + game_random(OBSTACLE_SPAWN_MAX - OBSTACLE_SPAWN_MIN);
            spawn_counter = 0;
            return;
        }
    }
}

static bool check_collision(void) {
    for (uint8_t i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].active &&
            obstacles[i].x == dino_x &&
            dino_y == DINO_RUN_Y) {
            return true;
        }
    }
    return false;
}

void dino_game_start(void) {
    dino_active = true;
    dino_game_over = false;
    dino_timer = timer_read();
    ground_timer = timer_read();

    // Reset dino position
    dino_x = DINO_START_X;
    dino_y = DINO_RUN_Y;
    jump_phase = JUMP_GROUND;
    jump_buffer = 0;

    // Clear obstacles
    for (uint8_t i = 0; i < MAX_OBSTACLES; i++) {
        obstacles[i].active = false;
    }
    spawn_counter = 0;
    next_spawn = OBSTACLE_SPAWN_MIN;
    ground_offset = 0;
    dino_score = 0;
}

static void dino_game_stop(void) {
    dino_active = false;
}

static void update_jump(void) {
    switch (jump_phase) {
        case JUMP_UP:
            dino_y = DINO_JUMP_Y;
            dino_x = DINO_START_X + 1;
            jump_phase = JUMP_PEAK;
            break;
        case JUMP_PEAK:
            dino_x = DINO_START_X + 2;
            jump_phase = JUMP_DOWN;
            break;
        case JUMP_DOWN:
            dino_y = DINO_RUN_Y;
            dino_x = DINO_START_X + 1;
            jump_phase = JUMP_LAND;
            break;
        case JUMP_LAND:
            dino_x = DINO_START_X;
            jump_phase = JUMP_GROUND;
            break;
    }
}

static void dino_update(void) {
    if (dino_game_over) return;

    // Update jump animation
    if (jump_phase != JUMP_GROUND) {
        update_jump();
    } else if (jump_buffer > 0) {
        // Start buffered jump
        jump_buffer--;
        jump_phase = JUMP_UP;
    }

    // Move obstacles left
    for (uint8_t i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].active) {
            obstacles[i].x--;
            if (obstacles[i].x < 0) {
                obstacles[i].active = false;
                dino_score++;
            }
        }
    }

    // Spawn new obstacles
    spawn_counter++;
    if (spawn_counter >= next_spawn) {
        spawn_obstacle();
    }

    // Check collision
    if (check_collision()) {
        dino_game_over = true;
    }
}

void dino_game_task(void) {
    if (!dino_active) return;

    uint16_t speed = game_speed(dino_score, DINO_SPEED_PER, DINO_SPEED_STEP, DINO_SPEED_MS, DINO_SPEED_MIN);
    uint16_t ground_speed = (uint16_t)DINO_GROUND_SPEED_MS * speed / DINO_SPEED_MS;

    // Update ground animation faster
    if (timer_elapsed(ground_timer) > ground_speed) {
        ground_offset = (ground_offset + 1) % DINO_WIDTH;
        ground_timer = timer_read();
    }

    if (dino_game_over) return;

    if (timer_elapsed(dino_timer) > speed) {
        dino_update();
        dino_timer = timer_read();
    }
}

bool dino_game_process_record(uint16_t keycode, keyrecord_t *record) {
    if (!dino_active) return true;

    switch (keycode) {
        case KC_ESC:
            if (record->event.pressed) dino_game_stop();
            return false;
        case KC_SPC:
            if (record->event.pressed) {
                if (dino_game_over) {
                    // Restart game
                    dino_game_start();
                } else if (jump_phase == JUMP_GROUND) {
                    // Start jump immediately if on ground
                    jump_phase = JUMP_UP;
                } else if (jump_buffer < JUMP_BUFFER_SIZE) {
                    // Queue jump for later
                    jump_buffer++;
                }
            }
            return false;
    }

    if (record->event.pressed) {
        return false;
    }
    return true;
}

void dino_game_render(void) {
    if (!dino_active) return;

    rgb_matrix_set_color_all(RGB_OFF);

    RGB bg = rgb_make(DINO_COLOR_BG);
    RGB ground_dim = rgb_make(DINO_COLOR_GROUND_DIM);
    RGB ground_bright = rgb_make(DINO_COLOR_GROUND_BRIGHT);

    // Draw background for play area
    for (uint8_t y = 0; y < DINO_HEIGHT - 1; y++) {
        for (uint8_t x = 0; x < DINO_WIDTH; x++) {
            uint8_t l_idx = grid_get_led(y, x);
            if (l_idx != NO_LED) {
                rgb_matrix_set_color(l_idx, bg.r, bg.g, bg.b);
            }
        }
    }

    // Draw ground
    for (uint8_t x = 0; x < DINO_WIDTH; x++) {
        uint8_t l_idx = grid_get_led(DINO_GROUND_Y, x);
        if (l_idx != NO_LED) {
            if (dino_game_over) {
                // Red ground on game over
                rgb_matrix_set_color(l_idx, DINO_COLOR_OBSTACLE);
            } else {
                // Moving texture pattern
                uint8_t pattern_x = (x + ground_offset) % 4;
                if (pattern_x == 0) {
                    rgb_matrix_set_color(l_idx, ground_bright.r, ground_bright.g, ground_bright.b);
                } else {
                    rgb_matrix_set_color(l_idx, ground_dim.r, ground_dim.g, ground_dim.b);
                }
            }
        }
    }

    // Draw obstacles (on run row) - not shown on game over, ground is all red
    if (!dino_game_over) {
        for (uint8_t i = 0; i < MAX_OBSTACLES; i++) {
            if (obstacles[i].active && obstacles[i].x >= 0 && obstacles[i].x < DINO_WIDTH) {
                uint8_t l_idx = grid_get_led(DINO_RUN_Y, obstacles[i].x);
                if (l_idx != NO_LED) {
                    rgb_matrix_set_color(l_idx, DINO_COLOR_OBSTACLE);
                }
            }
        }
    }

    // Draw dino
    uint8_t l_idx = grid_get_led(dino_y, dino_x);
    if (l_idx != NO_LED) {
        if (dino_game_over) {
            rgb_matrix_set_color(l_idx, DINO_COLOR_OBSTACLE);
        } else {
            rgb_matrix_set_color(l_idx, DINO_COLOR_DINO);
        }
    }

    render_score_bar(dino_score, dino_game_over);
}
