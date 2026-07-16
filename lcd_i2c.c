#include "lcd_i2c.h"

// Envia un byte crudo al puerto del PCF8574 con pulso de enable
static void lcd_writePort(lcd_t *ptr, uint8_t val)
{
    // Escribe con EN=1
    i2c_singleByteWrite(&ptr->myI2C, ptr->addr, (val | LCD_EN));
    delay_cycles(4000);   // 125us (AUMENTADO)
    // Escribe con EN=0 (pulso de enable)
    i2c_singleByteWrite(&ptr->myI2C, ptr->addr, (val & ~LCD_EN));
    delay_cycles(4000);   // 125us (AUMENTADO)
}

// Envia un nibble (los 4 bits altos de 'nib' van a D4-D7) con RS dado
static void lcd_writeNibble(lcd_t *ptr, uint8_t nib, uint8_t rs)
{
    uint8_t val = (nib & 0xF0) | LCD_BL | (rs ? LCD_RS : 0);
    lcd_writePort(ptr, val);
}

// Envia un byte completo (nibble alto, luego bajo). rs=0 comando, rs=1 dato
static void lcd_writeByte(lcd_t *ptr, uint8_t b, uint8_t rs)
{
    lcd_writeNibble(ptr, b & 0xF0, rs);          // nibble alto
    lcd_writeNibble(ptr, (b << 4) & 0xF0, rs);   // nibble bajo
    delay_cycles(1600);                          // ~50us: la mayoria de comandos
}

void lcd_init(lcd_t *ptr)
{
    i2c_init(&ptr->myI2C);
    delay_cycles(6400000);   // 200ms (DOBLADO)

    lcd_writeNibble(ptr, 0x30, 0); delay_cycles(640000);  // 200ms
    lcd_writeNibble(ptr, 0x30, 0); delay_cycles(64000);   // 2ms
    lcd_writeNibble(ptr, 0x30, 0); delay_cycles(64000);
    lcd_writeNibble(ptr, 0x20, 0); delay_cycles(64000);

    lcd_writeByte(ptr, 0x28, 0); delay_cycles(32000);
    lcd_writeByte(ptr, 0x0C, 0); delay_cycles(32000);
    lcd_writeByte(ptr, 0x06, 0); delay_cycles(32000);
    lcd_clear(ptr);
}

void lcd_clear(lcd_t *ptr)
{
    lcd_writeByte(ptr, 0x01, 0);
    delay_cycles(64000);   // clear tarda ~2ms
}

void lcd_setCursor(lcd_t *ptr, uint8_t col, uint8_t row)
{
    uint8_t base = (row == 0) ? 0x00 : 0x40;   // DDRAM: fila0=0x00, fila1=0x40
    lcd_writeByte(ptr, 0x80 | (base + col), 0);
}

void lcd_print(lcd_t *ptr, const char *str)
{
    while(*str) lcd_writeByte(ptr, (uint8_t)(*str++), 1);
}

void lcd_printNum(lcd_t *ptr, uint16_t n)
{
    char buf[6]; int8_t k = 0;
    if(n == 0){ lcd_writeByte(ptr, '0', 1); return; }
    while(n > 0){ buf[k++] = '0' + (n % 10); n /= 10; }
    while(k > 0) lcd_writeByte(ptr, buf[--k], 1);
}