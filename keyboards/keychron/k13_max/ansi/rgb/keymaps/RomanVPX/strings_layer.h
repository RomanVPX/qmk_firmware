#pragma once

#include QMK_KEYBOARD_H  // IWYU pragma: keep

bool strings_layer_process_record(uint16_t keycode, keyrecord_t *record, uint8_t fn_layer, uint8_t mac_base_layer, uint8_t strings_layer);
bool strings_layer_rgb_render(uint8_t led_min, uint8_t led_max, uint8_t current_val, uint8_t strings_layer);
