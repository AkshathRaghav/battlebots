#include "setup.h"

char serfifo[FIFOSIZE]; 
int seroffset = 0; 

// Setup needed for SysTick
void initc() {
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
  GPIOC->MODER &= ~ (0x000000FF); 
  GPIOC->PUPDR|= 0x000000AA;
  GPIOC->MODER |= (0x00055500);

  //testing usart 

  GPIOC->MODER &= ~GPIO_MODER_MODER9;
  GPIOC->MODER |= GPIO_MODER_MODER9_0;  


  GPIOC->MODER &= ~GPIO_MODER_MODER8;
  GPIOC->MODER |= GPIO_MODER_MODER8_0;  
}

void initb() {
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
  GPIOB->MODER &= ~ (0x000003F3);
  GPIOB->PUPDR|= 0x000000A0;
  GPIOB->MODER |= GPIO_MODER_MODER12_0;
  GPIOB->MODER |= GPIO_MODER_MODER13_0;
  GPIOB->MODER |= (0x00550000); 
}

void inita(){    
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN; 

    GPIOA->MODER &= ~(GPIO_MODER_MODER9 | GPIO_MODER_MODER10);
    GPIOA->MODER |= (GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1); 

    GPIOA->AFR[1] &= ~(0xF << 4); //clearing 4 bits for pa 9
    GPIOA->AFR[1] &= ~(0xf << 8); //clearing 4 bits for pa 10

    GPIOA->AFR[1] |= 0x1 << 4; // setting pa9 to af1 
    GPIOA->AFR[1] |= 0x1 << 8;   // setting pa10 to af1

    GPIOA->MODER &= ~(GPIO_MODER_MODER0); 
    GPIOA->MODER |= GPIO_MODER_MODER0_0; 
}


void init_usart1(){
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN; 

    USART1->CR1 &= ~USART_CR1_UE; 
    
    USART1->CR1 &= ~USART_CR1_M; 
    USART1->CR2 &= ~USART_CR2_STOP; 
    USART1->CR1 &= ~USART_CR1_PCE;
    USART1->CR1 &= ~USART_CR1_OVER8; 
    USART1->BRR = 416; // 48M/115200
    USART1->CR1 |= USART_CR1_TE; 
    USART1->CR1 |= USART_CR1_RE; 

    USART1->CR1 |= USART_CR1_UE; 

    while (!(USART1->ISR & USART_ISR_TEACK));
    while (!(USART1->ISR & USART_ISR_REACK));
}

// Keyboard Handling
void set_col(int col) {
    GPIOC->ODR &=~ (GPIO_ODR_4 | GPIO_ODR_5| GPIO_ODR_6| GPIO_ODR_7) ;
    GPIOC->ODR |= (1<< (8-col));

}

void init_systick() {
  SysTick->LOAD= 187500-1; 
  SysTick->VAL = 0;
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
  SysTick->CTRL &=~ SysTick_CTRL_CLKSOURCE_Msk;
}

// Setup needed for SPI - TFT Display communication
void init_spi1_slow(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    SPI2->CR1 &= ~SPI_CR1_SPE; 
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    GPIOB->MODER &= ~(GPIO_MODER_MODER3 | GPIO_MODER_MODER4 | GPIO_MODER_MODER5); 
    GPIOB->MODER |= (GPIO_MODER_MODER3_1 | GPIO_MODER_MODER4_1 | GPIO_MODER_MODER5_1); 

    GPIOB->AFR[0] &= ~0x00fff000;

    SPI1->CR1 |= SPI_CR1_BR;
    SPI1->CR1 |= SPI_CR1_MSTR;
    SPI1->CR2 |= SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2; 
    SPI1->CR2 &= ~SPI_CR2_DS_3; 
    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
    SPI1->CR2 |= SPI_CR2_FRXTH;
    SPI1->CR1 |= SPI_CR1_SPE;
}

void enable_sdcard() {
    GPIOB->BSRR |= GPIO_BSRR_BR_2;
}

void disable_sdcard() {
    GPIOB->BSRR |= GPIO_BSRR_BS_2;
}

void init_sdcard_io() {
    init_spi1_slow();              
    GPIOB->MODER &= ~GPIO_MODER_MODER2;    
    GPIOB->MODER |= GPIO_MODER_MODER2_0;   
    disable_sdcard();               
}

void sdcard_io_high_speed() {
    SPI1->CR1 &= ~SPI_CR1_SPE;           

    SPI1->CR1 &= ~SPI_CR1_BR;            
    SPI1->CR1 |= SPI_CR1_BR_0;    
    SPI1->CR1 &= ~(SPI_CR1_BR_1 | SPI_CR1_BR_2);       

    SPI1->CR1 |= SPI_CR1_SPE;            
}

void init_lcd_spi() {
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER &= ~0xfff;
    GPIOB->MODER |= 0x10410000; 
    init_spi1_slow();
    sdcard_io_high_speed();  
}
