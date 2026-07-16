#ifndef SFX_H
#define SFX_H

    #include "5_Game/tones.h"
    #include "stdint.h"

    // disparo: pip corto agudo
    static const uint16_t sfx_disparo[]     = { NOTE_E6 };
    static const int8_t   sfx_disparo_d[]   = { 32 };


    
    // impacto a enemigo: dos notas descendentes
    static const uint16_t sfx_impacto[]     = { NOTE_C5, NOTE_G4 };
    static const int8_t   sfx_impacto_d[]   = { 16, 16 };

    // perder una vida
    static const uint16_t sfx_vida[]        = { NOTE_G4, NOTE_DS4 };
    static const int8_t   sfx_vida_d[]      = { 8, 8 };

    // game over: descendente
    static const uint16_t sfx_gameover[]    = { NOTE_C5, NOTE_G4, NOTE_E4, NOTE_C4 };
    static const int8_t   sfx_gameover_d[]  = { 8, 8, 8, 4 };

    // victoria: ascendente
    static const uint16_t sfx_victoria[]    = { NOTE_C5, NOTE_E5, NOTE_G5, NOTE_C6 };
    static const int8_t   sfx_victoria_d[]  = { 8, 8, 8, 4 };

#endif // SFX_H