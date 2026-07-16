#include "Driver_UART.h"


void Driver_Uart_init(Driver_Uart_t *ptr)
{
    // 1. Reset y encendido
    ptr->uartInterface->GPRCM.RSTCTL =
        (UART_RSTCTL_KEY_UNLOCK_W | UART_RSTCTL_RESETSTKYCLR_CLR |
         UART_RSTCTL_RESETASSERT_ASSERT);
    ptr->uartInterface->GPRCM.PWREN =
        (UART_PWREN_KEY_UNLOCK_W | UART_PWREN_ENABLE_ENABLE);
    delay_cycles(16);

    // 2. Pinmux PA10=TX, PA11=RX
    IOMUX->SECCFG.PINCM[IOMUX_PINCM21] = IOMUX_PINCM21_PF_UART0_TX | IOMUX_PINCM_PC_CONNECTED;
    IOMUX->SECCFG.PINCM[IOMUX_PINCM22] = IOMUX_PINCM22_PF_UART0_RX | IOMUX_PINCM_PC_CONNECTED | IOMUX_PINCM_INENA_ENABLE;

    // 3. Clock
    ptr->uartInterface->CLKSEL = ptr->baud.CLK;
    ptr->uartInterface->CLKDIV = 0x00000000;

    // 4. Deshabilitar antes de configurar
    ptr->uartInterface->CTL0 &= ~UART_CTL0_ENABLE_MASK;

    // 5. CTL0: TX y RX + oversampling 3x
    ptr->uartInterface->CTL0 =
        UART_CTL0_TXE_ENABLE | UART_CTL0_RXE_ENABLE | UART_CTL0_HSE_OVS3;

    // 6. LCRH: 8 bits + FIFOs
    ptr->uartInterface->LCRH =
        UART_LCRH_WLEN_DATABIT8 | (1U << 4);

    // 7. Baudrate
    ptr->uartInterface->IBRD = UART_0_IBRD;
    ptr->uartInterface->FBRD = UART_0_FBRD;

    // 8. FIFO thresholds
    ptr->uartInterface->IFLS =
        (2U << UART_IFLS_RXIFLSEL_OFS) | (2U << UART_IFLS_TXIFLSEL_OFS);

    // 9. Habilitar UART
    ptr->uartInterface->CTL0 |= UART_CTL0_ENABLE_MASK;
}

void Driver_Uart_sendMessageUart(Driver_Uart_t *ptr, const char *str)
{
    while(*str) {
        while(ptr->uartInterface->STAT & 0x00000001){} // Wait because UART is busy
        ptr->uartInterface->TXDATA = *(str++);         // transmit because UART isn't busy
    }
}