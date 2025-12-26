#include QMK_KEYBOARD_H  // IWYU pragma: keep

// Matrix Cols is 17 for K13 Max
#define M_COLS 17

#undef F_ROW
#undef GRID_MAP
#undef REPEAT_ENABLED

#ifdef GRID_SNAKE
    #define F_ROW 0
    #define GRID_MAP grid_map_repeat
    #define REPEAT_ENABLED true
    #undef GRID_SNAKE
#else
    #define F_ROW 1
    #define GRID_MAP grid_map_clamp
    #define REPEAT_ENABLED false
#endif

#define GRID_WIDTH 12
#define GRID_HEIGHT (4 + F_ROW)

// Logical map to Matrix Index (row * 17 + col)
// 0xFF = invalid/empty
static const uint8_t PROGMEM GRID_MAP[GRID_HEIGHT][GRID_WIDTH] = {
#if F_ROW
    // Physical Row 0 (Function keys - F1-F10)
    {
        0xFF, 0*M_COLS+1, 0*M_COLS+2, 0*M_COLS+3, 0*M_COLS+4, 0*M_COLS+5,
        0*M_COLS+6, 0*M_COLS+7, 0*M_COLS+8, 0xFF, 0*M_COLS+9, 0*M_COLS+10
    },
#endif
    // Physical Row 1 (1, 2... =)
    {
        1*M_COLS+1, 1*M_COLS+2, 1*M_COLS+3, 1*M_COLS+4, 1*M_COLS+5, 1*M_COLS+6,
        1*M_COLS+7, 1*M_COLS+8, 1*M_COLS+9, 1*M_COLS+10, 1*M_COLS+11, 1*M_COLS+12
    },
    // Physical Row 2 (Q, W... ])
    {
        2*M_COLS+1, 2*M_COLS+2, 2*M_COLS+3, 2*M_COLS+4, 2*M_COLS+5, 2*M_COLS+6,
        2*M_COLS+7, 2*M_COLS+8, 2*M_COLS+9, 2*M_COLS+10, 2*M_COLS+11, 2*M_COLS+12
    },
    // Physical Row 3 (A, S... ')
    {
        3*M_COLS+1, 3*M_COLS+2, 3*M_COLS+3, 3*M_COLS+4, 3*M_COLS+5, 3*M_COLS+6,
        3*M_COLS+7, 3*M_COLS+8, 3*M_COLS+9, 3*M_COLS+10, 3*M_COLS+11,
#if REPEAT_ENABLED
        3*M_COLS+11
#else
        0xFF
#endif
    },
    // Physical Row 4 (Z, X... /)
    {
        4*M_COLS+2, 4*M_COLS+3, 4*M_COLS+4, 4*M_COLS+5, 4*M_COLS+6, 4*M_COLS+7,
        4*M_COLS+8, 4*M_COLS+9, 4*M_COLS+10, 4*M_COLS+11,
#if REPEAT_ENABLED
        4*M_COLS+11, 4*M_COLS+11
#else
        0xFF, 0xFF
#endif
    }
};

#ifndef GRID_MAP_INCLUDED
#define GRID_MAP_INCLUDED
// Helper to get LED index from matrix index
static inline uint8_t get_led_index_from_matrix(uint8_t matrix_idx) {
    if (matrix_idx == 0xFF) return NO_LED;
    uint8_t r = matrix_idx / M_COLS;
    uint8_t c = matrix_idx % M_COLS;
    if (r >= MATRIX_ROWS || c >= MATRIX_COLS) return NO_LED;
    return g_led_config.matrix_co[r][c];
}

// Get LED index for grid position (returns NO_LED if invalid)
static inline uint8_t grid_get_led(uint8_t y, uint8_t x) {
    uint8_t m_idx = pgm_read_byte(&GRID_MAP[y][x]);
    return (m_idx != 0xFF) ? get_led_index_from_matrix(m_idx) : NO_LED;
}

// Iterate over all grid positions
// Variables available in body: gx, gy
#define FOR_EACH_GRID_POS() \
    for (uint8_t gy = 0; gy < GRID_HEIGHT; ++gy) \
        for (uint8_t gx = 0; gx < GRID_WIDTH; ++gx)

// Iterate over all grid positions with valid LED
// Variables available in body: gx, gy, grid_led_index
#define FOR_EACH_GRID_LED() \
    for (uint8_t gy = 0; gy < GRID_HEIGHT; ++gy) \
        for (uint8_t gx = 0; gx < GRID_WIDTH; ++gx) \
            for (uint8_t grid_led_index = grid_get_led(gy, gx), _gonce = 1; \
                 _gonce && grid_led_index != NO_LED; \
                 _gonce = 0)

#endif // GRID_MAP_INCLUDED
