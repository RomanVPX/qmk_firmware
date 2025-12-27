#pragma once

#include QMK_KEYBOARD_H  // IWYU pragma: keep
#include "custom_keycodes.h" // IWYU pragma: keep

// Common interface for all games
typedef struct {
    uint16_t trigger_keycode;
    void (*start)(void);
    bool (*is_active)(void);
    void (*task)(void);
    bool (*process_record)(uint16_t keycode, keyrecord_t *record);
    void (*render)(void);
} GameInterface;


#define GAME_X(run_keycode, prefix) \
bool prefix##_is_active(void); \
void prefix##_game_start(void); \
void prefix##_game_task(void); \
bool prefix##_game_process_record(uint16_t keycode, keyrecord_t *record); \
void prefix##_game_render(void);

#include "games_x.inc"

static const GameInterface games[] = {
#define GAME_X(run_keycode, prefix) \
{\
    .trigger_keycode = run_keycode,\
    .start = prefix##_game_start,\
    .is_active = prefix##_is_active,\
    .task = prefix##_game_task,\
    .process_record = prefix##_game_process_record,\
    .render = prefix##_game_render,\
},
#include "games_x.inc"
};

#define GAMES_COUNT (sizeof(games) / sizeof(games[0]))

// Check if a keycode triggers a game start
static inline bool games_handle_trigger(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) return false;

    for (uint8_t i = 0; i < GAMES_COUNT; ++i) {
        if (keycode == games[i].trigger_keycode) {
            games[i].start();
            return true; // Game started, key handled
        }
    }
    return false;
}

// Process all games' input handling (returns false if any game consumed the key)
static inline bool games_process_record(uint16_t keycode, keyrecord_t *record) {
    for (uint8_t i = 0; i < GAMES_COUNT; ++i) {
        if (!games[i].process_record(keycode, record)) {
            return false;
        }
    }
    return true;
}

// Run all games' periodic tasks
static inline void games_task(void) {
    for (uint8_t i = 0; i < GAMES_COUNT; ++i) {
        games[i].task();
    }
}

// Render active game (returns true if a game was rendered)
static inline bool games_render(void) {
    for (uint8_t i = 0; i < GAMES_COUNT; ++i) {
        if (games[i].is_active()) {
            games[i].render();
            return true;
        }
    }
    return false;
}

