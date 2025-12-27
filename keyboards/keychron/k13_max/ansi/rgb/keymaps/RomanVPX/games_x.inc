#ifndef GAME_X
    #error "GAME_X must be defined before including games_x.inc"
#endif

/*     Run keycode  function name prefix   */
GAME_X(RUN_SNAKE,       snake               )
GAME_X(RUN_LIFE,        life                )
GAME_X(RUN_DINO,        dino                )

#undef GAME_X
