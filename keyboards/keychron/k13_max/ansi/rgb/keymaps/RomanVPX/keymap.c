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
    MAC_F_LAYER,  // Слой для клавиш F1-F12 в macOS
};

enum custom_keycodes {
    MACRO0 = SAFE_RANGE,
    MACRO1,
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
|`KC_NO`            |'XXXXXXX' |Ignore this key                                                                       |
|-------------------|----------|--------------------------------------------------------------------------------------|
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
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,       KC_9,        KC_0,     KC_MINS,  KC_EQL,    KC_BSPC, MACRO0,   MACRO1,   SCMD(KC_P),
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,       KC_O,        KC_P,     KC_LBRC,  KC_RBRC,   KC_BSLS, KC_INS,   KC_HOME,  KC_PGUP,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,       KC_L,        KC_SCLN,  KC_QUOT,             KC_ENT,  KC_DEL,   KC_END,   KC_PGDN,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,       KC_COMM,     KC_DOT,   KC_SLSH,             KC_RSFT,           KC_UP,
        KC_LCTL,  KC_LOPTN, KC_LCMMD,                               KC_SPC,                                      KC_RCMMD, KC_ROPTN, MO(MAC_FN),KC_RCTL, KC_LEFT,  KC_DOWN,  KC_RGHT),

    [MAC_FN] = LAYOUT_ansi_90(
        _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,     _______,  _______,  _______,   _______, KC_NUM,   XXXXXXX,  _______,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  P2P4G,    _______,  _______,  _______,  _______,    _______,     _______,  _______,  _______,   _______, _______,  _______,  _______,
        RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,    _______,     _______,  _______,  _______,   _______, _______,  _______,  _______,
        QK_BOOT,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______, TOGGLE_F_LAYER, _______,  _______,  _______,   HYP_P1,  _______,  HYP_P3,
        _______,            _______,  _______,  _______,  _______,  BAT_LVL,  NK_TOGG,  _______,    _______,     _______,  _______,             _______, _______,
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
        RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,    _______,     _______,  _______,  _______,   _______,  _______,  _______, _______,
        QK_BOOT,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______,    _______,     _______,  _______,             _______,  KC_NUM,   _______, _______,
        _______,            _______,  _______,  _______,  _______,  BAT_LVL,  NK_TOGG,  _______,    _______,     _______,  _______,             _______,            _______,
        _______,  _______,  _______,                                _______,                                     _______,  _______,  _______,   _______,  _______,  _______, _______),

    // MAC_F_LAYER - прозрачный, кроме F1-F12
    [MAC_F_LAYER] = LAYOUT_ansi_90(
        _______,             KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,      KC_F8,       KC_F9,    KC_F10,   KC_F11,   KC_F12,   _______,  _______, _______,
        _______,  _______,   _______,  _______, _______,  _______,   _______, _______,   _______,    _______,     _______,  _______,  _______,  _______,  _______,  _______, _______,
        _______,  _______,   _______,  _______, _______,  _______,   _______, _______,   _______,    _______,     _______,  _______,  _______,  _______,  _______,  _______, _______,
        _______,  _______,   _______,  _______, _______,  _______,   _______, _______,   _______,    _______,     _______,  _______,            _______,  _______,  _______, _______,
        _______,             _______,  _______, _______,  _______,   _______, _______,   _______,    _______,     _______,  _______,            _______,            _______,
        _______,  _______,   _______,                                _______,                                     _______,  _______,  _______,  _______,  _______,  _______, _______)
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

    return (base_keycode != target_keycode) && (target_keycode != KC_TRNS);
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

