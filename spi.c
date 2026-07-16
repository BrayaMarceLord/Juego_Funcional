#include "spi.h"

inline void spi_init(spi_t * ptr)
{
    ptr->SPI->GPRCM.RSTCTL = RSTKEY | 0x00000003;
    ptr->SPI->GPRCM.PWREN = PWRKEY | 0x00000001;

    //************* GPIO config ************************************/

        // PICO config
    if(ptr->PICO.port == PORTB){ // PF for PB22 = SPI1_PICO is 2
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOB(ptr->PICO.pin)] = IOMUX_PINCM_PC_CONNECTED | ((uint32_t) 0x00000003);    
    }else{
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOA(ptr->PICO.pin)] = IOMUX_PINCM_PC_CONNECTED | ((uint32_t) 0x00000003);   
    }
        // POCI config
/*    if(ptr->POCI.port == PORTB){ // PF for PB21 = SPI1_POCI is 2
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOB(ptr->POCI.pin)] = IOMUX_PINCM_PC_CONNECTED | ((uint32_t) 0x00000003);    
    }else{
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOA(ptr->POCI.pin)] = IOMUX_PINCM_PC_CONNECTED | ((uint32_t) 0x00000003);    
    }*/
        // SCLK config
    if(ptr->SCLK.port == PORTB){ // PF for PB23 = SPI1_SCK  is 2
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOB(ptr->SCLK.pin)] = IOMUX_PINCM_PC_CONNECTED |((uint32_t) 0x00000003);
    }else{ 
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOA(ptr->SCLK.pin)] = IOMUX_PINCM_PC_CONNECTED |((uint32_t) 0x00000003);    
    }
        // CS0 config
/*    if(ptr->CS0.port == PORTB){ 
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOB(ptr->CS0.pin)] = IOMUX_PINCM_PC_CONNECTED |  ((uint32_t) 0x00000003);
    }else{ // PF for PA2 = SPI1_CS0 is 5
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOA(ptr->CS0.pin)] = IOMUX_PINCM_PC_CONNECTED |  ((uint32_t) 0x00000003);    
    }*/
        // CS1 config
        // CS2 config
        // CS3 config
    if(ptr->CS3.port == PORTB){ 
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOB(ptr->CS3.pin)] = IOMUX_PINCM_PC_CONNECTED |  ((uint32_t) 0x00000002);   // PF for PB14 = SPI0_CS3 is 4
    }else{             // PF for PB24 = SPI0_CS3 is 2
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOA(ptr->CS3.pin)] = IOMUX_PINCM_PC_CONNECTED |  ((uint32_t) 0x00000003);   // PF for PA23 = SPI0_CS3 is 3   
    }                  


    // Ensure SPI DISABLED
    ptr->SPI->CTL1 = 0x00000000;
    // clk config
    ptr->SPI->CLKSEL = ptr->CLK.SOURCE;
    ptr->SPI->CLKDIV = ptr->CLK.DIV;
    // data size, chip selector and 4 wire mode,  Data change on the second clock edge transition, SPI produces a steady state HIGH value on the CLKOUT
    ptr->SPI->CTL0 = ptr->SIZE | ptr->CS | 0x00000020 | 0x00000300;
    //ptr->SPI->CLKCTL = 0x000003ff;
    // controller mode, parity disabled, Enable SPI, CS/CD mode and CD_MODE comands
    ptr->SPI->CTL1 = ptr->FORMAT | 0x00000004 | 0x00000001 | ptr->CD.ENABLE | ptr->CD.COMANDS; 
}

void spi_send(spi_t *ptr, uint16_t trama)
{
    while(ptr->SPI->STAT & 0x00000010){}   // Wait because Transmiter is busy (BUSYbit=1)
    ptr->SPI->TXDATA = trama; // Transmiter isn't busy (BUSYbit=0)
}

