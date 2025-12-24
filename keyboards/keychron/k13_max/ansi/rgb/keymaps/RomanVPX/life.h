#pragma once

#include QMK_KEYBOARD_H  // IWYU pragma: keep

bool life_is_active(void);
void life_game_start(void);
void life_game_task(void);
bool life_game_process_record(uint16_t keycode, keyrecord_t *record);
void life_game_render(void);
