#include <ti/devices/msp/msp.h>
#include <0_MyDefinitions/GPIODefinitions.h>

#ifndef SPI_DRIVER
#define SPI_DRIVER

    #define RSTKEY 0xB1000000
    #define PWRKEY 0x26000000

    typedef enum{
        MCLK_SEL = 0x00000008,
        MFCLK_SEL = 0x00000004,
        LFCLK_SEL = 0x00000002,
    }CLOCK_SOURCE;

    typedef enum{
        DIV_BY_1 = 0x00000000,
        DIV_BY_2 = 0x00000001,
        DIV_BY_3 = 0x00000002,
        DIV_BY_4 = 0x00000003,
        DIV_BY_5 = 0x00000004,
        DIV_BY_6 = 0x00000005,
        DIV_BY_7 = 0x00000006,
        DIV_BY_8 = 0x00000007,
    }CLOCK_DIV;

    typedef enum{
        SPI_LSB_FIRST = 0x00000000,
        SPI_MSB_FIRST = 0x00000010,
    }DATA_FORMAT;

    typedef enum{
        BITS4 = 0x00000003,
        BITS5 = 0x00000004,
        BITS6 = 0x00000005,
        BITS7 = 0x00000006,
        BITS8 = 0x00000007,
        BITS9 = 0x00000008,
        BITS10 = 0x00000009,
        BITS11 = 0x0000000A,
        BITS12 = 0x0000000B,
        BITS13 = 0x0000000C,
        BITS14 = 0x0000000D,
        BITS15 = 0x0000000E,
        BITS16 = 0x0000000F,
    }DATA_SIZE;

    typedef enum{
        CHIP_SELECT_0 = 0x00000000,
        CHIP_SELECT_1 = 0x00001000,
        CHIP_SELECT_2 = 0x00002000,
        CHIP_SELECT_3 = 0x00003000,
    }CSS;

    typedef enum{
        CS3_AS_CS = 0x00000000,
        CS3_AS_CD = 0x00000800,
    }CD_ENABLE;

    typedef enum{
        CD_0_BYTES  = 0x00000000,
        CD_1_BYTES  = 0x00001000,
        CD_2_BYTES  = 0x00002000,
        CD_3_BYTES  = 0x00003000,
        CD_4_BYTES  = 0x00004000,
        CD_5_BYTES  = 0x00005000,
        CD_6_BYTES  = 0x00006000,
        CD_7_BYTES  = 0x00007000,
        CD_8_BYTES  = 0x00008000,
        CD_9_BYTES  = 0x00009000,
        CD_10_BYTES = 0x0000A000,
        CD_11_BYTES = 0x0000B000,
        CD_12_BYTES = 0x0000C000,
        CD_13_BYTES = 0x0000D000,
        CD_14_BYTES = 0x0000E000,
        CD_MANUAL   = 0x0000F000,
    }CD_MODE;

    typedef struct {
        CD_ENABLE ENABLE;
        CD_MODE COMANDS;
    }CD_t;

    typedef struct {
        CLOCK_SOURCE SOURCE;
        CLOCK_DIV DIV;
    }clk_t;

    typedef struct{
        SPI_Regs *SPI;
        pin_t PICO;     // Peripheral input-Controller output pin
        pin_t POCI;     // Peripheral output-Controller input pin
        pin_t SCLK;     // Clock pin
        pin_t CS0;      // Chip select signal 0
        pin_t CS1;      // Chip select signal 1
        pin_t CS2;      // Chip select signal 2
        pin_t CS3;      // Chip select signal 3 or comand signal
        clk_t CLK;      // Clock configuration
        CD_t  CD;       // for comand mode with CS3 as CD
        DATA_FORMAT FORMAT;    // LSB first or MSB first
        DATA_SIZE SIZE;        // Trama size
        CSS CS;         // Select the CS line
    }spi_t;

    extern void spi_init(spi_t * ptr);
    extern void spi_send(spi_t *ptr, uint16_t trama);

#endif 