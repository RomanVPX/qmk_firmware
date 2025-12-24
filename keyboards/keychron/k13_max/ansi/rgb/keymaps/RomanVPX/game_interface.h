#pragma once

#include QMK_KEYBOARD_H  // IWYU pragma: keep

// Common interface for all games
typedef struct {
    bool (*is_active)(void);
    void (*task)(void);
    bool (*process_record)(uint16_t keycode, keyrecord_t *record);
    void (*render)(void);
} GameInterface;

// Forward declarations
#include "snake.h"
#include "life.h"

// Registry of all available games
static const GameInterface games[] = {
    {
        .is_active = snake_is_active,
        .task = snake_game_task,
        .process_record = snake_game_process_record,
        .render = snake_game_render,
    },
    {
        .is_active = life_is_active,
        .task = life_game_task,
        .process_record = life_game_process_record,
        .render = life_game_render,
    },
};

#define GAMES_COUNT (sizeof(games) / sizeof(games[0]))

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

