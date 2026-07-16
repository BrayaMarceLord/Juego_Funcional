#ifndef buzzer_H
#define buzzer_H

    #include "ti_msp_dl_config.h"
    #include "stdint.h"
    #include "stdbool.h"

    // numero de elementos de un arreglo de notas/duraciones
    #define BUZZER_LEN(a)  ((uint8_t)(sizeof(a)/sizeof((a)[0])))

    void buzzer_init(void);   // llamar una vez tras SYSCFG_DL_init()

    // ---- Voz de EFECTOS (PWM_0, buzzer 1) ----
    // misma firma que antes: game.c NO cambia
    void buzzer_play(const uint16_t *notes, const int8_t *durations,
                     uint8_t len, uint8_t tempo, bool loop);
    void buzzer_stop(void);
    bool buzzer_busy(void);

    // ---- Voz de MUSICA (PWM_1, buzzer 2) ----
    void buzzer_music_play(const uint16_t *notes, const int8_t *durations,
                           uint8_t len, uint8_t tempo, bool loop);
    void buzzer_music_stop(void);
    bool buzzer_music_busy(void);

#endif // BUZZER_H