#include "LEDS.h"

void LEDS_Init(LEDS_t* ptr)
{

    GPIOA->GPRCM.RSTCTL = (GPIO_RSTCTL_KEY_UNLOCK_W | GPIO_RSTCTL_RESETSTKYCLR_CLR | GPIO_RSTCTL_RESETASSERT_ASSERT);
    GPIOB->GPRCM.RSTCTL = (GPIO_RSTCTL_KEY_UNLOCK_W | GPIO_RSTCTL_RESETSTKYCLR_CLR | GPIO_RSTCTL_RESETASSERT_ASSERT);

    GPIOA->GPRCM.PWREN = (GPIO_PWREN_KEY_UNLOCK_W | GPIO_PWREN_ENABLE_ENABLE);
    GPIOB->GPRCM.PWREN = (GPIO_PWREN_KEY_UNLOCK_W | GPIO_PWREN_ENABLE_ENABLE);
    delay_cycles(16);

    //configurar pines de entrada
    if(ptr->pulsadores_port[0]){
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOB(ptr->pulsadores[0])] = IOMUX_PINCM_INENA_ENABLE | IOMUX_PINCM_PC_CONNECTED |((uint32_t) 0x00000001);
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOB(ptr->pulsadores[0])] |= DL_GPIO_INVERSION_DISABLE| DL_GPIO_RESISTOR_NONE |DL_GPIO_HYSTERESIS_DISABLE| DL_GPIO_WAKEUP_DISABLE|IOMUX_PINCM_WUEN_DISABLE;
    }else{
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOA(ptr->pulsadores[0])] = IOMUX_PINCM_INENA_ENABLE | IOMUX_PINCM_PC_CONNECTED |((uint32_t) 0x00000001);
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOA(ptr->pulsadores[0])] |= DL_GPIO_INVERSION_DISABLE| DL_GPIO_RESISTOR_NONE |DL_GPIO_HYSTERESIS_DISABLE| DL_GPIO_WAKEUP_DISABLE|IOMUX_PINCM_WUEN_DISABLE;
    }
    
    if(ptr->pulsadores_port[1]){
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOB(ptr->pulsadores[1])] = IOMUX_PINCM_INENA_ENABLE | IOMUX_PINCM_PC_CONNECTED |((uint32_t) 0x00000001);
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOB(ptr->pulsadores[1])] |= DL_GPIO_INVERSION_DISABLE| DL_GPIO_RESISTOR_PULL_UP |DL_GPIO_HYSTERESIS_DISABLE| DL_GPIO_WAKEUP_DISABLE|IOMUX_PINCM_WUEN_DISABLE;
    }else{
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOA(ptr->pulsadores[1])] = IOMUX_PINCM_INENA_ENABLE | IOMUX_PINCM_PC_CONNECTED |((uint32_t) 0x00000001);
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOA(ptr->pulsadores[1])] |= DL_GPIO_INVERSION_DISABLE| DL_GPIO_RESISTOR_PULL_UP |DL_GPIO_HYSTERESIS_DISABLE| DL_GPIO_WAKEUP_DISABLE|IOMUX_PINCM_WUEN_DISABLE;
    }

    // configurar pines de salida
    if(ptr->leds_port[0]){
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOB(ptr->leds[0])] = (IOMUX_PINCM_PC_CONNECTED | ((uint32_t) 0x00000001));
    }else{
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOA(ptr->leds[0])] = (IOMUX_PINCM_PC_CONNECTED | ((uint32_t) 0x00000001));
    }

        if(ptr->leds_port[1]){
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOB(ptr->leds[1])] = (IOMUX_PINCM_PC_CONNECTED | ((uint32_t) 0x00000001));
    }else{
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOA(ptr->leds[1])] = (IOMUX_PINCM_PC_CONNECTED | ((uint32_t) 0x00000001));
    }

        if(ptr->leds_port[2]){
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOB(ptr->leds[2])] = (IOMUX_PINCM_PC_CONNECTED | ((uint32_t) 0x00000001));
    }else{
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOA(ptr->leds[2])] = (IOMUX_PINCM_PC_CONNECTED | ((uint32_t) 0x00000001));
    }

        if(ptr->leds_port[3]){
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOB(ptr->leds[3])] = (IOMUX_PINCM_PC_CONNECTED | ((uint32_t) 0x00000001));
    }else{
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOA(ptr->leds[3])] = (IOMUX_PINCM_PC_CONNECTED | ((uint32_t) 0x00000001));
    }

    // clear output pins
    (ptr->leds_port[0])?(GPIOB->DOUTCLR31_0 = ptr->leds[0]):(GPIOA->DOUTCLR31_0 = ptr->leds[0]);
    (ptr->leds_port[1])?(GPIOB->DOUTCLR31_0 = ptr->leds[1]):(GPIOA->DOUTCLR31_0 = ptr->leds[1]);
    (ptr->leds_port[2])?(GPIOB->DOUTCLR31_0 = ptr->leds[2]):(GPIOA->DOUTCLR31_0 = ptr->leds[2]);
    (ptr->leds_port[3])?(GPIOB->DOUTCLR31_0 = ptr->leds[3]):(GPIOA->DOUTCLR31_0 = ptr->leds[3]);

    // enable output pins
    (ptr->leds_port[0])?(GPIOB->DOESET31_0 = ptr->leds[0]):(GPIOA->DOESET31_0 = ptr->leds[0]);
    (ptr->leds_port[1])?(GPIOB->DOESET31_0 = ptr->leds[1]):(GPIOA->DOESET31_0 = ptr->leds[1]);
    (ptr->leds_port[2])?(GPIOB->DOESET31_0 = ptr->leds[2]):(GPIOA->DOESET31_0 = ptr->leds[2]);
    (ptr->leds_port[3])?(GPIOB->DOESET31_0 = ptr->leds[3]):(GPIOA->DOESET31_0 = ptr->leds[3]);

}

