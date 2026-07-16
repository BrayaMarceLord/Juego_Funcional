#include "lfsr.h"

void lfsr_init(lfsr_t *ptr, uint16_t semilla)
{
    ptr->estado = (semilla == 0) ? 0xACE1 : semilla;  // 0 bloquea el LFSR
}

uint16_t lfsr_next(lfsr_t *ptr)
{
    // Polinomio x^16 + x^14 + x^13 + x^11 + 1 (periodo maximo)
    uint16_t bit = ((ptr->estado >> 0) ^ (ptr->estado >> 2) ^
                    (ptr->estado >> 3) ^ (ptr->estado >> 5)) & 0x0001;
    ptr->estado = (ptr->estado >> 1) | (bit << 15);
    return ptr->estado;
}

uint16_t lfsr_rango(lfsr_t *ptr, uint16_t max)
{
    return lfsr_next(ptr) % max;
}