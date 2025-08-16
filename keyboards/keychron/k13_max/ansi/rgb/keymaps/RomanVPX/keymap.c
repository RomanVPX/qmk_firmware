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
#include <math.h>

enum layers {
    MAC_BASE,
    MAC_FN,
    WIN_BASE,
    WIN_FN,
    MAC_F_LAYER,  // Новый слой для F-клавиш в macOS
};

// Пользовательские макросы
enum custom_keycodes {
    MACRO0 = SAFE_RANGE,
    MACRO1,
    MACRO2,
    MACRO3,
    MACRO4,
    MACRO5,
    MACRO6,
    MACRO7,
    MACRO8,
    MACRO13 = SAFE_RANGE + 13,
    MACRO14,
    MACRO15,
    TOGGLE_F_LAYER,
};


/* docs/feature_rgb_matrix.md
|Key                |Aliases   |Description                                                                           |
|-------------------|----------|--------------------------------------------------------------------------------------|
|`RGB_TOG`          |          |Toggle RGB lighting on or off                                                         |
|`RGB_MODE_FORWARD` |`RGB_MOD` |Cycle through modes, reverse direction when Shift is held                             |
|`RGB_MODE_REVERSE` |`RGB_RMOD`|Cycle through modes in reverse, forward direction when Shift is held                  |
|`RGB_HUI`          |          |Increase hue, decrease hue when Shift is held                                         |
|`RGB_HUD`          |          |Decrease hue, increase hue when Shift is held                                         |
|`RGB_SAI`          |          |Increase saturation, decrease saturation when Shift is held                           |
|`RGB_SAD`          |          |Decrease saturation, increase saturation when Shift is held                           |
|`RGB_VAI`          |          |Increase value (brightness), decrease value when Shift is held                        |
|`RGB_VAD`          |          |Decrease value (brightness), increase value when Shift is held                        |
|`RGB_SPI`          |          |Increase effect speed (does not support eeprom yet), decrease speed when Shift is held|
|`RGB_SPD`          |          |Decrease effect speed (does not support eeprom yet), increase speed when Shift is held|
|-------------------|----------|--------------------------------------------------------------------------------------|
*/

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [MAC_BASE] = LAYOUT_ansi_90(
        KC_ESC,             KC_BRID,  KC_BRIU,  KC_MCTRL, KC_LNPAD, RGB_VAD,  RGB_VAI,  KC_MPRV, KC_MPLY,       KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  MACRO5,   MACRO7,   MACRO6,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,    KC_9,          KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,  MACRO15,  MACRO13,  MACRO14,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,    KC_O,          KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,  KC_INS,   KC_HOME,  KC_PGUP,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,    KC_L,          KC_SCLN,  KC_QUOT,            KC_ENT,   KC_DEL,   KC_END,   KC_PGDN,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,    KC_COMM,       KC_DOT,   KC_SLSH,            KC_RSFT,            KC_UP,
        KC_LCTL,  KC_LOPTN, KC_LCMMD,                               KC_SPC,                                     KC_RCMMD, KC_ROPTN, MO(MAC_FN),KC_RCTL, KC_LEFT,  KC_DOWN,  KC_RGHT),

    [MAC_FN] = LAYOUT_ansi_90(
        _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______, _______,       _______,  _______,  _______,  _______,  KC_NUM,   XXXXXXX,  _______,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  P2P4G,    _______,  _______,  _______,  _______, _______,       _______,  _______,  _______,  _______,  _______,  _______,  _______,
        RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______, _______,       _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______, TOGGLE_F_LAYER,_______,  _______,  _______,  MACRO3,   _______,  MACRO4,
        _______,            _______,  _______,  _______,  _______,  BAT_LVL,  NK_TOGG,  _______, _______,       _______,  _______,            _______,            _______,
        _______,  _______,  _______,                                _______,                                    _______,  _______,  _______,  _______,  MACRO2,   MACRO0,   MACRO1),

    [WIN_BASE] = LAYOUT_ansi_90(
        KC_ESC,             KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,   KC_F8,         KC_F9,    KC_F10,   KC_F11,   KC_F12,   XXXXXXX,  XXXXXXX,  XXXXXXX,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,    KC_9,          KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,  XXXXXXX,  XXXXXXX,  XXXXXXX,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,    KC_O,          KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,  KC_INS,   KC_HOME,  KC_PGUP,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,    KC_L,          KC_SCLN,  KC_QUOT,            KC_ENT,   KC_DEL,   KC_END,   KC_PGDN,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,    KC_COMM,       KC_DOT,   KC_SLSH,            KC_RSFT,            KC_UP,
        KC_LCTL,  KC_LWIN,  KC_LALT,                                KC_SPC,                                     KC_RALT,  KC_RGUI, MO(WIN_FN),KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT),

    [WIN_FN] = LAYOUT_ansi_90(
        _______,            KC_BRID,  KC_BRIU,  KC_TASK,  KC_FILE,  RGB_VAD,  RGB_VAI,  KC_MPRV, KC_MPLY,       KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  _______,  _______,  _______,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  P2P4G,    _______,  _______,  _______,  _______, _______,       _______,  _______,  _______,  _______,  _______,  _______,  _______,
        RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______, _______,       _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______, _______,       _______,  _______,            _______,  KC_NUM,   _______,  _______,
        _______,            _______,  _______,  _______,  _______,  BAT_LVL,  NK_TOGG,  _______, _______,       _______,  _______,            _______,            _______,
        _______,  _______,  _______,                                _______,                                    _______,  _______,  _______,  _______,  _______,  _______,  _______),

    // MAC_F_LAYER - прозрачный, кроме F1-F12
    [MAC_F_LAYER] = LAYOUT_ansi_90(
        _______,             KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,   KC_F8,         KC_F9,    KC_F10,   KC_F11,   KC_F12,   _______,  _______,  _______,
        _______,  _______,   _______,  _______, _______,  _______,   _______, _______,   _______, _______,       _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,   _______,  _______, _______,  _______,   _______, _______,   _______, _______,       _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,   _______,  _______, _______,  _______,   _______, _______,   _______, _______,       _______,  _______,            _______,  _______,  _______,  _______,
        _______,             _______,  _______, _______,  _______,   _______, _______,   _______, _______,       _______,  _______,            _______,            _______,
        _______,  _______,   _______,                                _______,                                    _______,  _______,  _______,  _______,  _______,  _______,  _______)
};
// clang-format on
//
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

    return (base_keycode != target_keycode) && (target_keycode != KC_TRNS);
}

