void new_initb() {
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
  GPIOB->MODER |= GPIO_MODER_MODER12_0;
}

void set_led(int on)
{
  if(on){
    GPIOB->BSRR |= GPIO_BSRR_BS_12;
  }
  if(!on)
  {
    GPIOB->BSRR |= GPIO_BSRR_BR_12;
  }
}
