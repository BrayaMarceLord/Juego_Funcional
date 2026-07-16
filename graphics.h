#include "stdint.h"

#ifndef GRAPHICS
    #define GRAPHICS

    typedef union{
        volatile uint32_t cols[16];          //32 bit for each column
        volatile uint16_t halfCols[32];      //2 16 bits for each column
        volatile uint8_t  quarterCols[64];   //4 8 bits for each column
    }imagen16x32_t;

    extern void display16x32_shitfDownToUp(imagen16x32_t *ptr); 
    extern void display16x32_shitfUpToDown(imagen16x32_t *ptr); 
    extern void display16x32_shitfLeftToRight(imagen16x32_t *ptr);
    extern void display16x32_shitfRightToLeft(imagen16x32_t *ptr); 

#endif 