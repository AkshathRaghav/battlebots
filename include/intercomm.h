#include "stm32f0xx.h"
#define TIMEOUT 100000;

extern int done_first; 

void send_data(uint8_t msg);
uint8_t read_data();
void clear_uart_errors();