// Функция для подсветки F-клавиш указанным цветом
void highlight_f_keys(uint8_t led_min, uint8_t led_max, uint8_t r, uint8_t g, uint8_t b) {
    for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
        for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
            uint16_t keycode = keymap_key_to_keycode(MAC_F_LAYER, (keypos_t){col, row});
            if (keycode >= KC_F1 && keycode <= KC_F12) {
                uint8_t index = g_led_config.matrix_co[row][col];
                if (index != NO_LED && index >= led_min && index < led_max) {
                    rgb_matrix_set_color(index, r, g, b);
                }
            }
        }
    }
}

// RGB индикация для активных слоев
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    uint8_t base_layer = layer_state & (1UL << 1) ? MAC_BASE : WIN_BASE;

    // Если активен F-Layer для macOS, подсвечиваем F-клавиши бирюзовым
    if (layer_state_is(MAC_F_LAYER)) {
        highlight_f_keys(led_min, led_max, 0, 255, 255); // RGB_CYAN
    }

    // Если активен функциональный слой (MAC_FN или WIN_FN)
    if (layer_state_is(MAC_FN) || layer_state_is(WIN_FN)) {
        // Для MAC_FN подсвечиваем клавиши, которые меняют своё назначение
        uint8_t fn_layer = layer_state_is(MAC_FN) ? MAC_FN : WIN_FN;

        // --- Пульсация по яркости только для нужных клавиш ---
        uint32_t t = timer_read();
        // Период пульсации (мс)
        const uint16_t period = 900;
        float phase = (float)(t % period) / (float)period;
        // Синусоидальная пульсация между min_val и max_val
        uint8_t min_val = 80, max_val = 255;
        float val_f = min_val + (max_val - min_val) * 0.5f * (1.0f + sinf(phase * 2.0f * 3.1415926f));
        uint8_t val = (uint8_t)val_f;

        for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
            for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
                if (is_key_modified_in_layer(row, col, base_layer, fn_layer)) {
                    uint8_t index = g_led_config.matrix_co[row][col];
                     // Если светодиод существует и находится в пределах обрабатываемого диапазона
                    if (index != NO_LED && index >= led_min && index < led_max) {
                        // Бирюзовый HSV: h=128~180, s=255, v=val
                        // Используем стандартную функцию для HSV->RGB
                        HSV hsv = {128, 255, val};
                        RGB rgb = hsv_to_rgb(hsv);
                        rgb_matrix_set_color(index, rgb.r, rgb.g, rgb.b);
                    }
                }
            }
        }
    }

    return false;
}


