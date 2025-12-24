#pragma once

#include QMK_KEYBOARD_H  // IWYU pragma: keep

bool snake_is_active(void);
void snake_game_start(void);
void snake_game_task(void);
bool snake_game_process_record(uint16_t keycode, keyrecord_t *record);
void snake_game_render(void);
