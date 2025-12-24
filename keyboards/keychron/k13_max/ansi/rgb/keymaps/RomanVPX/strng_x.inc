#ifndef STRNG_X
    #error "STRNG_X must be defined before including strng.inc"
#endif

STRNG_X(STRNG_C, "t:camera")
STRNG_X(STRNG_R, "t:renderer")
STRNG_X(STRNG_M, "t:mesh")
STRNG_X(STRNG_S, "t:Shader")

#undef STRNG_X
