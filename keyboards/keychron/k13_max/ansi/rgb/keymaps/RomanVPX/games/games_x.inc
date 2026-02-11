#ifndef GAME_X
    #error "GAME_X must be defined before including games_x.inc"
#endif

/*     Run keycode  function name prefix   score type */
GAME_X(RUN_SNAKE,       snake,              uint16_t)
GAME_X(RUN_LIFE,        life,               void    )
GAME_X(RUN_DINO,        dino,               uint16_t)

#undef GAME_X
