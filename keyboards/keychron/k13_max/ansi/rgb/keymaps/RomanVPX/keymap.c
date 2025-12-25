/* Copyright 2024 @ Keychron (https://www.keychron.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "keychron_common.h"
#include <string.h>
#include "rgb_utils.h"
#include "matrix_utils.h"
#include "game_interface.h"

enum layers {
    MAC_BASE,
    MAC_FN,
    WIN_BASE,
    WIN_FN,
    MAC_F_LAYER,    // Слой для клавиш F1-F12 в macOS
    STRINGS_LAYER,  // Слой с клавишами STRINGnn
};

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

// Double-tap detection for FN_TAP
#define FN_TAP_TIMEOUT 200
static uint16_t fn_tap_timer = 0;
static bool fn_tap_pending = false;
static bool strings_layer_active = false;
static bool fn_held_in_strings_layer = false;
static bool fn_used_for_combo = false;
static uint16_t fn_strings_timer = 0;

// String animation state
#define ANIMATION_CHAR_DURATION 600
#define ANIMATION_CHAR_FADEOUT 500
static bool animation_active = false;
static const char* animation_string = NULL;
static uint8_t animation_index = 0;
static uint8_t animation_length = 0;
static uint16_t animation_timer = 0;
static uint8_t prev_anim_row = 0, prev_anim_col = 0;
static bool prev_anim_valid = false;
static uint16_t prev_anim_timer = 0;


typedef struct {
    RGB main;
    RGB alt;
    RGB pulsing;
    RGB antiphase;
} Palette;

// Animation cache to improve RGB matrix performance
typedef struct {
    uint8_t row;
    uint8_t col;
    bool valid;
} KeyPosCache;
static KeyPosCache anim_cache[32]; // Max reasonable string length
static uint32_t anim_key_mask[MATRIX_ROWS]; // Bitmask for O(1) key lookup

// Get string for a string macro keycode
static const char* get_string_for_keycode(uint16_t keycode) {
    switch (keycode) {
        #define STRNG_X(name, str) case name: return str;
        #include "strng_x.inc"
    }
    return NULL;
}

static uint16_t char_to_keycode(char c) {
    if (c >= 0 && c < 128) {
        uint8_t keycode = pgm_read_byte(&ascii_to_keycode_lut[(uint8_t)c]);
        return keycode != XXXXXXX ? keycode : KC_NO;
    }
    return KC_NO;
}

// Find matrix position for a keycode by searching the keymap
static bool find_keycode_position(uint16_t target, uint8_t layer, uint8_t* out_row, uint8_t* out_col) {
    FOR_EACH_MATRIX_POS() {
        if (KEYCODE_AT(layer, row, col) == target) {
            *out_row = row;
            *out_col = col;
            return true;
        }
    }
    return false;
}

// Get matrix position for a character (searches MAC_BASE layer)
static bool get_matrix_position_for_char(char c, uint8_t* row, uint8_t* col) {
    uint16_t kc = char_to_keycode(c);
    if (kc == KC_NO) return false;
    return find_keycode_position(kc, MAC_BASE, row, col);
}

// Animation control functions
static void stop_animation(void) {
    animation_active = false;
    animation_string = NULL;
    animation_index = 0;
    animation_length = 0;
    prev_anim_valid = false;
}

static void start_animation(uint16_t keycode) {
    const char* str = get_string_for_keycode(keycode);
    if (str == NULL) return;

    animation_string = str;
    animation_length = strlen(str);
    if (animation_length > 32) animation_length = 32;

    // Pre-calculate positions and build bitmask
    memset(anim_key_mask, 0, sizeof(anim_key_mask));
    for (uint8_t i = 0; i < animation_length; i++) {
        uint8_t r, c;
        if (get_matrix_position_for_char(str[i], &r, &c)) {
            anim_cache[i].row = r;
            anim_cache[i].col = c;
            anim_cache[i].valid = true;
            anim_key_mask[r] |= (1UL << c);
        } else {
            anim_cache[i].valid = false;
        }
    }

    animation_index = 0;
    animation_timer = timer_read();
    animation_active = true;
    prev_anim_valid = false;
}

// Check if a key position is part of the animation string (O(1) via bitmask)
static inline bool is_key_in_animation_string(uint8_t row, uint8_t col) {
    return animation_active && (anim_key_mask[row] & (1UL << col));
}


/*| docs/feature_rgb_matrix.md
 *|Key                |Aliases   |Description                                                                           |
 *|-------------------|----------|--------------------------------------------------------------------------------------|
 *|`RGB_TOG`          |          |Toggle RGB lighting on or off                                                         |
 *|`RGB_MODE_FORWARD` |`RGB_MOD` |Cycle through modes, reverse direction when Shift is held                             |
 *|`RGB_MODE_REVERSE` |`RGB_RMOD`|Cycle through modes in reverse, forward direction when Shift is held                  |
 *|`RGB_HUI`          |          |Increase hue, decrease hue when Shift is held                                         |
 *|`RGB_HUD`          |          |Decrease hue, increase hue when Shift is held                                         |
 *|`RGB_SAI`          |          |Increase saturation, decrease saturation when Shift is held                           |
 *|`RGB_SAD`          |          |Decrease saturation, increase saturation when Shift is held                           |
 *|`RGB_VAI`          |          |Increase value (brightness), decrease value when Shift is held                        |
 *|`RGB_VAD`          |          |Decrease value (brightness), increase value when Shift is held                        |
 *|`RGB_SPI`          |          |Increase effect speed (does not support eeprom yet), decrease speed when Shift is held|
 *|`RGB_SPD`          |          |Decrease effect speed (does not support eeprom yet), increase speed when Shift is held|
 *|`QK_BOOTLOADER`    |`QK_BOOT` |Put the keyboard into bootloader mode for flashing                                    |
 *|`KC_NO`            |'XXXXXXX' |Ignore this key                                                                       |
 *|-------------------|----------|--------------------------------------------------------------------------------------|
 */

