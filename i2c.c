#include "i2c.h"


void i2c_init(i2c_t * ptr)
{
    // 1) Configure SDA and SCL pin functions and select as input by using the IOMUX registers.
        // SDA config
    if(ptr->SDA.port == PORTB){ 
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOB(ptr->SDA.pin)] = IOMUX_PINCM_WCOMP_MATCH0|IOMUX_PINCM_WUEN_DISABLE|IOMUX_PINCM_INV_DISABLE|IOMUX_PINCM_HIZ1_ENABLE|IOMUX_PINCM_DRV_DRVVAL0|IOMUX_PINCM_HYSTEN_DISABLE|IOMUX_PINCM_INENA_ENABLE|IOMUX_PINCM_PIPU_DISABLE|IOMUX_PINCM_PIPD_DISABLE|IOMUX_PINCM_WAKESTAT_DISABLE|IOMUX_PINCM_PC_CONNECTED|ptr->SDA.PIMCM_PF;    
    }else{
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOA(ptr->SDA.pin)] = IOMUX_PINCM_WCOMP_MATCH0|IOMUX_PINCM_WUEN_DISABLE|IOMUX_PINCM_INV_DISABLE|IOMUX_PINCM_HIZ1_ENABLE|IOMUX_PINCM_DRV_DRVVAL0|IOMUX_PINCM_HYSTEN_DISABLE|IOMUX_PINCM_INENA_ENABLE|IOMUX_PINCM_PIPU_DISABLE|IOMUX_PINCM_PIPD_DISABLE|IOMUX_PINCM_WAKESTAT_DISABLE|IOMUX_PINCM_PC_CONNECTED|ptr->SDA.PIMCM_PF; 
    }
        // SCL config
    if(ptr->SCL.port == PORTB){ 
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOB(ptr->SCL.pin)] = IOMUX_PINCM_WCOMP_MATCH0|IOMUX_PINCM_WUEN_DISABLE|IOMUX_PINCM_INV_DISABLE|IOMUX_PINCM_HIZ1_ENABLE|IOMUX_PINCM_DRV_DRVVAL0|IOMUX_PINCM_HYSTEN_DISABLE|IOMUX_PINCM_INENA_ENABLE|IOMUX_PINCM_PIPU_DISABLE|IOMUX_PINCM_PIPD_DISABLE|IOMUX_PINCM_WAKESTAT_DISABLE|IOMUX_PINCM_PC_CONNECTED|ptr->SCL.PIMCM_PF;
    }else{ 
        IOMUX->SECCFG.PINCM[GET_PINCM_GPIOA(ptr->SCL.pin)] = IOMUX_PINCM_WCOMP_MATCH0|IOMUX_PINCM_WUEN_DISABLE|IOMUX_PINCM_INV_DISABLE|IOMUX_PINCM_HIZ1_ENABLE|IOMUX_PINCM_DRV_DRVVAL0|IOMUX_PINCM_HYSTEN_DISABLE|IOMUX_PINCM_INENA_ENABLE|IOMUX_PINCM_PIPU_DISABLE|IOMUX_PINCM_PIPD_DISABLE|IOMUX_PINCM_WAKESTAT_DISABLE|IOMUX_PINCM_PC_CONNECTED|ptr->SCL.PIMCM_PF;    
    }

    // 2) Reset the peripheral using I2Cx.RSTCTL register
    ptr->I2C->GPRCM.RSTCTL = I2C_RSTCTL_KEY_UNLOCK_W|I2C_RSTCTL_RESETSTKYCLR_CLR|I2C_RSTCTL_RESETASSERT_ASSERT;
    // 3) Enable the power to peripheral using I2Cx.PWREN register
    ptr->I2C->GPRCM.PWREN  = I2C_PWREN_KEY_UNLOCK_W|I2C_PWREN_ENABLE_ENABLE;
    // 4) Select and configure the I2C clock using the CLKCTL and CLKDIV registers.
    ptr->I2C->CLKSEL = ptr->CLK.SOURCE;
    ptr->I2C->CLKDIV = ptr->CLK.DIV;
    // 5) Set the desired SCL clock speed of by writing the TPR bit in I2Cx.MTPR 
    ptr->I2C->MASTER.MTPR = (uint32_t)(ptr->CLK.TPR & I2C_MTPR_TPR_MASK);
    ptr->I2C->MASTER.MCR = I2C_MCR_LPBK_DISABLE|I2C_MCR_CLKSTRETCH_ENABLE|I2C_MCR_MMST_DISABLE|I2C_MCR_ACTIVE_ENABLE;   // Enables the I2C Controller operation

    ptr->I2C->MASTER.MFIFOCTL = I2C_MFIFOCTL_RXFLUSH_FLUSH|I2C_MFIFOCTL_TXFLUSH_FLUSH; 
    ptr->I2C->MASTER.MFIFOCTL = I2C_MFIFOCTL_RXFLUSH_NOFLUSH|I2C_MFIFOCTL_TXFLUSH_NOFLUSH;
}


