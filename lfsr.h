#ifndef LFSR_PRNG
#define LFSR_PRNG

    #include "stdint.h"

    typedef struct{
        uint16_t estado;
    }lfsr_t;

    extern void lfsr_init(lfsr_t *ptr, uint16_t semilla);
    extern uint16_t lfsr_next(lfsr_t *ptr);
    extern uint16_t lfsr_rango(lfsr_t *ptr, uint16_t max);  // [0, max)

#endif