#define HYP_C       HYPR(KC_C)
#define HYP_N       HYPR(KC_N)
#define HYP_T       HYPR(KC_T)
#define HYP_P1      HYPR(KC_P1)
#define HYP_P3      HYPR(KC_P3)
#define HYP_LEFT    HYPR(KC_LEFT)
#define HYP_RGHT    HYPR(KC_RGHT)
#define HYP_DOWN    HYPR(KC_DOWN)

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [MAC_BASE] = LAYOUT_ansi_90(
        KC_ESC,             KC_BRID,  KC_BRIU,  KC_MCTRL, KC_LNPAD, RGB_VAD,  RGB_VAI,  KC_MPRV,    KC_MPLY,     KC_MNXT,  KC_MUTE,  KC_VOLD,   KC_VOLU, HYP_C,    HYP_N,    HYP_T,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,       KC_9,        KC_0,     KC_MINS,  KC_EQL,    KC_BSPC, STRNG_C,  STRNG_R,  SCMD(KC_P),
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,       KC_O,        KC_P,     KC_LBRC,  KC_RBRC,   KC_BSLS, KC_INS,   KC_HOME,  KC_PGUP,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,       KC_L,        KC_SCLN,  KC_QUOT,             KC_ENT,  KC_DEL,   KC_END,   KC_PGDN,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,       KC_COMM,     KC_DOT,   KC_SLSH,             KC_RSFT,           KC_UP,
        KC_LCTL,  KC_LOPTN, KC_LCMMD,                               KC_SPC,                                      KC_RCMMD, KC_ROPTN, FN_TAP,    KC_RCTL, KC_LEFT,  KC_DOWN,  KC_RGHT),

    [MAC_FN] = LAYOUT_ansi_90(
        _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,     _______,  _______,  _______,   _______, XXXXXXX,  XXXXXXX,  XXXXXXX,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  P2P4G,    _______,  _______,  _______,  _______,    _______,     _______,  _______,  _______,   _______, RUN_SNAKE,RUN_LIFE, RUN_DINO,
        RGB_TOG,  RGB_MOD,  _______,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,    _______,     _______,  _______,  _______,   _______, _______,  _______,  _______,
        QK_BOOT,  RGB_RMOD, _______,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______, TOGGLE_F_LAYER, _______,  _______,             _______,  HYP_P1,  _______,  HYP_P3,
        _______,            _______,  _______,  _______,  _______,  BAT_LVL,  NK_TOGG,  _______,    _______,     _______,  _______,             _______,           _______,
        _______,  _______,  _______,                                _______,                                     _______,  _______,  _______,   _______, HYP_LEFT, HYP_DOWN, HYP_RGHT),

    [WIN_BASE] = LAYOUT_ansi_90(
        KC_ESC,             KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,      KC_F8,       KC_F9,    KC_F10,   KC_F11,    KC_F12,   XXXXXXX, XXXXXXX,  XXXXXXX,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,       KC_9,        KC_0,     KC_MINS,  KC_EQL,    KC_BSPC,  XXXXXXX, XXXXXXX,  XXXXXXX,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,       KC_O,        KC_P,     KC_LBRC,  KC_RBRC,   KC_BSLS,  KC_INS,  KC_HOME,  KC_PGUP,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,       KC_L,        KC_SCLN,  KC_QUOT,             KC_ENT,   KC_DEL,  KC_END,   KC_PGDN,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,       KC_COMM,     KC_DOT,   KC_SLSH,             KC_RSFT,           KC_UP,
        KC_LCTL,  KC_LWIN,  KC_LALT,                                KC_SPC,                                      KC_RALT,  KC_RGUI, MO(WIN_FN), KC_RCTL,  KC_LEFT, KC_DOWN,  KC_RGHT),

    [WIN_FN] = LAYOUT_ansi_90(
        _______,            KC_BRID,  KC_BRIU,  KC_TASK,  KC_FILE,  RGB_VAD,  RGB_VAI,  KC_MPRV,    KC_MPLY,     KC_MNXT,  KC_MUTE,  KC_VOLD,   KC_VOLU,  _______,  _______, _______,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  P2P4G,    _______,  _______,  _______,  _______,    _______,     _______,  _______,  _______,   _______,  _______,  _______, _______,
        RGB_TOG,  RGB_MOD,  _______,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,    _______,     _______,  _______,  _______,   _______,  _______,  _______, _______,
        QK_BOOT,  RGB_RMOD, _______,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______,    _______,     _______,  _______,             _______,  KC_NUM,   _______, _______,
        _______,            _______,  _______,  _______,  _______,  BAT_LVL,  NK_TOGG,  _______,    _______,     _______,  _______,             _______,            _______,
        _______,  _______,  _______,                                _______,                                     _______,  _______,  _______,   _______,  _______,  _______, _______),

    // MAC_F_LAYER - прозрачный, кроме F1-F12
    [MAC_F_LAYER] = LAYOUT_ansi_90(
        _______,             KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,      KC_F8,       KC_F9,    KC_F10,   KC_F11,   KC_F12,   _______,  _______, _______,
        _______,  _______,   _______,  _______, _______,  _______,   _______, _______,   _______,    _______,     _______,  _______,  _______,  _______,  _______,  _______, _______,
        _______,  _______,   _______,  _______, _______,  _______,   _______, _______,   _______,    _______,     _______,  _______,  _______,  _______,  _______,  _______, _______,
        _______,  _______,   _______,  _______, _______,  _______,   _______, _______,   _______,    _______,     _______,  _______,            _______,  _______,  _______, _______,
        _______,             _______,  _______, _______,  _______,   _______, _______,   _______,    _______,     _______,  _______,            _______,            _______,
        _______,  _______,   _______,                                _______,                                     _______,  _______,  _______,  _______,  _______,  _______, _______),

    [STRINGS_LAYER] = LAYOUT_ansi_90(
        XXXXXXX,             XXXXXXX,  XXXXXXX, XXXXXXX,  XXXXXXX,   XXXXXXX, XXXXXXX,   XXXXXXX,    XXXXXXX,     XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX,
        XXXXXXX,  XXXXXXX,   XXXXXXX,  XXXXXXX, XXXXXXX,  XXXXXXX,   XXXXXXX, XXXXXXX,   XXXXXXX,    XXXXXXX,     XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX,
        XXXXXXX,  XXXXXXX,   XXXXXXX,  XXXXXXX, STRNG_R,  XXXXXXX,   XXXXXXX, XXXXXXX,   XXXXXXX,    XXXXXXX,     XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX,
        XXXXXXX,  XXXXXXX,   STRNG_S,  XXXXXXX, XXXXXXX,  XXXXXXX,   XXXXXXX, XXXXXXX,   XXXXXXX,    XXXXXXX,     XXXXXXX,  XXXXXXX,            XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX,
        XXXXXXX,             XXXXXXX,  XXXXXXX, STRNG_C,  XXXXXXX,   XXXXXXX, XXXXXXX,   STRNG_M,    XXXXXXX,     XXXXXXX,  XXXXXXX,            XXXXXXX,            XXXXXXX,
        XXXXXXX,  XXXXXXX,   XXXXXXX,                                XXXXXXX,                                     XXXXXXX,  XXXXXXX,  _______,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX)
};
// clang-format on

