#include "intercomm.h"
#include "game.h"

void send_data(uint8_t msg) {
  // Waiting until the USART TX data registor is !empty 
  while (!(USART1->ISR & USART_ISR_TXE));
  USART1->TDR = msg;
}

uint8_t read_data() {
  if (init_flag) { 
    if (!(USART1->ISR & USART_ISR_RXNE)) done_first = 0; 
  }
  
  // Waiting until the USART RX data register is !empty
  while(!(USART1->ISR & USART_ISR_RXNE)){};
  uint8_t x = USART1->RDR; 
  return x;
}

