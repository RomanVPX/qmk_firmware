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
#include <lib/lib8tion/lib8tion.h>

enum layers {
    MAC_BASE,
    MAC_FN,
    WIN_BASE,
    WIN_FN,
    MAC_F_LAYER,    // Слой для клавиш F1-F12 в macOS
    STRINGS_LAYER,  // Слой с клавишами STRINGnn
};

enum custom_keycodes {
    STRNG_C = SAFE_RANGE,
    STRNG_R,
    STRNG_S,
    TOGGLE_F_LAYER,
    FN_TAP,  // Custom Fn key with double-tap support
};

// Double-tap detection for FN_TAP
#define FN_TAP_TIMEOUT 200
static uint16_t fn_tap_timer = 0;
static bool fn_tap_pending = false;
static bool strings_layer_active = false;


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
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  P2P4G,    _______,  _______,  _______,  _______,    _______,     _______,  _______,  _______,   _______, _______,  _______, _______,
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

    // STRINGS_LAYER - выключает все клавиши, кроме тех, на которых есть STRINGnn
    [STRINGS_LAYER] = LAYOUT_ansi_90(
        XXXXXXX,             XXXXXXX,  XXXXXXX, XXXXXXX,  XXXXXXX,   XXXXXXX, XXXXXXX,   XXXXXXX,    XXXXXXX,     XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX,
        XXXXXXX,  XXXXXXX,   XXXXXXX,  XXXXXXX, XXXXXXX,  XXXXXXX,   XXXXXXX, XXXXXXX,   XXXXXXX,    XXXXXXX,     XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX,
        XXXXXXX,  XXXXXXX,   XXXXXXX,  XXXXXXX, STRNG_R,  XXXXXXX,   XXXXXXX, XXXXXXX,   XXXXXXX,    XXXXXXX,     XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX,
        XXXXXXX,  XXXXXXX,   STRNG_S,  XXXXXXX, XXXXXXX,  XXXXXXX,   XXXXXXX, XXXXXXX,   XXXXXXX,    XXXXXXX,     XXXXXXX,  XXXXXXX,            XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX,
        XXXXXXX,             XXXXXXX,  XXXXXXX, STRNG_C,  XXXXXXX,   XXXXXXX, XXXXXXX,   XXXXXXX,    XXXXXXX,     XXXXXXX,  XXXXXXX,            XXXXXXX,            XXXXXXX,
        XXXXXXX,  XXXXXXX,   XXXXXXX,                                XXXXXXX,                                     XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX)
};
// clang-format on

#define IS_F_KEYCODE(keycode) ((keycode) >= KC_F1 && (keycode) <= KC_F12)
#define IS_STRING_MACRO(keycode) ((keycode) >= STRNG_C && (keycode) <= STRNG_S)

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

// Функция для подсветки F-клавиш указанным цветом
void highlight_f_keys(uint8_t led_min, uint8_t led_max, uint8_t r, uint8_t g, uint8_t b) {
    for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
        for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
            uint16_t keycode = keymap_key_to_keycode(MAC_F_LAYER, (keypos_t){col, row});
            if (IS_F_KEYCODE(keycode)) {
                uint8_t index = g_led_config.matrix_co[row][col];
                if (index != NO_LED && index >= led_min && index < led_max) {
                    rgb_matrix_set_color(index, r, g, b);
                }
            }
        }
    }
}