#define IS_F_KEYCODE(keycode) ((keycode) >= KC_F1 && (keycode) <= KC_F12)
#define IS_STRING_MACRO(keycode) ((keycode) > STRNG_FIRST && (keycode) < STRNG_LAST)

// Вызывается при каждом изменении состояния слоев
layer_state_t layer_state_set_user(layer_state_t state) {
    // Проверяем активацию/деактивацию MAC_FN
    if ((IS_LAYER_ON_STATE(state, MAC_FN) && !layer_state_is(MAC_FN)) ||
        (!IS_LAYER_ON_STATE(state, MAC_FN) && layer_state_is(MAC_FN))) {
        // MAC_FN активируется или деактивируется - инвертируем MAC_F_LAYER
        state = state ^ (1UL << MAC_F_LAYER);
    }

    // Проверяем переключение на слой WIN_BASE или WIN_FN
    if (IS_LAYER_ON_STATE(state, WIN_BASE) || IS_LAYER_ON_STATE(state, WIN_FN)) {
        // Если переключились в режим Windows, отключаем MAC_F_LAYER
        state = state & ~(1UL << MAC_F_LAYER);
    }

    return state;
}

// Функция для проверки, изменяется ли функция клавиши при переключении слоя
bool is_key_modified_in_layer(uint8_t row, uint8_t col, uint8_t base_layer, uint8_t target_layer) {
    uint16_t base_keycode = keymap_key_to_keycode(base_layer, (keypos_t){col, row});
    uint16_t target_keycode = keymap_key_to_keycode(target_layer, (keypos_t){col, row});

    return (base_keycode != target_keycode) && (target_keycode != KC_TRNS) && (target_keycode != KC_NO);
}

