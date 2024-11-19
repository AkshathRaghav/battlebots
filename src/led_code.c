void initb() {
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
  GPIOB->MODER &= ~ (0x000003F3);
  GPIOB->PUPDR|= 0x000000A0;

  GPIOB->MODER |= (0x00550000); //the entire hex value is a bit mask

}
