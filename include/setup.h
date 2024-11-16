#include "stm32f0xx.h"
#include "fifo.h"
#include "tty.h"
#include "commands.h"

#define GPIOC_IDR GPIOC->IDR
#define FIFOSIZE 16

void internal_clock();

// Setup needed for SysTick
void initc();
void initb();

// Keyboard Handling
void set_col(int col);
void init_systick();

// Set up needed for USART
void init_usart5();

// Setup needed for TTY Interfacing for Terminal 
void enable_tty_interrupt(void);
char interrupt_getchar();
int __io_putchar(int c);
int __io_getchar(void);
void USART3_8_IRQHandler(void);

// Setup needed for SPI - TFT Display communication
void init_spi1_slow(void);
void enable_sdcard();
void disable_sdcard();
void init_sdcard_io();
void sdcard_io_high_speed();
void init_lcd_spi();