static inline void handle_mac_lighting(uint8_t row, uint8_t col, uint8_t index, const Palette* palette, bool is_mac_fn, bool is_mac_f_layer) {
    uint16_t f_layer_keycode = keymap_key_to_keycode(MAC_F_LAYER, (keypos_t){col, row});
    uint16_t fn_keycode      = keymap_key_to_keycode(MAC_FN, (keypos_t){col, row});

    if (IS_F_KEYCODE(f_layer_keycode)) { // Это F-клавиша?
        if (is_mac_f_layer) {
            if (is_mac_fn) { // Изначально ВЫКЛ, Fn зажата
                rgb_matrix_set_color(index, palette->pulsing.r, palette->pulsing.g, palette->pulsing.b);
            } else { // Изначально ВКЛ, Fn НЕ зажата
                rgb_matrix_set_color(index, palette->alt.r, palette->alt.g, palette->alt.b);
            }
        } else if (is_mac_fn) { // Изначально ВКЛ, Fn зажата
            rgb_matrix_set_color(index, palette->antiphase.r, palette->antiphase.g, palette->antiphase.b);
        }
        return; // F-клавиша обработана, дальше не идем
    }

    if (is_mac_fn && fn_keycode == TOGGLE_F_LAYER) { // Это TOGGLE_F_LAYER?
        rgb_matrix_set_color(index, palette->pulsing.r, palette->pulsing.g, palette->pulsing.b);
        return; // Клавиша обработана
    }

    if (is_mac_fn && is_key_modified_in_layer(row, col, MAC_BASE, MAC_FN)) { // Это другая измененная клавиша на MAC_FN?
        rgb_matrix_set_color(index, palette->main.r, palette->main.g, palette->main.b);
    }
}

