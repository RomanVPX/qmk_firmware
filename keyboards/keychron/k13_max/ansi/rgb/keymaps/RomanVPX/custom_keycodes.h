#pragma once

#include QMK_KEYBOARD_H

// Custom keycodes, including string macros and game controls
enum custom_keycodes {
    STRNG_FIRST = SAFE_RANGE - 1,
    #define STRNG_X(name, str) name,
    #include "strng_x.inc"
    STRNG_LAST,
    TOGGLE_F_LAYER,
    FN_TAP,
    RUN_SNAKE,
    RUN_LIFE,
    RUN_DINO
};

#define IS_STRING_MACRO(keycode) ((keycode) > STRNG_FIRST && (keycode) < STRNG_LAST)
