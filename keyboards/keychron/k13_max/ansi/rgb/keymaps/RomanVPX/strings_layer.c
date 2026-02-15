#include "strings_layer.h"
#include "custom_keycodes.h"
#include "rgb_utils.h"
#include "matrix_utils.h"
#include <string.h>

#define FN_TAP_TIMEOUT 200

#define ANIMATION_MAX_LENGTH 32
#define ANIMATION_CHAR_DURATION 600
#define ANIMATION_CHAR_FADEOUT 500

#define STRINGS_LAYER_COLOR_HSV         (HSV){HSV_GOLDENROD}
#define STRINGS_ANIMATION_COLOR_HSV     (HSV){HSV_SPRINGGREEN}
#define STRINGS_LAYER_PREVIEW_COLOR_HSV (HSV){HSV_WHITE}
#define STRINGS_LAYER_PREVIEW_DIV       2

typedef struct {
    uint8_t row;
    uint8_t col;
    bool valid;
} KeyPosCache;

static bool strings_layer_active = false;
static bool fn_held_in_strings_layer = false;
static bool fn_used_for_combo = false;
static uint16_t fn_strings_timer = 0;
static uint16_t fn_tap_timer = 0;
static bool fn_tap_pending = false;

static bool animation_active = false;
static const char *animation_string = NULL;
static uint8_t animation_index = 0;
static uint8_t animation_length = 0;
static uint16_t animation_timer = 0;
static uint8_t prev_anim_row = 0;
static uint8_t prev_anim_col = 0;
static bool prev_anim_valid = false;
static uint16_t prev_anim_timer = 0;

static KeyPosCache anim_cache[ANIMATION_MAX_LENGTH];
static uint32_t anim_key_mask[MATRIX_ROWS];
_Static_assert(MATRIX_COLS <= 32, "anim_key_mask requires MATRIX_COLS <= 32 for uint32_t bitmask");

