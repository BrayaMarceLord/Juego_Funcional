#include "max7219.h"

volatile uint8_t i;

void max7219_init(max7219_t *ptr, uint8_t numMax7219)
{
    spi_init(&(ptr->mySPI));
    // set Intensity
    max7219_setIntensity(ptr, numMax7219, ptr->Intensity);
    // set scanLimit
    max7219_setScanLimit(ptr, numMax7219, ptr->scanLimit);
    // set decode
    max7219_setDecode(ptr, numMax7219, ptr->decode);
    // set normal operation mode
    max7219_normalOperation(ptr, numMax7219);
}

void max7219_sendtrama(max7219_t *ptr, uint8_t numMax7219, uint16_t trama)
{
    while(ptr->mySPI.SPI->STAT & 0x00000010){}     // Wait if SPI is busy
    ptr->mySPI.SPI->CTL1 |= ptr->mySPI.CD.COMANDS; // Set bytes for comand
    for(i=0x00; i < numMax7219; i++)          //  
    {
        if(i==(numMax7219-1))
        {   // Send CD (comand) signal after current trama
            while(ptr->mySPI.SPI->STAT & 0x00000010){}   // Wait because Transmiter is busy (BUSYbit=1)
            ptr->mySPI.SPI->CTL1 = 0x00000000; 
            ptr->mySPI.SPI->CTL1 = ptr->mySPI.FORMAT | ptr->mySPI.CD.ENABLE | 0x00000005 | CD_2_BYTES; 
        }
        spi_send(&(ptr->mySPI), trama);
    }
}

void max7219_displaytTest(max7219_t *ptr, uint8_t numMax7219)
{
    max7219_sendtrama(ptr, numMax7219, DisplayTestADR|DisplayTestMode);
} 

void max7219_normalOperation(max7219_t *ptr, uint8_t numMax7219)
{
    max7219_sendtrama(ptr, numMax7219, DisplayTestADR|NormalOperation);
    max7219_sendtrama(ptr, numMax7219, ShutdownADR|NormalOperationMode);
}

void max7219_shutDown(max7219_t *ptr, uint8_t numMax7219)
{
    max7219_sendtrama(ptr, numMax7219, ShutdownADR|ShutdownMode);
}

void max7219_setIntensity(max7219_t *ptr, uint8_t numMax7219, uint16_t intensity)
{
    max7219_sendtrama(ptr, numMax7219, IntensityADR|intensity);
}

void max7219_setScanLimit(max7219_t *ptr, uint8_t numMax7219, uint16_t scanLimit)
{
    max7219_sendtrama(ptr, numMax7219, ScanLimitADR|scanLimit);
}

void max7219_setDecode(max7219_t *ptr, uint8_t numMax7219, uint16_t decode)
{
    max7219_sendtrama(ptr, numMax7219, DecModeADR|decode);
}
