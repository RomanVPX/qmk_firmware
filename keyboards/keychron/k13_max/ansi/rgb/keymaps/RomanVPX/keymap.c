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

// Используем настоящий слой MAC_F_LAYER вместо переменной
// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [MAC_BASE] = LAYOUT_ansi_90(
        KC_ESC,             KC_BRID,  KC_BRIU,  KC_MCTRL, KC_LNPAD, RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  MACRO5,   MACRO7,   MACRO6,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,  MACRO15,  MACRO13,  MACRO14,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,  KC_INS,   KC_HOME,  KC_PGUP,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,   KC_DEL,   KC_END,   KC_PGDN,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,            KC_UP,
        KC_LCTL,  KC_LOPTN, KC_LCMMD,                               KC_SPC,                                 KC_RCMMD, KC_ROPTN, MO(MAC_FN),KC_RCTL, KC_LEFT,  KC_DOWN,  KC_RGHT),

    [MAC_FN] = LAYOUT_ansi_90(
        _______,            _______,    _______, _______,  _______,    _______, _______, _______, _______,  _______, _______,   _______,   _______, KC_NUM,   XXXXXXX,  _______,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  P2P4G,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______,  TOGGLE_F_LAYER, _______,  _______,      _______,  MACRO3,   _______,  MACRO4,
        _______,            _______,  _______,  _______,  _______,  BAT_LVL,  NK_TOGG,  _______,  _______,  _______,  _______,            _______,            _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,     MACRO2,  MACRO0, MACRO1),

    [WIN_BASE] = LAYOUT_ansi_90(
        KC_ESC,             KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_P7,    KC_P8,    KC_P9,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,  KC_P4,    KC_P5,    KC_P6,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,  KC_P1,    KC_P2,    KC_P3,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,   KC_DEL,   KC_P0,    KC_PDOT,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,            KC_UP,
        KC_LCTL,  KC_LWIN,  KC_LALT,                                KC_SPC,                                 KC_RALT,  KC_RGUI, MO(WIN_FN),KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT),

    [WIN_FN] = LAYOUT_ansi_90(
        _______,            KC_BRID,  KC_BRIU,  KC_TASK,  KC_FILE,  RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  _______,  _______,  _______,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  P2P4G,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  KC_NUM,   _______,  _______,
        _______,            _______,  _______,  _______,  _______,  BAT_LVL,  NK_TOGG,  _______,  _______,  _______,  _______,            _______,            _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,  _______,  _______),

    // MAC_F_LAYER - прозрачный, кроме F1-F12
    [MAC_F_LAYER] = LAYOUT_ansi_90(
        _______,             KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   _______,   _______,   _______,
        _______,   _______,     _______,     _______,     _______,     _______,     _______,     _______,     _______,     _______,     _______,     _______,  _______,   _______,  _______,  _______,  _______,
        _______,   _______,     _______,     _______,     _______,     _______,     _______,     _______,     _______,     _______,     _______,     _______,  _______,  _______,  _______,   _______,  _______,
        _______,  _______,     _______,     _______,     _______,     _______,     _______,     _______,     _______,     _______,     _______,  _______,            _______,   _______,   _______,   _______,
        _______,            _______,     _______,     _______,     _______,     _______,     _______,     _______,     _______,  _______,   _______,            _______,            _______,
        _______,  _______, _______,                               _______,                                 _______, _______, _______,_______, _______,  _______,  _______)
};

