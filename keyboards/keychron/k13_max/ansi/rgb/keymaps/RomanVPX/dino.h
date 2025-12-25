#pragma once

#include QMK_KEYBOARD_H  // IWYU pragma: keep

bool dino_is_active(void);
void dino_game_start(void);
void dino_game_task(void);
bool dino_game_process_record(uint16_t keycode, keyrecord_t *record);
void dino_game_render(void);