void LEDS_ShiftLeft(LEDS_t* ptr)
{
    static uint8_t i = 0x04;
    (ptr->leds_port[0])?(GPIOB->DOUTCLR31_0 = ptr->leds[0]):(GPIOA->DOUTCLR31_0 = ptr->leds[0]);
    (ptr->leds_port[1])?(GPIOB->DOUTCLR31_0 = ptr->leds[1]):(GPIOA->DOUTCLR31_0 = ptr->leds[1]);
    (ptr->leds_port[2])?(GPIOB->DOUTCLR31_0 = ptr->leds[2]):(GPIOA->DOUTCLR31_0 = ptr->leds[2]);
    (ptr->leds_port[3])?(GPIOB->DOUTCLR31_0 = ptr->leds[3]):(GPIOA->DOUTCLR31_0 = ptr->leds[3]);
    
    if(i>0x00)
    {
        i--;
    }else{
        i=0x03;
    } 
    (ptr->leds_port[i])?(GPIOB->DOUTSET31_0 = ptr->leds[i]):(GPIOA->DOUTSET31_0 = ptr->leds[i]);
}

void LEDS_ShiftRight(LEDS_t* ptr)
{
    static uint8_t i = 0x00;
    (ptr->leds_port[0])?(GPIOB->DOUTCLR31_0 = ptr->leds[0]):(GPIOA->DOUTCLR31_0 = ptr->leds[0]);
    (ptr->leds_port[1])?(GPIOB->DOUTCLR31_0 = ptr->leds[1]):(GPIOA->DOUTCLR31_0 = ptr->leds[1]);
    (ptr->leds_port[2])?(GPIOB->DOUTCLR31_0 = ptr->leds[2]):(GPIOA->DOUTCLR31_0 = ptr->leds[2]);
    (ptr->leds_port[3])?(GPIOB->DOUTCLR31_0 = ptr->leds[3]):(GPIOA->DOUTCLR31_0 = ptr->leds[3]);
    
    if(i<=0x03)
    {
        i++;
    }else{
        i=0x00;
    } 
    (ptr->leds_port[i])?(GPIOB->DOUTSET31_0 = ptr->leds[i]):(GPIOA->DOUTSET31_0 = ptr->leds[i]);
}

uint32_t LEDS_estadoS1(LEDS_t* ptr)
{
    uint32_t a;
    if(ptr->pulsadores_port[0])
    {
        a = (GPIOB->DIN31_0) & (ptr->pulsadores[0]);
    }else{
        a = (GPIOA->DIN31_0) & (ptr->pulsadores[0]);
    }
    return a;
}

uint32_t LEDS_estadoS2(LEDS_t* ptr)
{
    uint32_t b;
    if(ptr->pulsadores_port[1] == PORTB)
    {
        b = ~(GPIOB->DIN31_0) & ptr->pulsadores[1];
    }else{
        b = ~(GPIOA->DIN31_0) & ptr->pulsadores[1];
    }
    return b;
}


