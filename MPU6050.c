#include "MPU6050.h"
#include <ti/driverlib/driverlib.h>   // <-- agrega esta linea para delay_cycles

// Sensibilidades del datasheet (LSB por unidad) segun el rango.
// gyro: seccion 4.20  |  accel: seccion 4.18
static float gyroSens(mpu6050_gyroFS_t fs)
{
    switch(fs){
        case GYRO_FS_250:  return 131.0f;
        case GYRO_FS_500:  return 65.5f;
        case GYRO_FS_1000: return 32.8f;
        case GYRO_FS_2000: return 16.4f;
    }
    return 131.0f;
}
static float accelSens(mpu6050_accelFS_t fs)
{
    switch(fs){
        case ACCEL_FS_2G:  return 16384.0f;
        case ACCEL_FS_4G:  return 8192.0f;
        case ACCEL_FS_8G:  return 4096.0f;
        case ACCEL_FS_16G: return 2048.0f;
    }
    return 16384.0f;
}

bool mpu6050_init(mpu6050_t *ptr)
{
    bool ok = true;
    i2c_init(&(ptr->myI2C));
    if(ptr->addr == 0) ptr->addr = MPU6050ADDR;   // por defecto AD0=GND

    // 1) Reset del dispositivo (Reg 107 DEVICE_RESET) y esperar
    ok &= i2c_singleByteWriteSequence(&(ptr->myI2C), ptr->addr,
                                      MPU6050_REG_PWR_MGMT_1, MPU6050_PWR1_DEVICE_RESET);
    delay_cycles(3200000);   // ~100 ms a 32 MHz, para que termine el reset

    // 2) Despertar del sleep y usar PLL con giro X como reloj (mas estable)
    ok &= i2c_singleByteWriteSequence(&(ptr->myI2C), ptr->addr,
                                      MPU6050_REG_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_PLL_XG);

    // 3) DLPF: filtro ~44/42 Hz (Reg 26 CONFIG = 3). Baja ruido, gyro rate = 1 kHz
    ok &= i2c_singleByteWriteSequence(&(ptr->myI2C), ptr->addr,
                                      MPU6050_REG_CONFIG, 0x03);

    // 4) Sample Rate = 1kHz/(1+SMPLRT_DIV). Con DIV=9 -> 100 Hz
    ok &= i2c_singleByteWriteSequence(&(ptr->myI2C), ptr->addr,
                                      MPU6050_REG_SMPLRT_DIV, 0x09);

    // 5) Rangos de fondo de escala
    ok &= i2c_singleByteWriteSequence(&(ptr->myI2C), ptr->addr,
                                      MPU6050_REG_GYRO_CONFIG,  (uint8_t)ptr->gyroFS);
    ok &= i2c_singleByteWriteSequence(&(ptr->myI2C), ptr->addr,
                                      MPU6050_REG_ACCEL_CONFIG, (uint8_t)ptr->accelFS);

    // El offset de calibracion arranca en 0 hasta que se llame a
    // mpu6050_calibrarGiro(); asi mpu6050_readGyroCal() sigue siendo
    // seguro de usar aunque aun no se haya calibrado.
    ptr->gyroOffset.x = 0;
    ptr->gyroOffset.y = 0;
    ptr->gyroOffset.z = 0;

    return ok;
}

bool mpu6050_whoAmI(mpu6050_t *ptr, uint8_t *id)
{
    return i2c_singleByteReadSequence(&(ptr->myI2C), ptr->addr,
                                      MPU6050_REG_WHO_AM_I, id);
}

// Une dos bytes (H,L) en un entero con signo de 16 bits
static int16_t join(uint8_t h, uint8_t l)
{
    return (int16_t)(((uint16_t)h << 8) | (uint16_t)l);
}

bool mpu6050_readAccel(mpu6050_t *ptr, mpu6050_axis_t *accel)
{
    uint8_t b[6];
    if(!i2c_burstReadSequence(&(ptr->myI2C), ptr->addr,
                              MPU6050_REG_ACCEL_XOUT_H, b, 6)) return false;
    accel->x = join(b[0], b[1]);
    accel->y = join(b[2], b[3]);
    accel->z = join(b[4], b[5]);
    return true;
}

