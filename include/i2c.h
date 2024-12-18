#include "stm32f0xx.h"
#include <stdio.h>

#define EEPROM_ADDR 0x57
#define LOC 0x0

void internal_clock();
void i2c_start(uint32_t targadr, uint8_t size, uint8_t dir);
void i2c_stop();
void i2c_waitidle();
void i2c_clearnack();
int i2c_checknack();
int8_t i2c_senddata(uint8_t targadr, uint8_t data[], uint8_t size);
int i2c_recvdata(uint8_t targadr, void *data, uint8_t size);

void eeprom_write(uint16_t loc, const char* data, uint8_t len);
void eeprom_read(uint16_t loc, char data[], uint8_t len);
int Num_Games_Updater();