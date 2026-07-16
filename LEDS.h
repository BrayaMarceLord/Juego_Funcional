#ifndef LEDS_DRIVER
#define LEDS_DRIVER

#include <ti/driverlib/driverlib.h>
#include <ti/devices/msp/msp.h>
#include <0_MyDefinitions/GPIODefinitions.h>

typedef struct{
    uint32_t leds[4];
    uint8_t leds_port[4];
    uint32_t pulsadores[2];
    uint8_t pulsadores_port[2]; 
}LEDS_t;

extern void LEDS_Init(LEDS_t* ptr);
extern void LEDS_ShiftLeft(LEDS_t* ptr);
extern void LEDS_ShiftRight(LEDS_t* ptr);
extern uint32_t LEDS_estadoS1(LEDS_t* ptr);
extern uint32_t LEDS_estadoS2(LEDS_t* ptr);

#endif