// clang-format on
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

        // Подсветить только клавиши, которые изменяют своё назначение
        for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
            for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
                if (is_key_modified_in_layer(row, col, base_layer, fn_layer)) {
                    uint8_t index = g_led_config.matrix_co[row][col];

                    // Если светодиод существует и находится в пределах обрабатываемого диапазона
                    if (index != NO_LED && index >= led_min && index < led_max) {
                        rgb_matrix_set_color(index, RGB_CYAN); // Подсветить активные клавиши бирюзовым цветом
                    }
                }
            }
        }
    }

    return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_keychron_common(keycode, record)) {
        return false;
    }

    // F-клавиши обрабатываются автоматически через слой MAC_F_LAYER

    // Обработка пользовательских макросов
    switch (keycode) {
        case TOGGLE_F_LAYER: // Переключение F-Layer
            if (record->event.pressed) {
                // Просто переключаем слой MAC_F_LAYER
                if (layer_state_is(MAC_F_LAYER)) {
                    layer_off(MAC_F_LAYER);
                } else {
                    layer_on(MAC_F_LAYER);
                }
            }
            return false;
        case MACRO0: // Стрелка вниз с модификаторами
            if (record->event.pressed) {
                register_code(KC_LSFT);
                register_code(KC_LCTL);
                register_code(KC_LALT);
                register_code(KC_LGUI);
                register_code(KC_DOWN);
            } else {
                unregister_code(KC_DOWN);
                unregister_code(KC_LGUI);
                unregister_code(KC_LALT);
                unregister_code(KC_LCTL);
                unregister_code(KC_LSFT);
            }
            return false;

        case MACRO1: // Стрелка вправо с модификаторами
            if (record->event.pressed) {
                register_code(KC_LSFT);
                register_code(KC_LCTL);
                register_code(KC_LALT);
                register_code(KC_LGUI);
                register_code(KC_RGHT);
            } else {
                unregister_code(KC_RGHT);
                unregister_code(KC_LGUI);
                unregister_code(KC_LALT);
                unregister_code(KC_LCTL);
                unregister_code(KC_LSFT);
            }
            return false;

        case MACRO2: // Стрелка влево с модификаторами
            if (record->event.pressed) {
                register_code(KC_LSFT);
                register_code(KC_LCTL);
                register_code(KC_LALT);
                register_code(KC_LGUI);
                register_code(KC_LEFT);
            } else {
                unregister_code(KC_LEFT);
                unregister_code(KC_LGUI);
                unregister_code(KC_LALT);
                unregister_code(KC_LCTL);
                unregister_code(KC_LSFT);
            }
            return false;

        case MACRO3: // Клавиша 1 на цифровой клавиатуре с модификаторами
            if (record->event.pressed) {
                register_code(KC_LSFT);
                register_code(KC_LCTL);
                register_code(KC_LALT);
                register_code(KC_LGUI);
                register_code(KC_P1);
            } else {
                unregister_code(KC_P1);
                unregister_code(KC_LGUI);
                unregister_code(KC_LALT);
                unregister_code(KC_LCTL);
                unregister_code(KC_LSFT);
            }
            return false;

        case MACRO4: // Клавиша 3 на цифровой клавиатуре с модификаторами
            if (record->event.pressed) {
                register_code(KC_LSFT);
                register_code(KC_LCTL);
                register_code(KC_LALT);
                register_code(KC_LGUI);
                register_code(KC_P3);
            } else {
                unregister_code(KC_P3);
                unregister_code(KC_LGUI);
                unregister_code(KC_LALT);
                unregister_code(KC_LCTL);
                unregister_code(KC_LSFT);
            }
            return false;

        case MACRO5: // Клавиша C с модификаторами (левыми)
            if (record->event.pressed) {
                register_code(KC_LSFT);
                register_code(KC_LCTL);
                register_code(KC_LALT);
                register_code(KC_LGUI);
                register_code(KC_C);
            } else {
                unregister_code(KC_C);
                unregister_code(KC_LGUI);
                unregister_code(KC_LALT);
                unregister_code(KC_LCTL);
                unregister_code(KC_LSFT);
            }
            return false;

        case MACRO6: // Клавиша T с модификаторами (правыми)
            if (record->event.pressed) {
                register_code(KC_RCTL);
                register_code(KC_RALT);
                register_code(KC_RGUI);
                register_code(KC_RSFT);
                register_code(KC_T);
            } else {
                unregister_code(KC_T);
                unregister_code(KC_RGUI);
                unregister_code(KC_RALT);
                unregister_code(KC_RCTL);
                unregister_code(KC_RSFT);
            }
            return false;

        case MACRO7: // Клавиша N с модификаторами
            if (record->event.pressed) {
                register_code(KC_LSFT);
                register_code(KC_LCTL);
                register_code(KC_LALT);
                register_code(KC_LGUI);
                register_code(KC_N);
            } else {
                unregister_code(KC_N);
                unregister_code(KC_LGUI);
                unregister_code(KC_LALT);
                unregister_code(KC_LCTL);
                unregister_code(KC_LSFT);
            }
            return false;

        case MACRO13: // Текстовая строка "t:renderer"
            if (record->event.pressed) {
                SEND_STRING("t:renderer");
            }
            return false;

        case MACRO14: // Shift+Command+P
            if (record->event.pressed) {
                register_code(KC_LSFT);
                register_code(KC_LGUI);
                register_code(KC_P);
            } else {
                unregister_code(KC_P);
                unregister_code(KC_LGUI);
                unregister_code(KC_LSFT);
            }
            return false;

        case MACRO15: // Текстовая строка "t:camera"
            if (record->event.pressed) {
                SEND_STRING("t:camera");
            }
            return false;
    }

    return true;
}
