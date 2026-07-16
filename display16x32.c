#include "display16x32.h"

volatile uint16_t i, j;

void display16x32_init(display16x32_t *ptr)
{
    ptr->myMax7219.decode = NoDecode;
    ptr->myMax7219.scanLimit = DisplayDigit_0_to_7;
    ptr->myMax7219.mySPI.SIZE = BITS16;
    ptr->myMax7219.mySPI.FORMAT = SPI_MSB_FIRST;
    ptr->myMax7219.mySPI.CS = CHIP_SELECT_3;
    ptr->myMax7219.mySPI.CD.ENABLE = CS3_AS_CD; 
    ptr->myMax7219.mySPI.CD.COMANDS = CD_MANUAL;
    ptr->numMax7219 = 8;

    spi_init(&(ptr->myMax7219.mySPI));
    max7219_setIntensity(&(ptr->myMax7219), ptr->numMax7219, ptr->myMax7219.Intensity);
    max7219_setScanLimit(&(ptr->myMax7219), ptr->numMax7219, ptr->myMax7219.scanLimit);
    max7219_setDecode(&(ptr->myMax7219), ptr->numMax7219, ptr->myMax7219.decode);
    display16x32_clearDisplay(ptr);
    max7219_normalOperation(&(ptr->myMax7219), ptr->numMax7219);
}



void display16x32_sendFrame(display16x32_t *ptr)
{
    for(i=0x0000; i < 0x0008; i++) // for each column in each max7219
    {
        while(ptr->myMax7219.mySPI.SPI->STAT & 0x00000010){}     // Wait if SPI is busy
        ptr->myMax7219.mySPI.SPI->CTL1 |= ptr->myMax7219.mySPI.CD.COMANDS; // Set bytes for comand
        for(j=0x0000; j < ptr->numMax7219;j++) // for each max7219
        {
            if(j==(ptr->numMax7219-1))
            {
                while(ptr->myMax7219.mySPI.SPI->STAT & 0x00000010){}
                ptr->myMax7219.mySPI.SPI->CTL1 = 0x00000000; 
                ptr->myMax7219.mySPI.SPI->CTL1 = SPI_MSB_FIRST | CS3_AS_CD | 0x00000005 | CD_2_BYTES;
            }
            if(j<4)
            {
                spi_send(&(ptr->myMax7219.mySPI), ((i+1)<<8) | (uint16_t)(ptr->myImage.quarterCols[35-j+(i<<2)])); //MSB
            }else{
                spi_send(&(ptr->myMax7219.mySPI), ((i+1)<<8) | (uint16_t)(reverse(ptr->myImage.quarterCols[24+j-(i<<2)]))); //LSB
            }
        }
    }
}

uint8_t reverse(uint8_t x)
{
    x = ((x & 0b11110000) >> 4) | ((x & 0b00001111) << 4);
    x = ((x & 0b11001100) >> 2) | ((x & 0b00110011) << 2);
    x = ((x & 0b10101010) >> 1) | ((x & 0b01010101) << 1);
    return x;
}


void display16x32_setIntensity(display16x32_t *ptr, uint16_t intensity)
{
    ptr->myMax7219.Intensity = intensity;
    max7219_setIntensity(&(ptr->myMax7219), ptr->numMax7219, intensity);
}


void display16x32_clearDisplay(display16x32_t *ptr)
{
    for(j=0x01;j<=0x08; j++)
    {
        for(i=0x00; i<ptr->numMax7219; i++)
        {
            max7219_sendtrama(&(ptr->myMax7219), ptr->numMax7219, ((j<<8) | 0x0000));
        }
    }     
}



void display16x32_test(display16x32_t *ptr)
{
    max7219_displaytTest(&(ptr->myMax7219), ptr->numMax7219);
}

void display16x32_normalOperation(display16x32_t *ptr)
{
    max7219_normalOperation(&(ptr->myMax7219), ptr->numMax7219);
}

void display16x32_shutDown(display16x32_t *ptr)
{
    max7219_shutDown(&(ptr->myMax7219), ptr->numMax7219);
}