// Nueva función para escribir un único byte (para PCF8574)
bool i2c_singleByteWrite(i2c_t *ptr, uint8_t slaveAddress, uint8_t data)
{
    volatile bool success;
    while(ptr->I2C->MASTER.MSR & I2C_MSR_BUSY_MASK){}
    ptr->I2C->MASTER.MFIFOCTL = I2C_MFIFOCTL_RXFLUSH_FLUSH|I2C_MFIFOCTL_TXFLUSH_FLUSH;
    ptr->I2C->MASTER.MFIFOCTL = I2C_MFIFOCTL_RXFLUSH_NOFLUSH|I2C_MFIFOCTL_TXFLUSH_NOFLUSH;
    ptr->I2C->MASTER.MSA = I2C_MSA_DIR_TRANSMIT|(slaveAddress << 1)|I2C_MSA_MMODE_MODE7;
    ptr->I2C->MASTER.MTXDATA = (uint32_t)data;  // SOLO 1 byte
    ptr->I2C->MASTER.MCTR = (1 << 16)|I2C_MCTR_STOP_ENABLE|I2C_MCTR_START_ENABLE|I2C_MCTR_BURSTRUN_ENABLE;
    while(ptr->I2C->MASTER.MSR & I2C_MSR_BUSY_MASK){}
    while(ptr->I2C->MASTER.MSR & I2C_MSR_BUSBSY_MASK){}
    (ptr->I2C->MASTER.MSR & I2C_MSR_ERR_SET)?(success = false):(success = true);
    return success;
}


bool i2c_singleByteWriteSequence(i2c_t * ptr, uint8_t slaveAddress, uint8_t slaveRegisterAddress, uint8_t data)
{
//            Single-Byte Write Sequence MPU6050
//  |--------|---|------|-----|----|-----|------|-----|---|
//  | Master | S | AD+W |     | RA |     | DATA |     | P |
//  |--------|---|------|-----|----|-----|------|-----|---|
//  | Slave  |   |      | ACK |    | ACK |      | ACK |   |
//  |--------|---|------|-----|----|-----|------|-----|---|
    volatile bool success;
    const uint8_t dataLength = 2;
    while(ptr->I2C->MASTER.MSR & I2C_MSR_BUSY_MASK){}
    ptr->I2C->MASTER.MFIFOCTL = I2C_MFIFOCTL_RXFLUSH_FLUSH|I2C_MFIFOCTL_TXFLUSH_FLUSH; 
    ptr->I2C->MASTER.MFIFOCTL = I2C_MFIFOCTL_RXFLUSH_NOFLUSH|I2C_MFIFOCTL_TXFLUSH_NOFLUSH;
    ptr->I2C->MASTER.MSA = I2C_MSA_DIR_TRANSMIT|(slaveAddress << 1)|I2C_MSA_MMODE_MODE7;
    ptr->I2C->MASTER.MTXDATA = (uint32_t)slaveRegisterAddress;
    ptr->I2C->MASTER.MTXDATA = (uint32_t)data;
    ptr->I2C->MASTER.MCTR = ((uint32_t)dataLength << 16)|I2C_MCTR_STOP_ENABLE|I2C_MCTR_START_ENABLE|I2C_MCTR_BURSTRUN_ENABLE;
    while(ptr->I2C->MASTER.MSR & I2C_MSR_BUSY_MASK){}
    //For polling purposes only, please comment out next instruction for the interrupt-driven approach
    while(ptr->I2C->MASTER.MSR & I2C_MSR_BUSBSY_MASK){}    
    (ptr->I2C->MASTER.MSR & I2C_MSR_ERR_SET)?(success = false):(success = true);
    return success;
}

