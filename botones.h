#ifndef BOTONES_GAME
#define BOTONES_GAME

    #include <ti/devices/msp/msp.h>
    #include <ti/driverlib/driverlib.h>
    #include "0_MyDefinitions/GPIODefinitions.h"
    #include "1_Drivers/LEDS.h"

    typedef struct{
        pin_t disparo;      // externo, a GND, pull-up interno (activo-bajo)
        pin_t pausa;        // externo, a GND, pull-up interno (activo-bajo)
        LEDS_t *placa;      // S1 (izquierda) y S2 (derecha) del profesor
        pin_t izquierda;   
        pin_t derecha;
        uint8_t prevDisparo;  // estados anteriores para flancos
        uint8_t prevPausa;
    }botones_t;

    extern void botones_init(botones_t *ptr);
    extern uint8_t botones_izquierda(botones_t *ptr);     // por NIVEL (mantener = moverse)
    extern uint8_t botones_derecha(botones_t *ptr);       // por NIVEL
    extern uint8_t botones_flancoDisparo(botones_t *ptr); // por FLANCO (1 solo al presionar)
    extern uint8_t botones_flancoPausa(botones_t *ptr);   // por FLANCO

#endif