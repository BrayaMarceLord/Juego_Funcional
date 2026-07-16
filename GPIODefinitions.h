
#ifndef GPIO_DEFINITIONS
#define GPIO_DEFINITIONS

    #include "stdint.h"

    typedef struct{
        const uint32_t pin;
        const uint8_t port;
    }pin_t;

    #define PA0  0x00000001U
    #define PA1  0x00000002U
    #define PA2  0x00000004U
    #define PA3  0x00000008U
    #define PA4  0x00000010U
    #define PA5  0x00000020U
    #define PA6  0x00000040U
    #define PA7  0x00000080U
    #define PA8  0x00000100U
    #define PA9  0x00000200U
    #define PA10 0x00000400U
    #define PA11 0x00000800U
    #define PA12 0x00001000U
    #define PA13 0x00002000U
    #define PA14 0x00004000U
    #define PA15 0x00008000U
    #define PA16 0x00010000U
    #define PA17 0x00020000U
    #define PA18 0x00040000U
    #define PA19 0x00080000U
    #define PA20 0x00100000U
    #define PA21 0x00200000U
    #define PA22 0x00400000U
    #define PA23 0x00800000U
    #define PA24 0x01000000U
    #define PA25 0x02000000U
    #define PA26 0x04000000U
    #define PA27 0x08000000U
    #define PA28 0x10000000U
    #define PA29 0x20000000U
    #define PA30 0x40000000U
    #define PA31 0x80000000U

    #define PB0  0x00000001U
    #define PB1  0x00000002U
    #define PB2  0x00000004U
    #define PB3  0x00000008U
    #define PB4  0x00000010U
    #define PB5  0x00000020U
    #define PB6  0x00000040U
    #define PB7  0x00000080U
    #define PB8  0x00000100U
    #define PB9  0x00000200U
    #define PB10 0x00000400U
    #define PB11 0x00000800U
    #define PB12 0x00001000U
    #define PB13 0x00002000U
    #define PB14 0x00004000U
    #define PB15 0x00008000U
    #define PB16 0x00010000U
    #define PB17 0x00020000U
    #define PB18 0x00040000U
    #define PB19 0x00080000U
    #define PB20 0x00100000U
    #define PB21 0x00200000U
    #define PB22 0x00400000U
    #define PB23 0x00800000U
    #define PB24 0x01000000U
    #define PB25 0x02000000U
    #define PB26 0x04000000U
    #define PB27 0x08000000U

    #define PORTA 0x00
    #define PORTB 0x01

    // define macro to get pincm for GPIOA
    #define GET_PINCM_GPIOA(x) ( \
        ((x) & PA0 ) ? 0  : \
        ((x) & PA1 ) ? 1  : \
        ((x) & PA2 ) ? 6  : \
        ((x) & PA3 ) ? 7  : \
        ((x) & PA4 ) ? 8  : \
        ((x) & PA5 ) ? 9  : \
        ((x) & PA6 ) ? 10  :\
        ((x) & PA7 ) ? 13 : \
        ((x) & PA8 ) ? 18 : \
        ((x) & PA9 ) ? 19 : \
        ((x) & PA10) ? 20 : \
        ((x) & PA11) ? 21 : \
        ((x) & PA12) ? 33 : \
        ((x) & PA13) ? 34 : \
        ((x) & PA14) ? 35 : \
        ((x) & PA15) ? 36 : \
        ((x) & PA16) ? 37 : \
        ((x) & PA17) ? 38 : \
        ((x) & PA18) ? 39 : \
        ((x) & PA19) ? 40 : \
        ((x) & PA20) ? 41 : \
        ((x) & PA21) ? 45 : \
        ((x) & PA22) ? 46 : \
        ((x) & PA23) ? 52 : \
        ((x) & PA24) ? 53 : \
        ((x) & PA25) ? 54 : \
        ((x) & PA26) ? 58 : \
        ((x) & PA27) ? 59 : \
        ((x) & PA28) ? 2 : \
        ((x) & PA29) ? 3 : \
        ((x) & PA30) ? 4 : \
        ((x) & PA31) ? 5 : \
        100 /* Invalid input: no bit set */ \
    )

    // define macro to get pincm for GPIOA
    #define GET_PINCM_GPIOB(x) ( \
        ((x) & PB0 ) ? 11  : \
        ((x) & PB1 ) ? 12  : \
        ((x) & PB2 ) ? 14  : \
        ((x) & PB3 ) ? 15  : \
        ((x) & PB4 ) ? 16 : \
        ((x) & PB5 ) ? 17 : \
        ((x) & PB6 ) ? 22  :\
        ((x) & PB7 ) ? 23 : \
        ((x) & PB8 ) ? 24 : \
        ((x) & PB9 ) ? 25 : \
        ((x) & PB10) ? 26 : \
        ((x) & PB11) ? 27 : \
        ((x) & PB12) ? 28 : \
        ((x) & PB13) ? 29 : \
        ((x) & PB14) ? 30 : \
        ((x) & PB15) ? 31 : \
        ((x) & PB16) ? 32 : \
        ((x) & PB17) ? 42 : \
        ((x) & PB18) ? 43 : \
        ((x) & PB19) ? 44 : \
        ((x) & PB20) ? 47 : \
        ((x) & PB21) ? 48 : \
        ((x) & PB22) ? 49 : \
        ((x) & PB23) ? 50 : \
        ((x) & PB24) ? 51 : \
        ((x) & PB25) ? 55 : \
        ((x) & PB26) ? 56 : \
        ((x) & PB27) ? 57 : \
        100 /* Invalid input: no bit set */ \
    )


#endif // GPIO_DEFINITIONS
