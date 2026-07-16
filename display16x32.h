#include "2_HAL/max7219.h"
#include "4_APIs/graphics.h"

#ifndef DISPLAY_16X32
    #define DISPLAY_16X32

    typedef struct {
        max7219_t myMax7219;
        uint8_t numMax7219;
        imagen16x32_t myImage;
    }display16x32_t;


    extern void display16x32_init(display16x32_t *ptr);

    extern void display16x32_test(display16x32_t *ptr);
    extern void display16x32_normalOperation(display16x32_t *ptr);
    extern void display16x32_shutDown(display16x32_t *ptr);
    extern void display16x32_setIntensity(display16x32_t *ptr, uint16_t intensity);

    extern void display16x32_clearDisplay(display16x32_t *ptr);
    extern void display16x32_sendFrame(display16x32_t *ptr);
    extern uint8_t reverse(uint8_t x);

#endif 