static inline void handle_mac_lighting(uint8_t row, uint8_t col, uint8_t index, const RGB* static_color_main, const RGB* static_color_alt, const RGB* pulsing_color, const RGB* antiphase_color) {
    uint16_t f_layer_keycode = keymap_key_to_keycode(MAC_F_LAYER, (keypos_t){col, row});
    uint16_t fn_keycode      = keymap_key_to_keycode(MAC_FN, (keypos_t){col, row});

    if (IS_F_KEYCODE(f_layer_keycode)) { // Это F-клавиша?
        if (layer_state_is(MAC_F_LAYER)) {
            if (layer_state_is(MAC_FN)) { // Изначально ВЫКЛ, Fn зажата
                rgb_matrix_set_color(index, pulsing_color->r, pulsing_color->g, pulsing_color->b);
            } else { // Изначально ВКЛ, Fn НЕ зажата
                rgb_matrix_set_color(index, static_color_alt->r, static_color_alt->g, static_color_alt->b);
            }
        } else if (layer_state_is(MAC_FN)) { // Изначально ВКЛ, Fn зажата
            rgb_matrix_set_color(index, antiphase_color->r, antiphase_color->g, antiphase_color->b);
        }
        return; // F-клавиша обработана, дальше не идем
    }

    if (layer_state_is(MAC_FN) && fn_keycode == TOGGLE_F_LAYER) { // Это TOGGLE_F_LAYER?
        rgb_matrix_set_color(index, pulsing_color->r, pulsing_color->g, pulsing_color->b);
        return; // Клавиша обработана
    }

    if (layer_state_is(MAC_FN) && is_key_modified_in_layer(row, col, MAC_BASE, MAC_FN)) { // Это другая измененная клавиша на MAC_FN?
        rgb_matrix_set_color(index, static_color_main->r, static_color_main->g, static_color_main->b);
    }
}

static inline void handle_win_lighting(uint8_t row, uint8_t col, uint8_t index, const RGB* static_color) {
    if (layer_state_is(WIN_FN) && is_key_modified_in_layer(row, col, WIN_BASE, WIN_FN)) {
        rgb_matrix_set_color(index, static_color->r, static_color->g, static_color->b);
    }
}

// Handles STRINGS_LAYER lighting: string macro keys are highlighted, others are off
static inline void handle_strings_layer_lighting(uint8_t row, uint8_t col, uint8_t index, const RGB* highlight_color) {
    uint16_t keycode = keymap_key_to_keycode(STRINGS_LAYER, (keypos_t){col, row});
    if (IS_STRING_MACRO(keycode)) {
        rgb_matrix_set_color(index, highlight_color->r, highlight_color->g, highlight_color->b);
    }
}

static inline uint8_t get_effective_sat(uint8_t sat) {
    return scale8(sat, rgb_matrix_get_sat());
}

#define INDICATOR_MAX_VALUE RGB_MATRIX_MAXIMUM_BRIGHTNESS
#ifndef INDICATOR_MAX_VALUE
    #define INDICATOR_MAX_VALUE rgb_matrix_get_val()
#endif

#define MAIN_COLOR_HSV          (HSV){HSV_MAGENTA}
#define SECONDARY_COLOR_HSV     (HSV){HSV_CYAN}
#define STRINGS_LAYER_COLOR_HSV (HSV){HSV_WHITE}

#define PULSING_SPEED_DIV 2

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    uint8_t current_val = INDICATOR_MAX_VALUE;
    if (current_val == 0) return false;

    // STRINGS_LAYER has priority — show only string macro keys in yellow
    if (strings_layer_active) {
        HSV hsv_strings = STRINGS_LAYER_COLOR_HSV;
        uint8_t strings_effective_sat = get_effective_sat(hsv_strings.s);
        RGB rgb_strings = hsv_to_rgb((HSV){hsv_strings.h, strings_effective_sat, current_val});
        for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
            for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
                uint8_t index = g_led_config.matrix_co[row][col];
                if (index == NO_LED || index < led_min || index >= led_max) continue;
                handle_strings_layer_lighting(row, col, index, &rgb_strings);
            }
        }
        return false;
    }

    uint8_t base_layer;
    if (layer_state_is(MAC_FN) || layer_state_is(WIN_FN)) { // Если Fn зажата, используем работающий хак:
        base_layer = layer_state & (1UL << MAC_FN) ? MAC_BASE : WIN_BASE; // base_layer = layer_state_is(MAC_FN) ? MAC_BASE : WIN_BASE;
    } else { // Если Fn НЕ зажата, используем стандартный метод:
        base_layer = IS_LAYER_ON(WIN_BASE) ? WIN_BASE : MAC_BASE;
    }
    // --- Анимация и цвета ---
    uint8_t sin_wave = sin8(timer_read() >> PULSING_SPEED_DIV); // timer_read() возвращает миллисекунды. Сдвиг вправо замедляет анимацию.
    uint8_t antiphase_sin_wave = 255 - sin_wave;
    uint8_t max_v = current_val;
    uint8_t min_v = max_v / 3; // Минимум — треть от максимума яркости
    uint8_t pulsing_val = min_v + scale8(sin_wave, max_v - min_v); // scale8 — быстрая 8-битная функция умножения (a * b) / 255.
    uint8_t antiphase_pulsing_val = min_v + scale8(antiphase_sin_wave, max_v - min_v);

    HSV hsv_static_main = MAIN_COLOR_HSV;
    uint8_t main_effective_sat = get_effective_sat(hsv_static_main.s);
    RGB rgb_static_main = hsv_to_rgb((HSV){hsv_static_main.h, main_effective_sat, current_val});

    HSV hsv_static_alt = SECONDARY_COLOR_HSV;
    uint8_t secondary_effective_sat = get_effective_sat(hsv_static_alt.s);
    RGB rgb_static_alt = hsv_to_rgb((HSV){hsv_static_alt.h, secondary_effective_sat, current_val});
    RGB rgb_pulsing_alt = hsv_to_rgb((HSV){hsv_static_alt.h, secondary_effective_sat, pulsing_val});
    RGB rgb_antiphase_pulsing_alt = hsv_to_rgb((HSV){hsv_static_alt.h, secondary_effective_sat, antiphase_pulsing_val});

    for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
        for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
            uint8_t index = g_led_config.matrix_co[row][col];
            if (index == NO_LED || index < led_min || index >= led_max) { continue; }
            if (base_layer == MAC_BASE) {
                handle_mac_lighting(row, col, index, &rgb_static_main, &rgb_static_alt, &rgb_pulsing_alt, &rgb_antiphase_pulsing_alt);
            } else {
                handle_win_lighting(row, col, index, &rgb_static_main);
            }
        }
    }
    return false;
}

