#include "intercomm.h"
#include "game.h"

void COMM_SendData(uint8_t msg) {
  // Waiting until the USART TX data registor is !empty 
  _clear_uart_errors(); 

  while (!(USART1->ISR & USART_ISR_TXE));
  USART1->TDR = msg;
}

uint8_t COMM_ReadData() {
  if (init_flag) { 
    if (!(USART1->ISR & USART_ISR_RXNE)) done_first = 0; 
  }
  
  // Waiting until the USART RX data register is !empty
  while(!(USART1->ISR & USART_ISR_RXNE)){};
  uint8_t x = USART1->RDR; 
  return x;
}

void _clear_uart_errors() {
  if (USART1->ISR & USART_ISR_ORE) {
    volatile uint8_t dummy = USART1->RDR; 
    (void)dummy;
    USART1->ICR |= USART_ICR_ORECF;    
  }
  if (USART1->ISR & (USART_ISR_FE | USART_ISR_NE)) {
    volatile uint8_t dummy = USART1->RDR; 
    (void)dummy;
  }
}
