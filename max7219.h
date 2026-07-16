#include "1_Drivers/spi.h"

#ifndef MAX_7219
#define MAX_7219

    typedef enum {
        NoOpADR        =  0x0000,
        Digit0ADR      =  0x0100,
        Digit1ADR      =  0x0200,
        Digit2ADR      =  0x0300,
        Digit3ADR      =  0x0400,
        Digit4ADR      =  0x0500,
        Digit5ADR      =  0x0600,
        Digit6ADR      =  0x0700,
        Digit7ADR      =  0x0800,
        DecModeADR     =  0x0900,
        IntensityADR   =  0x0A00,
        ScanLimitADR   =  0x0B00,
        ShutdownADR    =  0x0C00,
        DisplayTestADR =  0x0F00,
    }REG_ADDRESS;    

    // Test data definitions
    typedef enum{
        NormalOperation = 0x0000,
        DisplayTestMode = 0x0001,
    }TEST;

    // Shutdown data definitions 
    typedef enum{
        NormalOperationMode = 0x0001,
        ShutdownMode        = 0x0000,
    }OPERATION;

    
    // Decode mode definitions
    typedef enum{
        NoDecode = 0x0000, 
        Bcode    = 0x00FF,
    }DECODE;


    // scan limit display data definitions
    typedef enum{
        DisplayDigit_0_Only = 0x0000,
        DisplayDigit_0_to_1 = 0x0001,
        DisplayDigit_0_to_2 = 0x0002,
        DisplayDigit_0_to_3 = 0x0003,
        DisplayDigit_0_to_4 = 0x0004,
        DisplayDigit_0_to_5 = 0x0005,
        DisplayDigit_0_to_6 = 0x0006,
        DisplayDigit_0_to_7 = 0x0007, 
    }SCAN_LIMIT;


    // Intensity data definitions
    typedef enum{
        Intensity_1   =  0x0000,
        Intensity_2   =  0x0001,
        Intensity_3   =  0x0002,
        Intensity_4   =  0x0003,
        Intensity_5   =  0x0004,
        Intensity_6   =  0x0005,
        Intensity_7   =  0x0006,
        Intensity_8   =  0x0007,
        Intensity_9   =  0x0008,
        Intensity_10  =  0x0009,
        Intensity_11  =  0x000A,
        Intensity_12  =  0x000B,
        Intensity_13  =  0x000C,
        Intensity_14  =  0x000D,
        Intensity_15  =  0x000E,
        Intensity_16  =  0x000F,
    }INTENSITY;

    typedef struct{
        spi_t mySPI;
        INTENSITY Intensity;
        SCAN_LIMIT scanLimit;
        DECODE decode;
    }max7219_t;

    extern void max7219_init(max7219_t *ptr, uint8_t numMax7219);
    extern void max7219_sendtrama(max7219_t *ptr, uint8_t numMax7219, uint16_t trama);

    extern void max7219_displaytTest(max7219_t *ptr, uint8_t numMax7219);
    extern void max7219_normalOperation(max7219_t *ptr, uint8_t numMax7219);
    extern void max7219_shutDown(max7219_t *ptr, uint8_t numMax7219);
    extern void max7219_setIntensity(max7219_t *ptr, uint8_t numMax7219, uint16_t intensity);
    extern void max7219_setScanLimit(max7219_t *ptr, uint8_t numMax7219, uint16_t scanLimit);
    extern void max7219_setDecode(max7219_t *ptr, uint8_t numMax7219, uint16_t decode);
#endif 