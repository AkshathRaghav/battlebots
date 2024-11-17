#include "intercomm.h"

void send_data(int8_t msg){
  //Waiting until the Tx data registor is not empty 
  while (!(USART1->ISR & USART_ISR_TXE)){};
  
  USART1->TDR  = msg;

  for(int i =0 ; i < 1000000; i ++); 
  GPIOC->ODR &= ~(GPIO_ODR_9); 
}

int8_t read_data(){
  //Waiting until the usart data registor is not empty 
  while(!(USART1->ISR & USART_ISR_RXNE)){};
  
  int x = USART1->RDR; 
  return x; 
}