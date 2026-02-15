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
#include "rgb_utils.h"
#include "matrix_utils.h"
#include "games/game_interface.h"
#include "custom_keycodes.h"
#include "strings_layer.h"

enum layers {
    MAC_BASE,
    MAC_FN,
    WIN_BASE,
    WIN_FN,
    MAC_F_LAYER,    // Слой для клавиш F1-F12 в macOS
    STRINGS_LAYER,  // Слой с клавишами STRINGnn
};

typedef struct {
    RGB main;
    RGB alt;
    RGB pulsing;
    RGB antiphase;
} Palette;

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

static inline void handle_win_lighting(uint8_t row, uint8_t col, uint8_t index, const RGB* static_color, bool is_win_fn) {
    if (is_win_fn && is_key_modified_in_layer(row, col, WIN_BASE, WIN_FN)) {
        rgb_matrix_set_color(index, static_color->r, static_color->g, static_color->b);
    }
}

// #define INDICATOR_MAX_VALUE rgb_matrix_get_val()
#ifndef INDICATOR_MAX_VALUE
    #define INDICATOR_MAX_VALUE RGB_MATRIX_MAXIMUM_BRIGHTNESS
#endif

#define MAIN_COLOR_HSV                  (HSV){HSV_MAGENTA}
#define SECONDARY_COLOR_HSV             (HSV){HSV_CYAN}

static inline Palette get_current_palette(uint8_t current_val) {
    PulsingConfig pulsing_cfg = PULSING_CONFIG_DEFAULT;

    HSV hsv_static_main = MAIN_COLOR_HSV;
    hsv_static_main.v = current_val;
    RGB rgb_static_main = hsv_to_rgb_effective(hsv_static_main);

    HSV hsv_static_alt = SECONDARY_COLOR_HSV;
    hsv_static_alt.v = current_val;
    RGB rgb_static_alt = hsv_to_rgb_effective(hsv_static_alt);
    RGB rgb_pulsing_alt = rgb_pulsing(hsv_static_alt, current_val, pulsing_cfg, 0);
    RGB rgb_antiphase_pulsing_alt = rgb_pulsing(hsv_static_alt, current_val, pulsing_cfg, 128);

    return (Palette){
        .main = rgb_static_main,
        .alt = rgb_static_alt,
        .pulsing = rgb_pulsing_alt,
        .antiphase = rgb_antiphase_pulsing_alt,
    };
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (games_render()) {
        return false;
    }
    uint8_t current_val = INDICATOR_MAX_VALUE;
    if (current_val == 0) return false;

    // STRINGS_LAYER has priority
    if (strings_layer_rgb_render(led_min, led_max, current_val, STRINGS_LAYER)) {
        return false;
    }

    uint8_t base_layer;
    if (layer_state_is(MAC_FN) || layer_state_is(WIN_FN)) { // Если Fn зажата, используем работающий хак:
        base_layer = layer_state & (1UL << MAC_FN) ? MAC_BASE : WIN_BASE; // base_layer = layer_state_is(MAC_FN) ? MAC_BASE : WIN_BASE;
    } else { // Если Fn НЕ зажата, используем стандартный метод:
        base_layer = IS_LAYER_ON(WIN_BASE) ? WIN_BASE : MAC_BASE;
    }

    bool is_mac_fn = layer_state_is(MAC_FN);
    bool is_mac_f_layer = layer_state_is(MAC_F_LAYER);
    bool is_win_fn = layer_state_is(WIN_FN);

    // Skip palette calculation if no indicators needed
    bool need_indicators = is_mac_fn || is_mac_f_layer || (base_layer == WIN_BASE && is_win_fn);
    if (!need_indicators) {
        return false;
    }

    // --- Анимация и цвета ---
    Palette palette = get_current_palette(current_val);

    FOR_EACH_LED_IN_RANGE(led_min, led_max) {
        if (base_layer == MAC_BASE) {
            handle_mac_lighting(row, col, led_index, &palette, is_mac_fn, is_mac_f_layer);
        } else {
            handle_win_lighting(row, col, led_index, &palette.main, is_win_fn);
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

    if (!strings_layer_process_record(keycode, record, fn_layer, MAC_BASE, STRINGS_LAYER)) {
        return false;
    }

    if (games_handle_trigger(keycode, record)) {
        return false;
    }

    if (keycode == TOGGLE_F_LAYER) {
        if (record->event.pressed) {
            if (layer_state_is(MAC_F_LAYER)) {
                layer_off(MAC_F_LAYER);
            } else {
                layer_on(MAC_F_LAYER);
            }
        }
        return false;
    }

    return true;
}

void eeconfig_init_user(void) {
    eeconfig_update_user(0);
}

void matrix_scan_user(void) {
    games_task();
}
