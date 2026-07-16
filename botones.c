#include "botones.h"

static void botones_configEntradaPullUp(pin_t *p)
{
    // Mismo patron que el pulsador[1] (S2) de LEDS_Init del profesor
    if(p->port == PORTB){
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOB(p->pin)] = IOMUX_PINCM_INENA_ENABLE | IOMUX_PINCM_PC_CONNECTED | ((uint32_t)0x00000001);
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOB(p->pin)] |= DL_GPIO_INVERSION_DISABLE | DL_GPIO_RESISTOR_PULL_UP | DL_GPIO_HYSTERESIS_DISABLE | DL_GPIO_WAKEUP_DISABLE | IOMUX_PINCM_WUEN_DISABLE;
    }else{
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOA(p->pin)] = IOMUX_PINCM_INENA_ENABLE | IOMUX_PINCM_PC_CONNECTED | ((uint32_t)0x00000001);
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOA(p->pin)] |= DL_GPIO_INVERSION_DISABLE | DL_GPIO_RESISTOR_PULL_UP | DL_GPIO_HYSTERESIS_DISABLE | DL_GPIO_WAKEUP_DISABLE | IOMUX_PINCM_WUEN_DISABLE;
    }
}

void botones_init(botones_t *ptr)
{
    // GPIOA/GPIOB ya quedan energizados por LEDS_Init (se llama antes en main)
    botones_configEntradaPullUp(&ptr->disparo);
    botones_configEntradaPullUp(&ptr->pausa);
    botones_configEntradaPullUp(&ptr->izquierda);  
    botones_configEntradaPullUp(&ptr->derecha);
    ptr->prevDisparo = 0;
    ptr->prevPausa   = 0;
}



static uint8_t botones_leerActivoBajo(pin_t *p)
{
    uint32_t din = (p->port == PORTB) ? GPIOB->DIN31_0 : GPIOA->DIN31_0;
    return ((~din) & p->pin) ? 1 : 0;
}

uint8_t botones_izquierda(botones_t *ptr)
{
    return botones_leerActivoBajo(&ptr->izquierda);
}

uint8_t botones_derecha(botones_t *ptr)
{
    return botones_leerActivoBajo(&ptr->derecha);
}

uint8_t botones_flancoDisparo(botones_t *ptr)
{
    uint8_t ahora = botones_leerActivoBajo(&ptr->disparo);
    uint8_t flanco = (ahora && !ptr->prevDisparo);  // subida logica: se acaba de presionar
    ptr->prevDisparo = ahora;
    return flanco;
}

uint8_t botones_flancoPausa(botones_t *ptr)
{
    uint8_t ahora = botones_leerActivoBajo(&ptr->pausa);
    uint8_t flanco = (ahora && !ptr->prevPausa);
    ptr->prevPausa = ahora;
    return flanco;
}