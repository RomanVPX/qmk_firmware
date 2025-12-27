#pragma once

#include QMK_KEYBOARD_H  // IWYU pragma: keep

enum custom_keycodes {
    STRNG_FIRST = SAFE_RANGE - 1,
    #define STRNG_X(name, str) name,
    #include "strng_x.inc"
    STRNG_LAST,
    TOGGLE_F_LAYER,
    FN_TAP,
    #define GAME_X(run_keycode, prefix) run_keycode,
    #include "games_x.inc"
};

#define IS_STRING_MACRO(keycode) ((keycode) > STRNG_FIRST && (keycode) < STRNG_LAST)
