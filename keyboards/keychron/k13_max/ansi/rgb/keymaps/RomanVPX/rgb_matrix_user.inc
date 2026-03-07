RGB_MATRIX_EFFECT(TYPING_HEATMAP_WIDE_SPACE)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

#ifndef RGB_MATRIX_TYPING_HEATMAP_INCREASE_STEP
#    define RGB_MATRIX_TYPING_HEATMAP_INCREASE_STEP 32
#endif

#ifndef RGB_MATRIX_TYPING_HEATMAP_DECREASE_DELAY_MS
#    define RGB_MATRIX_TYPING_HEATMAP_DECREASE_DELAY_MS 25
#endif

#ifndef RGB_MATRIX_TYPING_HEATMAP_SPREAD
#    define RGB_MATRIX_TYPING_HEATMAP_SPREAD 40
#endif

#ifndef RGB_MATRIX_TYPING_HEATMAP_AREA_LIMIT
#    define RGB_MATRIX_TYPING_HEATMAP_AREA_LIMIT 16
#endif

#ifndef RGB_MATRIX_TYPING_HEATMAP_WIDE_SPACEBAR_SPREAD_WEIGHT
#    define RGB_MATRIX_TYPING_HEATMAP_WIDE_SPACEBAR_SPREAD_WEIGHT 192
#endif

#ifndef RGB_MATRIX_TYPING_HEATMAP_WIDE_SPACEBAR_SELF_WEIGHT
#    define RGB_MATRIX_TYPING_HEATMAP_WIDE_SPACEBAR_SELF_WEIGHT 127
#endif

static inline uint8_t get_heatmap_decrease_delay(void) {
    uint8_t speed = rgb_matrix_config.speed;
    if (speed >= 224) return 9;
    if (speed >= 192) return 12;
    if (speed >= 160) return 15;
    if (speed >= 128) return 20;
    if (speed >= 96)  return 25; // default
    if (speed >= 64)  return 32;
    if (speed >= 32)  return 40;
    return 50;
}

void process_rgb_matrix_typing_heatmap_wide_space(uint8_t row, uint8_t col) {
    bool is_space = (row == 5 && col == 6);

    if (g_led_config.matrix_co[row][col] == NO_LED) {
        return;
    }

    led_point_t center = g_led_config.point[g_led_config.matrix_co[row][col]];
    led_point_t sources[5];
    uint8_t source_count = 0;

    if (is_space) {
        sources[source_count++] = (led_point_t){qsub8(center.x, 20), center.y};
        sources[source_count++] = (led_point_t){qsub8(center.x, 40), center.y};
        sources[source_count++] = (led_point_t){qadd8(center.x, 20), center.y};
        sources[source_count++] = (led_point_t){qadd8(center.x, 40), center.y};
    } else {
        sources[source_count++] = (led_point_t){center.x, center.y};
    }

    uint8_t increase_step = RGB_MATRIX_TYPING_HEATMAP_INCREASE_STEP;
    if (is_space) {
        increase_step = scale8(increase_step, RGB_MATRIX_TYPING_HEATMAP_WIDE_SPACEBAR_SELF_WEIGHT);
    }
    g_rgb_frame_buffer[row][col] = qadd8(g_rgb_frame_buffer[row][col], increase_step);

    for (uint8_t i_row = 0; i_row < MATRIX_ROWS; i_row++) {
        for (uint8_t i_col = 0; i_col < MATRIX_COLS; i_col++) {
            if (g_led_config.matrix_co[i_row][i_col] == NO_LED) {
                continue;
            }

            if (i_row == row && i_col == col) {
                continue;
            }

            led_point_t target = g_led_config.point[g_led_config.matrix_co[i_row][i_col]];
            uint8_t min_distance = 255;

            for (uint8_t s = 0; s < source_count; s++) {
                int16_t dx = (int16_t)sources[s].x - target.x;
                int16_t dy = (int16_t)sources[s].y - target.y;
                uint8_t dist = sqrt16(dx * dx + dy * dy);
                if (dist < min_distance) {
                    min_distance = dist;
                }
            }

            if (min_distance <= RGB_MATRIX_TYPING_HEATMAP_SPREAD) {
                uint8_t amount = qsub8(RGB_MATRIX_TYPING_HEATMAP_SPREAD, min_distance);
                if (amount > RGB_MATRIX_TYPING_HEATMAP_AREA_LIMIT) {
                    amount = RGB_MATRIX_TYPING_HEATMAP_AREA_LIMIT;
                }
                if (is_space) {
                    amount = scale8(amount, RGB_MATRIX_TYPING_HEATMAP_WIDE_SPACEBAR_SPREAD_WEIGHT);
                }
                g_rgb_frame_buffer[i_row][i_col] = qadd8(g_rgb_frame_buffer[i_row][i_col], amount);
            }
        }
    }
}

static uint16_t heatmap_wide_decrease_timer;
static bool decrease_heatmap_wide_values;

bool TYPING_HEATMAP_WIDE_SPACE(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    if (params->init) {
        rgb_matrix_set_color_all(0, 0, 0);
        memset(g_rgb_frame_buffer, 0, sizeof g_rgb_frame_buffer);
    }

    if (params->iter == 0) {
        uint8_t delay_ms = get_heatmap_decrease_delay();
        decrease_heatmap_wide_values = timer_elapsed(heatmap_wide_decrease_timer) >= delay_ms;

        if (decrease_heatmap_wide_values) {
            heatmap_wide_decrease_timer = timer_read();
        }
    }

    uint8_t count = 0;
    for (uint8_t row = 0; row < MATRIX_ROWS && count < RGB_MATRIX_LED_PROCESS_LIMIT; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS && count < RGB_MATRIX_LED_PROCESS_LIMIT; col++) {
            if (g_led_config.matrix_co[row][col] >= led_min && g_led_config.matrix_co[row][col] < led_max) {
                count++;
                uint8_t val = g_rgb_frame_buffer[row][col];
                if (!HAS_ANY_FLAGS(g_led_config.flags[g_led_config.matrix_co[row][col]], params->flags)) continue;

                HSV hsv = {170 - qsub8(val, 85), rgb_matrix_config.hsv.s, scale8((qadd8(170, val) - 170) * 3, rgb_matrix_config.hsv.v)};
                RGB rgb = rgb_matrix_hsv_to_rgb(hsv);
                rgb_matrix_set_color(g_led_config.matrix_co[row][col], rgb.r, rgb.g, rgb.b);

                if (decrease_heatmap_wide_values) {
                    g_rgb_frame_buffer[row][col] = qsub8(val, 1);
                }
            }
        }
    }

    return rgb_matrix_check_finished_leds(led_max);
}

#endif