// Deactivates STRINGS_LAYER and resets state
static void deactivate_strings_layer(void) {
    if (strings_layer_active) {
        strings_layer_active = false;
        layer_off(STRINGS_LAYER);
    }
}

// Activates STRINGS_LAYER
static void activate_strings_layer(void) {
    strings_layer_active = true;
    layer_on(STRINGS_LAYER);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_keychron_common(keycode, record)) {
        return false;
    }

    // Handle FN_TAP: hold = MO(MAC_FN/WIN_FN), double-tap = STRINGS_LAYER
    if (keycode == FN_TAP) {
        bool is_mac = !IS_LAYER_ON(WIN_BASE);
        uint8_t fn_layer = is_mac ? MAC_FN : WIN_FN;

        if (record->event.pressed) {
            // Check for double-tap
            if (fn_tap_pending && timer_elapsed(fn_tap_timer) < FN_TAP_TIMEOUT) {
                fn_tap_pending = false;
                activate_strings_layer();
                return false;
            }
            // Start hold — activate Fn layer
            layer_on(fn_layer);
            fn_tap_timer = timer_read();
        } else {
            // Release
            layer_off(fn_layer);
            // Mark as pending tap if it was a quick tap (not a hold)
            if (timer_elapsed(fn_tap_timer) < FN_TAP_TIMEOUT) {
                fn_tap_pending = true;
                fn_tap_timer = timer_read();
            }
        }
        return false;
    }

    // Reset fn_tap_pending on any other key press
    if (record->event.pressed && fn_tap_pending) {
        fn_tap_pending = false;
    }

    // STRINGS_LAYER: any key press deactivates the layer
    if (strings_layer_active && record->event.pressed && keycode != FN_TAP) {
        bool is_string_macro = IS_STRING_MACRO(keycode);
        deactivate_strings_layer();
        // If it's not a string macro, block the keypress (just exit layer)
        if (!is_string_macro) {
            return false;
        }
        // String macro will be processed below
    }

    switch (keycode) {
        case TOGGLE_F_LAYER: // Переключение F-Layer
            if (record->event.pressed) {
                if (layer_state_is(MAC_F_LAYER)) {
                    layer_off(MAC_F_LAYER);
                } else {
                    layer_on(MAC_F_LAYER);
                }
            } return false;

        case STRNG_C: // "t:camera"
            if (record->event.pressed) {
                SEND_STRING("t:camera");
            } return false;
        case STRNG_R: // "t:renderer"
            if (record->event.pressed) {
                SEND_STRING("t:renderer");
            } return false;
        case STRNG_S: // "t:Shader"
            if (record->event.pressed) {
                SEND_STRING("t:Shader");
            } return false;
    }

    return true;
}