static inline void handle_win_lighting(uint8_t row, uint8_t col, uint8_t index, const RGB* static_color) {
    if (layer_state_is(WIN_FN) && is_key_modified_in_layer(row, col, WIN_BASE, WIN_FN)) {
        rgb_matrix_set_color(index, static_color->r, static_color->g, static_color->b);
    }
}

#define INDICATOR_MAX_VALUE RGB_MATRIX_MAXIMUM_BRIGHTNESS
#ifndef INDICATOR_MAX_VALUE
    #define INDICATOR_MAX_VALUE rgb_matrix_get_val()
#endif

#define MAIN_COLOR_HSV                  (HSV){HSV_MAGENTA}
#define SECONDARY_COLOR_HSV             (HSV){HSV_CYAN}

#define STRINGS_LAYER_COLOR_HSV         (HSV){HSV_GOLDENROD}
#define STRINGS_ANIMATION_COLOR_HSV     (HSV){HSV_SPRINGGREEN}
#define STRINGS_LAYER_PREVIEW_COLOR_HSV (HSV){HSV_WHITE}
#define STRINGS_LAYER_PREVIEW_DIV       2

static bool rgb_render_strings_layer(uint8_t led_min, uint8_t led_max, uint8_t current_val) {
    PulsingConfig pulsing_cfg = PULSING_CONFIG_DEFAULT;

    // Colors with effective saturation
    HSV hsv_strings = STRINGS_LAYER_COLOR_HSV;
    hsv_strings.v = current_val;
    RGB rgb_strings = hsv_to_rgb_effective(hsv_strings);
    RGB rgb_strings_pulsing = rgb_pulsing(hsv_strings, current_val, pulsing_cfg);

    // Animation color
    HSV hsv_anim = STRINGS_ANIMATION_COLOR_HSV;
    hsv_anim.v = current_val;
    RGB rgb_anim = hsv_to_rgb_effective(hsv_anim);

    HSV hsv_preview = STRINGS_LAYER_PREVIEW_COLOR_HSV;
    hsv_preview.v = current_val / STRINGS_LAYER_PREVIEW_DIV;
    RGB rgb_anim_preview = hsv_to_rgb_effective(hsv_preview);

    // Update animation state using cached positions
    uint8_t anim_row = 0, anim_col = 0;
    bool anim_key_found = false;
    if (animation_active) {
        // Get current character position from cache
        if (anim_cache[animation_index].valid) {
            anim_row = anim_cache[animation_index].row;
            anim_col = anim_cache[animation_index].col;
            anim_key_found = true;
        }
        // Check if we need to advance to next character
        if (timer_elapsed(animation_timer) >= ANIMATION_CHAR_DURATION) {
            // Save current position as previous for fadeout
            if (anim_key_found) {
                prev_anim_row = anim_row;
                prev_anim_col = anim_col;
                prev_anim_valid = true;
                prev_anim_timer = timer_read();
            }
            animation_index++;
            animation_timer = timer_read();
            // Check if animation finished
            if (animation_index >= animation_length) {
                stop_animation();
                anim_key_found = false;
            } else {
                // Update to new character position from cache
                if (anim_cache[animation_index].valid) {
                    anim_row = anim_cache[animation_index].row;
                    anim_col = anim_cache[animation_index].col;
                    anim_key_found = true;
                } else {
                    anim_key_found = false;
                }
            }
        }
    }

    // Calculate transition progress for previous key (0 = anim color, 255 = target color)
    uint8_t transition_progress = 255;
    if (prev_anim_valid) {
        uint16_t elapsed = timer_elapsed(prev_anim_timer);
        if (elapsed >= ANIMATION_CHAR_FADEOUT) {
            prev_anim_valid = false;
        } else {
            transition_progress = (elapsed * 255) / ANIMATION_CHAR_FADEOUT;
        }
    }

    RGB blend_active = rgb_lerp(rgb_anim, rgb_strings, transition_progress);
    RGB blend_preview = rgb_lerp(rgb_anim, rgb_anim_preview, transition_progress);
    RGB blend_off = rgb_lerp(rgb_anim, (RGB){0,0,0}, transition_progress);

    // Render lighting
    FOR_EACH_LED_IN_RANGE(led_min, led_max) {
        uint16_t keycode = KEYCODE_AT(STRINGS_LAYER, row, col);
        bool is_active_key = IS_STRING_MACRO(keycode);

        // Current animation key - full brightness green
        if (anim_key_found && row == anim_row && col == anim_col) {
            rgb_matrix_set_color(led_index, rgb_anim.r, rgb_anim.g, rgb_anim.b);
        }
        // Previous key - transition from anim color to target color
        else if (prev_anim_valid && row == prev_anim_row && col == prev_anim_col) {
            if (is_active_key) {
                rgb_matrix_set_color(led_index, blend_active.r, blend_active.g, blend_active.b);
            } else if (is_key_in_animation_string(row, col)) {
                rgb_matrix_set_color(led_index, blend_preview.r, blend_preview.g, blend_preview.b);
            } else {
                rgb_matrix_set_color(led_index, blend_off.r, blend_off.g, blend_off.b);
            }
        }
        // Keys in animation string - preview
        else if (animation_active && is_key_in_animation_string(row, col)) {
            rgb_matrix_set_color(led_index, rgb_anim_preview.r, rgb_anim_preview.g, rgb_anim_preview.b);
        }
        // Active keys with Fn held - pulsing
        else if (fn_held_in_strings_layer && is_active_key) {
            rgb_matrix_set_color(led_index, rgb_strings_pulsing.r, rgb_strings_pulsing.g, rgb_strings_pulsing.b);
        }
        // Normal active keys
        else if (is_active_key) {
            rgb_matrix_set_color(led_index, rgb_strings.r, rgb_strings.g, rgb_strings.b);
        }
        // Other keys - no highlight
    }
    return false;
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (games_render()) {
        return false;
    }
    uint8_t current_val = INDICATOR_MAX_VALUE;
    if (current_val == 0) return false;

    // STRINGS_LAYER has priority
    if (strings_layer_active) {
        return rgb_render_strings_layer(led_min, led_max, current_val);
    }

    uint8_t base_layer;
    if (layer_state_is(MAC_FN) || layer_state_is(WIN_FN)) { // Если Fn зажата, используем работающий хак:
        base_layer = layer_state & (1UL << MAC_FN) ? MAC_BASE : WIN_BASE; // base_layer = layer_state_is(MAC_FN) ? MAC_BASE : WIN_BASE;
    } else { // Если Fn НЕ зажата, используем стандартный метод:
        base_layer = IS_LAYER_ON(WIN_BASE) ? WIN_BASE : MAC_BASE;
    }
    // --- Анимация и цвета ---
    PulsingConfig pulsing_cfg = PULSING_CONFIG_DEFAULT;

    HSV hsv_static_main = MAIN_COLOR_HSV;
    hsv_static_main.v = current_val;
    RGB rgb_static_main = hsv_to_rgb_effective(hsv_static_main);

    HSV hsv_static_alt = SECONDARY_COLOR_HSV;
    hsv_static_alt.v = current_val;
    RGB rgb_static_alt = hsv_to_rgb_effective(hsv_static_alt);
    RGB rgb_pulsing_alt = rgb_pulsing(hsv_static_alt, current_val, pulsing_cfg);
    RGB rgb_antiphase_pulsing_alt = rgb_pulsing_antiphase(hsv_static_alt, current_val, pulsing_cfg);

    bool is_mac_fn = layer_state_is(MAC_FN);
    bool is_mac_f_layer = layer_state_is(MAC_F_LAYER);

    Palette palette = {
        .main = rgb_static_main,
        .alt = rgb_static_alt,
        .pulsing = rgb_pulsing_alt,
        .antiphase = rgb_antiphase_pulsing_alt,
    };

    FOR_EACH_LED_IN_RANGE(led_min, led_max) {
        if (base_layer == MAC_BASE) {
            handle_mac_lighting(row, col, led_index, &palette, is_mac_fn, is_mac_f_layer);
        } else {
            handle_win_lighting(row, col, led_index, &rgb_static_main);
        }
    }
    return false;
}