static const char *get_string_for_keycode(uint16_t keycode) {
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

static bool find_keycode_position(uint16_t target, uint8_t layer, uint8_t *out_row, uint8_t *out_col) {
    FOR_EACH_MATRIX_POS() {
        if (KEYCODE_AT(layer, row, col) == target) {
            *out_row = row;
            *out_col = col;
            return true;
        }
    }
    return false;
}

static bool get_matrix_position_for_char(char c, uint8_t base_layer, uint8_t *row, uint8_t *col) {
    uint16_t kc = char_to_keycode(c);
    if (kc == KC_NO) {
        return false;
    }
    return find_keycode_position(kc, base_layer, row, col);
}

static void stop_animation(void) {
    animation_active = false;
    animation_string = NULL;
    animation_index = 0;
    animation_length = 0;
    prev_anim_valid = false;
}

static void start_animation(uint16_t keycode, uint8_t base_layer) {
    const char *str = get_string_for_keycode(keycode);
    if (str == NULL) {
        return;
    }

    animation_string = str;
    animation_length = strlen(str);
    if (animation_length > ANIMATION_MAX_LENGTH) {
        animation_length = ANIMATION_MAX_LENGTH;
    }

    memset(anim_key_mask, 0, sizeof(anim_key_mask));
    for (uint8_t i = 0; i < animation_length; i++) {
        uint8_t r;
        uint8_t c;
        if (get_matrix_position_for_char(str[i], base_layer, &r, &c)) {
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

static inline bool is_key_in_animation_string(uint8_t row, uint8_t col) {
    return animation_active && (anim_key_mask[row] & (1UL << col));
}

static void deactivate_strings_layer(uint8_t strings_layer) {
    if (strings_layer_active) {
        strings_layer_active = false;
        fn_held_in_strings_layer = false;
        fn_used_for_combo = false;
        stop_animation();
        layer_off(strings_layer);
    }
}

static void activate_strings_layer(uint8_t strings_layer) {
    strings_layer_active = true;
    layer_on(strings_layer);
}

static bool process_fn_tap_logic(keyrecord_t *record, uint8_t fn_layer, uint8_t strings_layer) {
    if (record->event.pressed) {
        if (strings_layer_active) {
            fn_held_in_strings_layer = true;
            fn_used_for_combo = false;
            fn_strings_timer = timer_read();
            return false;
        }
        if (fn_tap_pending && timer_elapsed(fn_tap_timer) < FN_TAP_TIMEOUT) {
            fn_tap_pending = false;
            activate_strings_layer(strings_layer);
            return false;
        }
        layer_on(fn_layer);
        fn_tap_timer = timer_read();
    } else {
        if (strings_layer_active && fn_held_in_strings_layer) {
            if (!fn_used_for_combo && timer_elapsed(fn_strings_timer) < FN_TAP_TIMEOUT) {
                deactivate_strings_layer(strings_layer);
            }
            fn_held_in_strings_layer = false;
            fn_used_for_combo = false;
            return false;
        }
        fn_held_in_strings_layer = false;
        layer_off(fn_layer);
        if (timer_elapsed(fn_tap_timer) < FN_TAP_TIMEOUT) {
            fn_tap_pending = true;
            fn_tap_timer = timer_read();
        }
    }
    return false;
}

bool strings_layer_process_record(uint16_t keycode, keyrecord_t *record, uint8_t fn_layer, uint8_t mac_base_layer, uint8_t strings_layer) {
    bool is_string_macro = IS_STRING_MACRO(keycode);

    if (keycode == FN_TAP) {
        return process_fn_tap_logic(record, fn_layer, strings_layer);
    }

    if (record->event.pressed && fn_tap_pending) {
        fn_tap_pending = false;
    }

    if (strings_layer_active && record->event.pressed) {
        if (animation_active) {
            stop_animation();
        }

        if (fn_held_in_strings_layer && is_string_macro) {
            fn_used_for_combo = true;
            start_animation(keycode, mac_base_layer);
            return false;
        }

        if (fn_held_in_strings_layer && !is_string_macro) {
            fn_used_for_combo = true;
            deactivate_strings_layer(strings_layer);
            return false;
        }

        deactivate_strings_layer(strings_layer);
        if (!is_string_macro) {
            return false;
        }
    }

    if (is_string_macro) {
        if (record->event.pressed) {
            const char *str = get_string_for_keycode(keycode);
            if (str) {
                SEND_STRING(str);
            }
        }
        return false;
    }

    return true;
}

bool strings_layer_rgb_render(uint8_t led_min, uint8_t led_max, uint8_t current_val, uint8_t strings_layer) {
    if (!strings_layer_active) {
        return false;
    }

    PulsingConfig pulsing_cfg = PULSING_CONFIG_DEFAULT;

    HSV hsv_strings = STRINGS_LAYER_COLOR_HSV;
    hsv_strings.v = current_val;
    RGB rgb_strings = hsv_to_rgb_effective(hsv_strings);
    RGB rgb_strings_pulsing = rgb_pulsing(hsv_strings, current_val, pulsing_cfg, 0);

    HSV hsv_anim = STRINGS_ANIMATION_COLOR_HSV;
    hsv_anim.v = current_val;
    RGB rgb_anim = hsv_to_rgb_effective(hsv_anim);

    HSV hsv_preview = STRINGS_LAYER_PREVIEW_COLOR_HSV;
    hsv_preview.v = current_val / STRINGS_LAYER_PREVIEW_DIV;
    RGB rgb_anim_preview = hsv_to_rgb_effective(hsv_preview);

    uint8_t anim_row = 0;
    uint8_t anim_col = 0;
    bool anim_key_found = false;
    if (animation_active) {
        if (anim_cache[animation_index].valid) {
            anim_row = anim_cache[animation_index].row;
            anim_col = anim_cache[animation_index].col;
            anim_key_found = true;
        }

        if (timer_elapsed(animation_timer) >= ANIMATION_CHAR_DURATION) {
            if (anim_key_found) {
                prev_anim_row = anim_row;
                prev_anim_col = anim_col;
                prev_anim_valid = true;
                prev_anim_timer = timer_read();
            }
            animation_index++;
            animation_timer = timer_read();
            if (animation_index >= animation_length) {
                stop_animation();
                anim_key_found = false;
            } else {
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

    uint8_t transition_progress = 255;
    if (prev_anim_valid) {
        uint16_t elapsed = timer_elapsed(prev_anim_timer);
        if (elapsed >= ANIMATION_CHAR_FADEOUT) {
            prev_anim_valid = false;
        } else {
            transition_progress = (elapsed * 255U) / ANIMATION_CHAR_FADEOUT;
        }
    }

    RGB blend_active = rgb_lerp(rgb_anim, rgb_strings, transition_progress);
    RGB blend_preview = rgb_lerp(rgb_anim, rgb_anim_preview, transition_progress);
    RGB blend_off = rgb_lerp(rgb_anim, (RGB){0, 0, 0}, transition_progress);

    FOR_EACH_LED_IN_RANGE(led_min, led_max) {
        uint16_t keycode = KEYCODE_AT(strings_layer, row, col);
        bool is_active_key = IS_STRING_MACRO(keycode);

        if (anim_key_found && row == anim_row && col == anim_col) {
            rgb_matrix_set_color(led_index, rgb_anim.r, rgb_anim.g, rgb_anim.b);
        } else if (prev_anim_valid && row == prev_anim_row && col == prev_anim_col) {
            if (is_active_key) {
                rgb_matrix_set_color(led_index, blend_active.r, blend_active.g, blend_active.b);
            } else if (is_key_in_animation_string(row, col)) {
                rgb_matrix_set_color(led_index, blend_preview.r, blend_preview.g, blend_preview.b);
            } else {
                rgb_matrix_set_color(led_index, blend_off.r, blend_off.g, blend_off.b);
            }
        } else if (animation_active && is_key_in_animation_string(row, col)) {
            rgb_matrix_set_color(led_index, rgb_anim_preview.r, rgb_anim_preview.g, rgb_anim_preview.b);
        } else if (fn_held_in_strings_layer && is_active_key) {
            rgb_matrix_set_color(led_index, rgb_strings_pulsing.r, rgb_strings_pulsing.g, rgb_strings_pulsing.b);
        } else if (is_active_key) {
            rgb_matrix_set_color(led_index, rgb_strings.r, rgb_strings.g, rgb_strings.b);
        }
    }

    return true;
}
