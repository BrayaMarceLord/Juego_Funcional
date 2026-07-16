#include "graphics.h"

void display16x32_shitfDownToUp(imagen16x32_t *ptr)
{
    uint32_t temp = ptr->cols[0];
    uint8_t i;
    for(i=0x00; i<=14; i++)
    {

        ptr->cols[i] = ptr->cols[i+1];
    }
    ptr->cols[15] = temp;
}

void display16x32_shitfUpToDown(imagen16x32_t *ptr)
{
    uint32_t temp = ptr->cols[15];
    uint8_t i;
    for(i=0x00; i<=14; i++)
    {
        ptr->cols[15-i] = ptr->cols[14-i];
    }
    ptr->cols[0] = temp;
}

void display16x32_shitfLeftToRight(imagen16x32_t *ptr)
{
    uint8_t i;
    for(i=0x00;i<=15;i++)
    {
        (ptr->cols[i]&0x00000001) ? (ptr->cols[i] = (ptr->cols[i]>>1)|0x80000000) : (ptr->cols[i] = (ptr->cols[i]>>1));
    }    
}

void display16x32_shitfRightToLeft(imagen16x32_t *ptr)
{
    uint8_t i;
    for(i=0x00;i<=15;i++)
    {
        (ptr->cols[i]&0x80000000) ? (ptr->cols[i] = (ptr->cols[i]<<1)|0x00000001) : (ptr->cols[i] = (ptr->cols[i]<<1));
    }
}