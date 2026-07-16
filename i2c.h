#include <ti/devices/msp/msp.h>
#include <0_MyDefinitions/GPIODefinitions.h>

#ifndef I2C_DRIVER
#define I2C_DRIVER

    #define I2C_OK  0x00U
    #define I2C_ERR 0x01U

    typedef enum{
        BUSCLK_SEL_I2C = 0x00000008,
        MFCLK_SEL_I2C = 0x00000004,
    }CLOCK_SOURCE_I2C;

    typedef enum{
        DIV_BY_1_I2C = 0x00000000,
        DIV_BY_2_I2C = 0x00000001,
        DIV_BY_3_I2C = 0x00000002,
        DIV_BY_4_I2C = 0x00000003,
        DIV_BY_5_I2C = 0x00000004,
        DIV_BY_6_I2C = 0x00000005,
        DIV_BY_7_I2C = 0x00000006,
        DIV_BY_8_I2C = 0x00000007,
    }CLOCK_DIV_I2C;

    typedef struct{
        const uint32_t pin;
        const uint8_t port;
        const uint32_t PIMCM_PF;
    }pinI2C_t;

    typedef struct {
        CLOCK_SOURCE_I2C SOURCE;
        CLOCK_DIV_I2C DIV;
        uint8_t TPR;
    }clk_I2C_t;

    typedef struct{
        I2C_Regs *I2C;
        pinI2C_t SDA;     // Peripheral input-Controller output pin
        pinI2C_t SCL;     // Clock pin
        clk_I2C_t CLK;        // Clock configuration
    }i2c_t;

    extern void i2c_init(i2c_t * ptr);
    extern bool i2c_singleByteWrite(i2c_t *ptr, uint8_t slaveAddress, uint8_t data);
    extern bool i2c_singleByteWriteSequence(i2c_t *ptr, uint8_t slaveAddress, uint8_t slaveRegisterAddress, uint8_t data);
    extern bool i2c_singleByteReadSequence(i2c_t *ptr, uint8_t slaveAddress, uint8_t slaveRegisterAddress, uint8_t *data);
    extern bool i2c_burstReadSequence(i2c_t *ptr, uint8_t slaveAddress, uint8_t slaveRegisterAddress, uint8_t *data, uint8_t numBytes);
    extern bool i2c_burstWriteSequence(i2c_t *ptr, uint8_t slaveAddress, uint8_t slaveRegisterAddress, uint8_t *data, uint8_t numBytes);

#endif