// Deactivates STRINGS_LAYER and resets all related state
static void deactivate_strings_layer(void) {
    if (strings_layer_active) {
        strings_layer_active = false;
        fn_held_in_strings_layer = false;
        fn_used_for_combo = false;
        stop_animation();
        layer_off(STRINGS_LAYER);
    }
}

// Activates STRINGS_LAYER
static void activate_strings_layer(void) {
    strings_layer_active = true;
    layer_on(STRINGS_LAYER);
}

// Process FN_TAP logic
static bool process_fn_tap_logic(keyrecord_t *record, uint8_t fn_layer) {
    if (record->event.pressed) {
        // In STRINGS_LAYER: Fn press starts hold detection
        if (strings_layer_active) {
            fn_held_in_strings_layer = true;
            fn_used_for_combo = false;
            fn_strings_timer = timer_read();
            return false;
        }
        // Check for double-tap to activate STRINGS_LAYER
        if (fn_tap_pending && timer_elapsed(fn_tap_timer) < FN_TAP_TIMEOUT) {
            fn_tap_pending = false;
            activate_strings_layer();
            return false;
        }
        // Normal hold — activate Fn layer
        layer_on(fn_layer);
        fn_tap_timer = timer_read();
    } else {
        // Release in STRINGS_LAYER
        if (strings_layer_active && fn_held_in_strings_layer) {
            // Tap (quick release without combo) = exit layer
            if (!fn_used_for_combo && timer_elapsed(fn_strings_timer) < FN_TAP_TIMEOUT) {
                deactivate_strings_layer();
            }
            fn_held_in_strings_layer = false;
            fn_used_for_combo = false;
            return false;
        }
        // Normal release
        fn_held_in_strings_layer = false;
        layer_off(fn_layer);
        // Mark as pending tap if it was a quick tap
        if (timer_elapsed(fn_tap_timer) < FN_TAP_TIMEOUT) {
            fn_tap_pending = true;
            fn_tap_timer = timer_read();
        }
    }
    return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!games_process_record(keycode, record)) {
        return false;
    }
    if (!process_record_keychron_common(keycode, record)) {
        return false;
    }

    bool is_mac = !IS_LAYER_ON(WIN_BASE);
    uint8_t fn_layer = is_mac ? MAC_FN : WIN_FN;
    bool is_string_macro = IS_STRING_MACRO(keycode);

    // Handle FN_TAP
    if (keycode == FN_TAP) {
        return process_fn_tap_logic(record, fn_layer);
    }

    // Reset fn_tap_pending on any other key press
    if (record->event.pressed && fn_tap_pending) {
        fn_tap_pending = false;
    }

    // Handle keys while STRINGS_LAYER is active
    if (strings_layer_active && record->event.pressed) {
        if (animation_active) {
            stop_animation();
        }

        // Fn + string macro = start new animation
        if (fn_held_in_strings_layer && is_string_macro) {
            fn_used_for_combo = true;
            start_animation(keycode);
            return false;
        }

        // Fn + empty = exit layer
        if (fn_held_in_strings_layer && !is_string_macro) {
            fn_used_for_combo = true;
            deactivate_strings_layer();
            return false;
        }

        // String macro without Fn = execute and exit
        if (is_string_macro) {
            deactivate_strings_layer();
            // Fall through to execute macro below
        } else {
            // Empty key without Fn = just exit layer
            deactivate_strings_layer();
            return false;
        }
    }

    switch (keycode) {
        case TOGGLE_F_LAYER:
            if (record->event.pressed) {
                if (layer_state_is(MAC_F_LAYER)) {
                    layer_off(MAC_F_LAYER);
                } else {
                    layer_on(MAC_F_LAYER);
                }
            } return false;

        case RUN_SNAKE:
            if (record->event.pressed) {
                snake_game_start();
            }
            return false;

        case RUN_LIFE:
            if (record->event.pressed) {
                life_game_start();
            }
            return false;

        case RUN_DINO:
            if (record->event.pressed) {
                dino_game_start();
            }
            return false;

        #define STRNG_X(name, str) case name: if (record->event.pressed) { SEND_STRING(str); } return false;
        #include "strng_x.inc"
    }

    return true;
}

void matrix_scan_user(void) {
    games_task();
}