#define HYPER_REGISTER register_code(KC_LSFT); register_code(KC_LCTL); register_code(KC_LALT); register_code(KC_LGUI);
#define HYPER_UNREGISTER unregister_code(KC_LGUI); unregister_code(KC_LALT); unregister_code(KC_LCTL); unregister_code(KC_LSFT);

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_keychron_common(keycode, record)) {
        return false;
    }
    // F-клавиши обрабатываются автоматически через слой MAC_F_LAYER
    switch (keycode) {
        case TOGGLE_F_LAYER: // Переключение F-Layer
            if (record->event.pressed) {
                // Просто переключаем слой MAC_F_LAYER
                if (layer_state_is(MAC_F_LAYER)) {
                    layer_off(MAC_F_LAYER);
                } else {
                    layer_on(MAC_F_LAYER);
                }
            } return false;

        case MACRO0: // Hyper ↓
            if (record->event.pressed) {
                HYPER_REGISTER
                register_code(KC_DOWN);
            } else {
                unregister_code(KC_DOWN);
                HYPER_UNREGISTER
            } return false;

        case MACRO1: // Hyper →
            if (record->event.pressed) {
                HYPER_REGISTER
                register_code(KC_RGHT);
            } else {
                unregister_code(KC_RGHT);
                HYPER_UNREGISTER
            } return false;

        case MACRO2: // Hyper ←
            if (record->event.pressed) {
                HYPER_REGISTER
                register_code(KC_LEFT);
            } else {
                unregister_code(KC_LEFT);
                HYPER_UNREGISTER
            }
            return false;
        case MACRO3: // Hyper NUMPAD1
            if (record->event.pressed) {
                HYPER_REGISTER
                register_code(KC_P1);
            } else {
                unregister_code(KC_P1);
                HYPER_UNREGISTER
            } return false;

        case MACRO4: // Hyper NUMPAD3
            if (record->event.pressed) {
                HYPER_REGISTER
                register_code(KC_P3);
            } else {
                unregister_code(KC_P3);
                HYPER_UNREGISTER
            } return false;

        case MACRO5: // Hyper C
            if (record->event.pressed) {
                HYPER_REGISTER
                register_code(KC_C);
            } else {
                unregister_code(KC_C);
                HYPER_UNREGISTER
            } return false;

        case MACRO6: // Hyper T
            if (record->event.pressed) {
                HYPER_REGISTER
                register_code(KC_T);
            } else {
                unregister_code(KC_T);
                HYPER_UNREGISTER
            } return false;

        case MACRO7: // Hyper N
            if (record->event.pressed) {
                HYPER_REGISTER
                register_code(KC_N);
            } else {
                unregister_code(KC_N);
                HYPER_UNREGISTER
            } return false;

        case MACRO13: // "t:renderer"
            if (record->event.pressed) {
                SEND_STRING("t:renderer");
            } return false;

        case MACRO14: // ⇧⌘P
            if (record->event.pressed) {
                register_code(KC_LSFT);
                register_code(KC_LGUI);
                register_code(KC_P);
            } else {
                unregister_code(KC_P);
                unregister_code(KC_LGUI);
                unregister_code(KC_LSFT);
            } return false;

        case MACRO15: // "t:camera"
            if (record->event.pressed) {
                SEND_STRING("t:camera");
            } return false;
    }

    return true;
}