bool i2c_burstWriteSequence(i2c_t *ptr, uint8_t slaveAddress, uint8_t slaveRegisterAddress, uint8_t *data, uint8_t numBytes)
{
//                           Burst Write Sequence MPU6050
//      |--------|---|------|-----|----|-----|------|-----|------|-----|---|
//      | Master | S | AD+W |     | RA |     | DATA |     | DATA |     | P |
//      |--------|---|------|-----|----|-----|------|-----|------|-----|---|
//      | Slave  |   |      | ACK |    | ACK |      | ACK |      | ACK |   |
//      |--------|---|------|-----|----|-----|------|-----|------|-----|---|
    volatile bool success;
    volatile uint8_t j;
    while(ptr->I2C->MASTER.MSR & I2C_MSR_BUSY_MASK){}
    I2C0->MASTER.MFIFOCTL = I2C_MFIFOCTL_RXFLUSH_FLUSH|I2C_MFIFOCTL_TXFLUSH_FLUSH; 
    I2C0->MASTER.MFIFOCTL = I2C_MFIFOCTL_RXFLUSH_NOFLUSH|I2C_MFIFOCTL_TXFLUSH_NOFLUSH;
    ptr->I2C->MASTER.MSA = I2C_MSA_DIR_TRANSMIT|(slaveAddress << 1)|I2C_MSA_MMODE_MODE7;
    ptr->I2C->MASTER.MTXDATA = slaveRegisterAddress;
    for(j=0x00;j<numBytes;j++)
    {
        ptr->I2C->MASTER.MTXDATA = (uint32_t)(*data++);
    }
    ptr->I2C->MASTER.MCTR = ((uint32_t)numBytes << 16)|I2C_MCTR_STOP_ENABLE|I2C_MCTR_START_ENABLE|I2C_MCTR_BURSTRUN_ENABLE;
    while(ptr->I2C->MASTER.MSR != I2C_MSR_BUSBSY_MASK){}
    //For polling purposes only, please comment out next instruction for the interrupt-driven approach
    while(ptr->I2C->MASTER.MSR & I2C_MSR_BUSBSY_MASK){}    
    (ptr->I2C->MASTER.MSR & I2C_MSR_ERR_SET)?(success = false):(success = true);
    return success;
}

bool i2c_singleByteReadSequence(i2c_t *ptr, uint8_t slaveAddress, uint8_t slaveRegisterAddress, uint8_t *data)
{
//                          Single-Byte Read Sequence MPU6050
//  |--------|---|------|-----|----|-----|---|------|-----|------|------|---|
//  | Master | S | AD+W |     | RA |     | S | AD+R |     |      | NACK | P |
//  |--------|---|------|-----|----|-----|---|------|-----|------|------|---|
//  | Slave  |   |      | ACK |    | ACK |   |      | ACK | DATA |      |   |
//  |--------|---|------|-----|----|-----|---|------|-----|------|------|---|
    volatile bool success;
    volatile uint8_t dataLength = 1;
    while(ptr->I2C->MASTER.MSR & I2C_MSR_BUSY_MASK){}
    ptr->I2C->MASTER.MFIFOCTL = I2C_MFIFOCTL_RXFLUSH_FLUSH|I2C_MFIFOCTL_TXFLUSH_FLUSH; 
    ptr->I2C->MASTER.MFIFOCTL = I2C_MFIFOCTL_RXFLUSH_NOFLUSH|I2C_MFIFOCTL_TXFLUSH_NOFLUSH;
//    while(ptr->I2C->MASTER.MSR & I2C_MSR_BUSY_MASK){}
    ptr->I2C->MASTER.MSA = I2C_MSA_DIR_TRANSMIT|(slaveAddress << 1)|I2C_MSA_MMODE_MODE7;
    ptr->I2C->MASTER.MTXDATA = slaveRegisterAddress;
    ptr->I2C->MASTER.MCTR = ((uint32_t)dataLength << 16)|I2C_MCTR_START_ENABLE|I2C_MCTR_BURSTRUN_ENABLE;
    while(ptr->I2C->MASTER.MSR != I2C_MSR_BUSBSY_MASK){}
    dataLength = 1;
    ptr->I2C->MASTER.MSA = I2C_MSA_DIR_RECEIVE|(slaveAddress << 1)|I2C_MSA_MMODE_MODE7;
    ptr->I2C->MASTER.MCTR = ((uint32_t)dataLength << 16)|I2C_MCTR_ACK_DISABLE|I2C_MCTR_START_ENABLE|I2C_MCTR_STOP_ENABLE|I2C_MCTR_BURSTRUN_ENABLE;
    while(ptr->I2C->MASTER.MSR & I2C_MSR_BUSY_MASK){}
    //For polling purposes only, please comment out next instruction for the interrupt-driven approach
    while(ptr->I2C->MASTER.MSR & I2C_MSR_BUSBSY_MASK){}   
    if(ptr->I2C->MASTER.MSR & I2C_MSR_ERR_SET){
       success = false; 
    }else{
        (*data) = (uint8_t)(ptr->I2C->MASTER.MRXDATA);
        success = true;
    }
    return success;
}

