#pragma once

#include QMK_KEYBOARD_H  // IWYU pragma: keep


// Matrix Cols is 17 for K13 Max
#define M_COLS 17

#define GRID_WIDTH 12
#define GRID_HEIGHT 4

// Logical map to Matrix Index (row * 17 + col)
// 0xFF = invalid/empty
static const uint8_t PROGMEM grid_map_repeat[GRID_HEIGHT][GRID_WIDTH] = {
    // Row 0 -> Physical Row 1 (1, 2... =)
    {
        1*M_COLS+1, 1*M_COLS+2, 1*M_COLS+3, 1*M_COLS+4, 1*M_COLS+5, 1*M_COLS+6,
        1*M_COLS+7, 1*M_COLS+8, 1*M_COLS+9, 1*M_COLS+10, 1*M_COLS+11, 1*M_COLS+12
    },
    // Row 1 -> Physical Row 2 (Q, W... ])
    {
        2*M_COLS+1, 2*M_COLS+2, 2*M_COLS+3, 2*M_COLS+4, 2*M_COLS+5, 2*M_COLS+6,
        2*M_COLS+7, 2*M_COLS+8, 2*M_COLS+9, 2*M_COLS+10, 2*M_COLS+11, 2*M_COLS+12
    },
    // Row 2 -> Physical Row 3 (A, S... ') - Last 2 entries duplicated to match width 12
    {
        3*M_COLS+1, 3*M_COLS+2, 3*M_COLS+3, 3*M_COLS+4, 3*M_COLS+5, 3*M_COLS+6,
        3*M_COLS+7, 3*M_COLS+8, 3*M_COLS+9, 3*M_COLS+10, 3*M_COLS+11, 3*M_COLS+11
    },
    // Row 3 -> Physical Row 4 (Z, X... /) - Last 3 entries duplicated to match width 12
    {
        4*M_COLS+2, 4*M_COLS+3, 4*M_COLS+4, 4*M_COLS+5, 4*M_COLS+6, 4*M_COLS+7,
        4*M_COLS+8, 4*M_COLS+9, 4*M_COLS+10, 4*M_COLS+11, 4*M_COLS+11, 4*M_COLS+11
    }
};

static const uint8_t PROGMEM grid_map_clamp[GRID_HEIGHT][GRID_WIDTH] = {
    // Row 0 -> Physical Row 1 (1, 2... =)
    {
        1*M_COLS+1, 1*M_COLS+2, 1*M_COLS+3, 1*M_COLS+4, 1*M_COLS+5, 1*M_COLS+6,
        1*M_COLS+7, 1*M_COLS+8, 1*M_COLS+9, 1*M_COLS+10, 1*M_COLS+11, 1*M_COLS+12
    },
    // Row 1 -> Physical Row 2 (Q, W... ])
    {
        2*M_COLS+1, 2*M_COLS+2, 2*M_COLS+3, 2*M_COLS+4, 2*M_COLS+5, 2*M_COLS+6,
        2*M_COLS+7, 2*M_COLS+8, 2*M_COLS+9, 2*M_COLS+10, 2*M_COLS+11, 2*M_COLS+12
    },
    // Row 2 -> Physical Row 3 (A, S... ') - Last 2 entries duplicated to match width 12
    {
        3*M_COLS+1, 3*M_COLS+2, 3*M_COLS+3, 3*M_COLS+4, 3*M_COLS+5, 3*M_COLS+6,
        3*M_COLS+7, 3*M_COLS+8, 3*M_COLS+9, 3*M_COLS+10, 3*M_COLS+11, 0xFF
    },
    // Row 3 -> Physical Row 4 (Z, X... /) - Last 3 entries clamped to valid range
    {
        4*M_COLS+2, 4*M_COLS+3, 4*M_COLS+4, 4*M_COLS+5, 4*M_COLS+6, 4*M_COLS+7,
        4*M_COLS+8, 4*M_COLS+9, 4*M_COLS+10, 4*M_COLS+11, 0xFF, 0xFF
    }
};

// Helper to get LED index from matrix index
static inline uint8_t get_led_index_from_matrix(uint8_t matrix_idx) {
    if (matrix_idx == 0xFF) return NO_LED;
    uint8_t r = matrix_idx / M_COLS;
    uint8_t c = matrix_idx % M_COLS;
    // Safety check
    if (r >= MATRIX_ROWS || c >= MATRIX_COLS) return NO_LED;
    return g_led_config.matrix_co[r][c];
}
