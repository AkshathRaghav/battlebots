#include "stm32f0xx.h"

extern int done_first; 

void COMM_SendData(uint8_t msg);
uint8_t COMM_ReadData();
void _clear_uart_errors();