// RGB индикация для активных слоев
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    uint8_t current_val = rgb_matrix_get_val(); // Глобальная яркость подсветки
    uint8_t current_sat = rgb_matrix_get_sat(); // Глобальная насыщенность подсветки

    // Если подсветка выключена, ничего не делаем
    if (current_val == 0) {
        return false;
    }

    uint8_t base_layer = layer_state & (1UL << 1) ? MAC_BASE : WIN_BASE;
    // uint8_t base_layer = IS_LAYER_ON(WIN_BASE) ? WIN_BASE : MAC_BASE; // Нагляднее, но чёт не работает, надо проверить

    // --- Анимация пульсации ---
    // sin8(t) возвращает значение от 0 до 255 (из LUT), описывающее полную синусоиду.
    uint8_t sin_wave = sin8(timer_read() >> 4); // timer_read() возвращает миллисекунды. Сдвиг вправо замедляет анимацию.
    uint8_t max_v = current_val; // Максимум - текущая яркость, минимум - треть от нее.
    uint8_t min_v = max_v / 2; // Минимум — половина от максимума яркости
    // Масштабируем синусоиду (0-255) до нашего диапазона (0 - (max_v - min_v)) и прибавляем смещение min_v.
    uint8_t pulsing_val = min_v + scale8(sin_wave, max_v - min_v); // scale8 - быстрая 8-битная функция умножения (a * b) / 255.

    // --- Готовим наши цвета ---
    HSV hsv_static_cyan = {128, current_sat, current_val};
    RGB rgb_static_cyan = hsv_to_rgb(hsv_static_cyan);

    HSV hsv_pulsing_cyan = {128, current_sat, pulsing_val};
    RGB rgb_pulsing_cyan = hsv_to_rgb(hsv_pulsing_cyan);



    // Если активен F-Layer для macOS, подсвечиваем F-клавиши бирюзовым (статично)
    if (layer_state_is(MAC_F_LAYER)) {
        HSV hsv = {128, current_sat, current_val}; // H=128 (Cyan), S и V - глобальные
        RGB rgb = hsv_to_rgb(hsv);
        highlight_f_keys(led_min, led_max, rgb.r, rgb.g, rgb.b);
    }

    // 1. Подсветка F-клавиш, когда MAC_F_LAYER активен
    if (layer_state_is(MAC_F_LAYER)) {
        for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
            for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
                uint16_t keycode = keymap_key_to_keycode(MAC_F_LAYER, (keypos_t){col, row});
                if (IS_F_KEYCODE(keycode)) {
                    uint8_t index = g_led_config.matrix_co[row][col];
                    if (index != NO_LED && index >= led_min && index < led_max) {
                        // Анимация ТОЛЬКО в режиме Mac
                        if (base_layer == MAC_BASE) {
                            rgb_matrix_set_color(index, rgb_pulsing_cyan.r, rgb_pulsing_cyan.g, rgb_pulsing_cyan.b);
                        } else {
                            rgb_matrix_set_color(index, rgb_static_cyan.r, rgb_static_cyan.g, rgb_static_cyan.b);
                        }
                    }
                }
            }
        }
    }

    // 2. Подсветка измененных клавиш на FN слое
    if (layer_state_is(MAC_FN) || layer_state_is(WIN_FN)) {
        uint8_t fn_layer = layer_state_is(MAC_FN) ? MAC_FN : WIN_FN;

        for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
            for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
                if (is_key_modified_in_layer(row, col, base_layer, fn_layer)) {
                    uint8_t index = g_led_config.matrix_co[row][col];
                    if (index != NO_LED && index >= led_min && index < led_max) {
                        // Пропускаем F-клавиши, они уже обработаны выше
                        uint16_t f_layer_keycode = keymap_key_to_keycode(MAC_F_LAYER, (keypos_t){col, row});
                        if (layer_state_is(MAC_F_LAYER) && IS_F_KEYCODE(f_layer_keycode)) {
                           continue;
                        }

                        // Получаем кейкод на FN-слое, чтобы найти нашу 'L' (TOGGLE_F_LAYER)
                        uint16_t fn_keycode = keymap_key_to_keycode(fn_layer, (keypos_t){col, row});

                        // Анимация для TOGGLE_F_LAYER и только в режиме Mac
                        if (fn_keycode == TOGGLE_F_LAYER && base_layer == MAC_BASE) {
                             rgb_matrix_set_color(index, rgb_pulsing_cyan.r, rgb_pulsing_cyan.g, rgb_pulsing_cyan.b);
                        } else {
                             // Все остальные измененные клавиши подсвечиваем статично
                             rgb_matrix_set_color(index, rgb_static_cyan.r, rgb_static_cyan.g, rgb_static_cyan.b);
                        }
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
    switch (keycode) {
        case TOGGLE_F_LAYER: // Переключение F-Layer
            if (record->event.pressed) {
                if (layer_state_is(MAC_F_LAYER)) {
                    layer_off(MAC_F_LAYER);
                } else {
                    layer_on(MAC_F_LAYER);
                }
            } return false;

        case MACRO0: // "t:camera"
            if (record->event.pressed) {
                SEND_STRING("t:camera");
            } return false;
        case MACRO1: // "t:renderer"
            if (record->event.pressed) {
                SEND_STRING("t:renderer");
            } return false;
    }

    return true;
}
