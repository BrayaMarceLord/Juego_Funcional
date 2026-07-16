#ifndef LCD_I2C_H
#define LCD_I2C_H

    #include <ti/driverlib/driverlib.h>  
    #include "1_Drivers/i2c.h"
    #define LCD_RS   0x01  // bit 0
    #define LCD_RW   0x02  // bit 1
    #define LCD_EN   0x04  // bit 2
    #define LCD_BL   0x08  // bit 3
    
    typedef struct{
        i2c_t   myI2C;
        uint8_t addr;       // 0x27 para PCF8574T
    }lcd_t;

    extern void lcd_init(lcd_t *ptr);
    extern void lcd_clear(lcd_t *ptr);
    extern void lcd_setCursor(lcd_t *ptr, uint8_t col, uint8_t row);
    extern void lcd_print(lcd_t *ptr, const char *str);
    extern void lcd_printNum(lcd_t *ptr, uint16_t n);

#endif