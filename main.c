#include "ti_msp_dl_config.h"
#include "3_HardwareSupport/display16x32.h"
#include "1_Drivers/LEDS.h"
#include "2_HAL/MPU6050.h"
#include "2_HAL/lcd_i2c.h"
#include "5_Game/game.h"
#include "5_Game/buzzer.h"


// ---- Capa del profesor ----
display16x32_t myDisplay = {
    .myMax7219 = {
        //Intensity = Intensity_8,
        .mySPI = {
            .SPI  = SPI0,
            .PICO = {.pin = PB17, .port = PORTB},   // DIN
            .SCLK = {.pin = PA12, .port = PORTA},   // CLK
            .CS3  = {.pin = PB24, .port = PORTB},   // LOAD (modo C/D)
            .CLK  = {.SOURCE = MCLK_SEL, .DIV = DIV_BY_4},
        },
    },
};

LEDS_t myLeds = {
    .leds = {PA0, PB26, PB27, PB22},          // LEDs de vidas
    .leds_port = {PORTA, PORTB, PORTB, PORTB},
    .pulsadores = {PA18, PB21},               // S1, S2 de la LaunchPad
    .pulsadores_port = {PORTA, PORTB},
};

Driver_Uart_t myUart = {
    .uartInterface = UART0,
    .baud = {.CLK = BUSCLK, .baudRate = 115200},
};

// ---- LCD 16x2 por I2C (PCF8574T, dir 0x27) ----
lcd_t myLcd = {
    .addr = 0x27,
    .myI2C = {
        .I2C = I2C0,
        // BUSCLK: SCL = 32MHz / ((31+1)*10) = 100 kHz (probado con el "Hola")
        .CLK = {.DIV = DIV_BY_1_I2C, .SOURCE = BUSCLK_SEL_I2C, .TPR = 31},
        .SDA = {.pin = PA28, .port = PORTA, .PIMCM_PF = 3},
        .SCL = {.pin = PA31, .port = PORTA, .PIMCM_PF = 3},
    },
};

// ---- MPU6050 por I2C (mismo bus que la LCD, dir 0x68) ----
mpu6050_t myMpu = {
    .addr   = MPU6050ADDR,      // AD0 = GND
    .gyroFS  = GYRO_FS_250,
    .accelFS = ACCEL_FS_2G,
    .myI2C = {
        .I2C = I2C0,
        .CLK = {.DIV = DIV_BY_1_I2C, .SOURCE = BUSCLK_SEL_I2C, .TPR = 31},
        .SDA = {.pin = PA28, .port = PORTA, .PIMCM_PF = 3},
        .SCL = {.pin = PA31, .port = PORTA, .PIMCM_PF = 3},
    },
};

// ---- Capa del juego ----
botones_t myBotones = {
    .disparo = {.pin = PB6, .port = PORTB},
    .pausa   = {.pin = PB7, .port = PORTB},
    .izquierda = {.pin = PB16, .port = PORTB},
    .derecha   = {.pin = PB8,  .port = PORTB},
    .placa   = &myLeds,
};

game_t myGame = {
    .display = &myDisplay,
    .botones = &myBotones,
    .leds    = &myLeds,
    .uart    = &myUart,
    .lcd     = &myLcd,
    .mpu = &myMpu
};

volatile uint16_t ADCLight = 0;

void ADC12_0_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_MEM1_RESULT_LOADED:
            ADCLight = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_1);  // MEM1 = luz
            break;
        default:
            break;
    }
}

volatile uint8_t tickFlag = 0;

int main(void)
{
    SYSCFG_DL_init();          // TIMER_0 (30 Hz), ADC, PWMs, I2C0 desde SysConfig
    buzzer_init();
    LEDS_Init(&myLeds);        // energiza GPIOA/B y configura S1/S2 + LEDs
    botones_init(&myBotones);
    display16x32_init(&myDisplay);
    lcd_init(&myLcd);          // LCD 16x2 (I2C0, BUSCLK)
    lcd_init(&myLcd);
    mpu6050_init(&myMpu);      // mismo I2C0; re-init inofensivo (misma config)
    Driver_Uart_init(&myUart);
    Driver_Uart_init(&myUart);
    game_init(&myGame);

    NVIC_ClearPendingIRQ(ADC12_0_INST_INT_IRQN);
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);

    NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
    __enable_irq();
    DL_TimerA_startCounter(TIMER_0_INST);

    while(1)
    {
        if(tickFlag)
        {
            tickFlag = 0;
            game_tick(&myGame);   // toda la logica corre fuera de la ISR
        }
    }
}

void TIMER_0_INST_IRQHandler(void)
{
    if(DL_TimerA_getPendingInterrupt(TIMER_0_INST) == DL_TIMERA_IIDX_ZERO)
        tickFlag = 1;
}

INTENSITY sensor_luzAIntensidad(uint16_t adc)
{
    if(adc <= 1740) return Intensity_1;
    uint32_t x = ((uint32_t)(adc - 1740) * 15u) / 2355u;
    return (INTENSITY)x;    // 0x0=Intensity_1 ... 0xF=Intensity_16
}
