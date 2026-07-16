#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#define UART_0_IBRD  (92)
#define UART_0_FBRD  (38)

/* Delay for 5ms to ensure UART TX is idle before starting transmission */
#define UART_TX_DELAY 16000

typedef enum{
    Bits8 = 8,
    Bits5 = 5,
    Bits7 = 7,
    Bits6 = 6 
}UART_DATA_SIZE;

typedef enum{
    none = 0,
    even = 1,
    odd = 3
}PARITY;

typedef enum{
    one = 1,
    two = 2
}STOP_BITS;

typedef enum{
    BUSCLK = 0x00000008,
    MFCLK = 0x00000004,
    LFCLK = 0x00000002
}UART_CLOCK_SOURCE;

typedef struct{
    UART_DATA_SIZE Data;
    PARITY Par;
    STOP_BITS bits;
}TRAMA_UART;

typedef struct{
    UART_CLOCK_SOURCE CLK;
    uint32_t baudRate;
}BAUD_RATE;

typedef struct{
    UART_Regs *uartInterface;
    TRAMA_UART trama;
    BAUD_RATE baud;
    uint32_t RX;
    uint8_t RX_port; 
    uint32_t TX;
    uint8_t TX_port; 
}Driver_Uart_t;


extern void Driver_Uart_init(Driver_Uart_t *ptr);
extern void Driver_Uart_sendMessageUart(Driver_Uart_t *ptr, const char *str);
