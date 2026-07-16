#include "1_Drivers/i2c.h"

#ifndef MPU6050_H
#define MPU6050_H

#define MPU6050ADDR                    (0x68U)   // AD0 = GND
#define MPU6050ADDR2                   (0x69U)   // AD0 = VCC
#define MPU6050_WHOAMI_VAL             (0x68U)   // valor esperado del reg WHO_AM_I

// ---- Mapa de registros (del datasheet, seccion 3) ----
#define MPU6050_REG_SELF_TEST_X        (0x0DU)
#define MPU6050_REG_SELF_TEST_Y        (0x0EU)
#define MPU6050_REG_SELF_TEST_Z        (0x0FU)
#define MPU6050_REG_SELF_TEST_A        (0x10U)
#define MPU6050_REG_SMPLRT_DIV         (0x19U)
#define MPU6050_REG_CONFIG             (0x1AU)
#define MPU6050_REG_GYRO_CONFIG        (0x1BU)
#define MPU6050_REG_ACCEL_CONFIG       (0x1CU)
#define MPU6050_REG_FIFO_EN            (0x23U)
#define MPU6050_REG_INT_PIN_CFG        (0x37U)
#define MPU6050_REG_INT_ENABLE         (0x38U)
#define MPU6050_REG_INT_STATUS         (0x3AU)
#define MPU6050_REG_ACCEL_XOUT_H       (0x3BU)
#define MPU6050_REG_ACCEL_XOUT_L       (0x3CU)
#define MPU6050_REG_ACCEL_YOUT_H       (0x3DU)
#define MPU6050_REG_ACCEL_YOUT_L       (0x3EU)
#define MPU6050_REG_ACCEL_ZOUT_H       (0x3FU)
#define MPU6050_REG_ACCEL_ZOUT_L       (0x40U)
#define MPU6050_REG_TEMP_OUT_H         (0x41U)
#define MPU6050_REG_TEMP_OUT_L         (0x42U)
#define MPU6050_REG_GYRO_XOUT_H        (0x43U)
#define MPU6050_REG_GYRO_XOUT_L        (0x44U)
#define MPU6050_REG_GYRO_YOUT_H        (0x45U)
#define MPU6050_REG_GYRO_YOUT_L        (0x46U)
#define MPU6050_REG_GYRO_ZOUT_H        (0x47U)
#define MPU6050_REG_GYRO_ZOUT_L        (0x48U)
#define MPU6050_REG_SIGNAL_PATH_RESET  (0x68U)
#define MPU6050_REG_USER_CTRL          (0x6AU)
#define MPU6050_REG_PWR_MGMT_1         (0x6BU)
#define MPU6050_REG_PWR_MGMT_2         (0x6CU)
#define MPU6050_REG_WHO_AM_I           (0x75U)

// ---- Bits utiles ----
#define MPU6050_PWR1_DEVICE_RESET      (0x80U)   // Reg 107 bit7
#define MPU6050_PWR1_SLEEP             (0x40U)   // Reg 107 bit6
#define MPU6050_PWR1_CLKSEL_PLL_XG     (0x01U)   // Reg 107: reloj = PLL con giro X (recomendado)

// ---- Rangos de fondo de escala (Reg 27 GYRO_CONFIG bits[4:3]) ----
typedef enum{
    GYRO_FS_250  = 0x00,   // +-250  dps -> 131   LSB/(dps)
    GYRO_FS_500  = 0x08,   // +-500  dps -> 65.5
    GYRO_FS_1000 = 0x10,   // +-1000 dps -> 32.8
    GYRO_FS_2000 = 0x18,   // +-2000 dps -> 16.4
}mpu6050_gyroFS_t;

// ---- Rangos de fondo de escala (Reg 28 ACCEL_CONFIG bits[4:3]) ----
typedef enum{
    ACCEL_FS_2G  = 0x00,   // +-2g  -> 16384 LSB/g
    ACCEL_FS_4G  = 0x08,   // +-4g  -> 8192
    ACCEL_FS_8G  = 0x10,   // +-8g  -> 4096
    ACCEL_FS_16G = 0x18,   // +-16g -> 2048
}mpu6050_accelFS_t;

// ---- Datos crudos (16-bit con signo, complemento a 2) ----
typedef struct{
    int16_t x;
    int16_t y;
    int16_t z;
}mpu6050_axis_t;

typedef struct{
    i2c_t myI2C;
    uint8_t addr;                 // MPU6050ADDR o MPU6050ADDR2
    mpu6050_gyroFS_t  gyroFS;
    mpu6050_accelFS_t accelFS;
    mpu6050_axis_t    gyroOffset; // NUEVO: bias crudo del giroscopio (ver mpu6050_calibrarGiro)
}mpu6050_t;

// ---- API ----
extern bool mpu6050_init(mpu6050_t *ptr);              // despierta + configura escalas
extern bool mpu6050_whoAmI(mpu6050_t *ptr, uint8_t *id);
extern bool mpu6050_readAccel(mpu6050_t *ptr, mpu6050_axis_t *accel);
extern bool mpu6050_readGyro(mpu6050_t *ptr, mpu6050_axis_t *gyro);
extern bool mpu6050_readAll(mpu6050_t *ptr, mpu6050_axis_t *accel,
                            mpu6050_axis_t *gyro, int16_t *tempRaw);

// Conversiones a unidades fisicas (usan la escala configurada)
extern float mpu6050_gyroToDps(mpu6050_t *ptr, int16_t raw);   // dps
extern float mpu6050_accelToG(mpu6050_t *ptr, int16_t raw);    // g
extern float mpu6050_tempToC(int16_t raw);                     // grados C

// ------------------------------------------------------------------
// NUEVO: calibracion del giroscopio.
//
// Promedia "muestras" lecturas CRUDAS del giroscopio con el sensor
// QUIETO y apoyado en una superficie estable, y guarda el promedio en
// ptr->gyroOffset. Ese offset es el "bias" propio de fabricacion del
// MEMS: con el sensor en reposo cualquier lectura distinta de cero es
// error sistematico, no movimiento real.
//
// Debe llamarse UNA VEZ, despues de mpu6050_init() y ANTES de usar
// mpu6050_readGyroCal(). Con 100-500 muestras suele bastar; mas
// muestras dan un offset mas estable pero alargan el arranque
// (cada muestra espera ~1 ms, ver implementacion).
// ------------------------------------------------------------------
extern void mpu6050_calibrarGiro(mpu6050_t *ptr, uint16_t muestras);

// Igual que mpu6050_readGyro(), pero resta ptr->gyroOffset. Usar esta
// version (en vez de mpu6050_readGyro) una vez calibrado el sensor.
extern bool mpu6050_readGyroCal(mpu6050_t *ptr, mpu6050_axis_t *gyro);

#endif