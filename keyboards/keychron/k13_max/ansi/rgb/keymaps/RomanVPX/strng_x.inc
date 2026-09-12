#ifndef STRNG_X
    #error "STRNG_X must be defined before including strng_x.inc"
#endif

/*      Run keycode      String             */
STRNG_X(STRNG_C,         "t:camera"        )
STRNG_X(STRNG_R,         "t:renderer"      )
STRNG_X(STRNG_M,         "t:mesh"          )
STRNG_X(STRNG_L,         "t:light"         )
STRNG_X(STRNG_S,         "t:Shader"        )

// Run keycode should be addedd to STRINGS_LAYER in keymap.c

#undef STRNG_X