bool mpu6050_readGyro(mpu6050_t *ptr, mpu6050_axis_t *gyro)
{
    uint8_t b[6];
    if(!i2c_burstReadSequence(&(ptr->myI2C), ptr->addr,
                              MPU6050_REG_GYRO_XOUT_H, b, 6)) return false;
    gyro->x = join(b[0], b[1]);
    gyro->y = join(b[2], b[3]);
    gyro->z = join(b[4], b[5]);
    return true;
}

// Lectura completa: accel(6) + temp(2) + gyro(6) = 14 bytes contiguos
// desde ACCEL_XOUT_H (0x3B). Una sola rafaga => una sola muestra coherente.
bool mpu6050_readAll(mpu6050_t *ptr, mpu6050_axis_t *accel,
                     mpu6050_axis_t *gyro, int16_t *tempRaw)
{
    uint8_t b[14];
    if(!i2c_burstReadSequence(&(ptr->myI2C), ptr->addr,
                              MPU6050_REG_ACCEL_XOUT_H, b, 14)) return false;
    accel->x = join(b[0],  b[1]);
    accel->y = join(b[2],  b[3]);
    accel->z = join(b[4],  b[5]);
    if(tempRaw) *tempRaw = join(b[6], b[7]);
    gyro->x  = join(b[8],  b[9]);
    gyro->y  = join(b[10], b[11]);
    gyro->z  = join(b[12], b[13]);
    return true;
}

float mpu6050_gyroToDps(mpu6050_t *ptr, int16_t raw)
{
    return (float)raw / gyroSens(ptr->gyroFS);
}

float mpu6050_accelToG(mpu6050_t *ptr, int16_t raw)
{
    return (float)raw / accelSens(ptr->accelFS);
}

// Datasheet seccion 4.19: Temp(C) = raw/340 + 36.53
float mpu6050_tempToC(int16_t raw)
{
    return (float)raw / 340.0f + 36.53f;
}

// ------------------------------------------------------------------
// NUEVO: calibracion del giroscopio por promedio simple.
//
// Con el sensor inmovil, cada eje deberia leer 0 dps. Cualquier valor
// distinto es el "bias" de fabricacion del MEMS. Aqui se promedian
// "muestras" lecturas crudas (sin convertir a dps) y se guarda el
// promedio entero en ptr->gyroOffset, para restarlo luego en cada
// lectura real (ver mpu6050_readGyroCal).
// ------------------------------------------------------------------
void mpu6050_calibrarGiro(mpu6050_t *ptr, uint16_t muestras)
{
    int32_t sumX = 0, sumY = 0, sumZ = 0;
    mpu6050_axis_t g;
    uint16_t i, validas = 0;

    ptr->gyroOffset.x = 0;
    ptr->gyroOffset.y = 0;
    ptr->gyroOffset.z = 0;

    for(i = 0; i < muestras; i++)
    {
        if(mpu6050_readGyro(ptr, &g))
        {
            sumX += g.x;
            sumY += g.y;
            sumZ += g.z;
            validas++;
        }
        delay_cycles(32000);   // ~1 ms @ 32 MHz: da tiempo a que exista una muestra nueva (sample rate = 100 Hz)
    }

    if(validas > 0)
    {
        ptr->gyroOffset.x = (int16_t)(sumX / (int32_t)validas);
        ptr->gyroOffset.y = (int16_t)(sumY / (int32_t)validas);
        ptr->gyroOffset.z = (int16_t)(sumZ / (int32_t)validas);
    }
    // Si "validas" quedo en 0 (todas las lecturas fallaron), el offset
    // se deja en 0 y mpu6050_readGyroCal() se comporta igual que
    // mpu6050_readGyro(): preferible a dividir por cero.
}

bool mpu6050_readGyroCal(mpu6050_t *ptr, mpu6050_axis_t *gyro)
{
    mpu6050_axis_t raw;
    if(!mpu6050_readGyro(ptr, &raw)) return false;
    gyro->x = (int16_t)(raw.x - ptr->gyroOffset.x);
    gyro->y = (int16_t)(raw.y - ptr->gyroOffset.y);
    gyro->z = (int16_t)(raw.z - ptr->gyroOffset.z);
    return true;
}