bool i2c_burstReadSequence(i2c_t *ptr, uint8_t slaveAddress, uint8_t slaveRegisterAddress, uint8_t *data, uint8_t numBytes)
{
//                            Burst Read Sequence MPU6050
//  |--------|---|------|-----|----|-----|---|------|-----|------|-----|------|------|---|
//  | Master | S | AD+W |     | RA |     | S | AD+R |     |      | ACK |      | NACK | P |
//  |--------|---|------|-----|----|-----|---|------|-----|------|-----|------|------|---|
//  | Slave  |   |      | ACK |    | ACK |   |      | ACK | DATA |     | DATA |      |   |
//  |--------|---|------|-----|----|-----|---|------|-----|------|-----|------|------|---|
    volatile bool success;
    volatile uint8_t j;
    const uint8_t dataLength = 1;
    while(ptr->I2C->MASTER.MSR & I2C_MSR_BUSY_MASK){}
    ptr->I2C->MASTER.MFIFOCTL = I2C_MFIFOCTL_RXFLUSH_FLUSH|I2C_MFIFOCTL_TXFLUSH_FLUSH; 
    ptr->I2C->MASTER.MFIFOCTL = I2C_MFIFOCTL_RXFLUSH_NOFLUSH|I2C_MFIFOCTL_TXFLUSH_NOFLUSH;
    ptr->I2C->MASTER.MSA = I2C_MSA_DIR_TRANSMIT|(slaveAddress << 1)|I2C_MSA_MMODE_MODE7;
    ptr->I2C->MASTER.MTXDATA = slaveRegisterAddress;
    ptr->I2C->MASTER.MCTR = ((uint32_t)dataLength << 16)|I2C_MCTR_START_ENABLE|I2C_MCTR_BURSTRUN_ENABLE;
    while(I2C0->MASTER.MSR != I2C_MSR_BUSBSY_MASK){}
    ptr->I2C->MASTER.MSA = I2C_MSA_DIR_RECEIVE|(slaveAddress << 1)|I2C_MSA_MMODE_MODE7;
    ptr->I2C->MASTER.MCTR = ((uint32_t)numBytes << 16)|I2C_MCTR_ACK_DISABLE|I2C_MCTR_START_ENABLE|I2C_MCTR_STOP_ENABLE|I2C_MCTR_BURSTRUN_ENABLE;
    while(ptr->I2C->MASTER.MSR != I2C_MSR_IDLE_SET){}
    //For polling purposes only, please comment out next instruction for the interrupt-driven approach
    while(ptr->I2C->MASTER.MSR & I2C_MSR_BUSBSY_MASK){}   
    if(ptr->I2C->MASTER.MSR & I2C_MSR_ERR_SET){
       success = false; 
    }else{
        for(j=0x00; j<numBytes; j++)
        {
            (*(data++)) = (uint8_t)(ptr->I2C->MASTER.MRXDATA);
        }
        success = true;
    }
    return success;    
}