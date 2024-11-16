/**
  ******************************************************************************
  * @file    main.c
  * @author  Weili An, Niraj Menon
  * @date    Feb 7, 2024
  * @brief   ECE 362 Lab 7 student template
  ******************************************************************************
*/

/*******************************************************************************/

// Fill out your username!  Even though we're not using an autotest, 
// it should be a habit to fill out your username in this field now.
const char* username = "araviki";

/*******************************************************************************/ 

#include "stm32f0xx.h"
#include <stdint.h>
#include <stdio.h>
#include "fifo.h"
#include "tty.h"
#include "commands.h"

// #include "mvmnt_verification_blocks.c"

#define FIFOSIZE 16
char serfifo[FIFOSIZE]; 
int seroffset = 0; 

volatile int current_col = 1;


void internal_clock();

void delay(volatile uint32_t s){
    for(; s > 0; s--);
}

int cursor_x = 4; 
int cursor_y = 4; 

void init_usart5() {
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN; 
    RCC->AHBENR |= RCC_AHBENR_GPIODEN; 

    GPIOC->MODER &= ~GPIO_MODER_MODER12; 
    GPIOC->MODER |= GPIO_MODER_MODER12_1; 

    GPIOC->AFR[1] |= 0x2 << 16; 

    GPIOD->MODER &= ~GPIO_MODER_MODER2; 
    GPIOD->MODER |= GPIO_MODER_MODER2_1; 
    GPIOD->AFR[0] |= 0x2 << 8;

    RCC->APB1ENR |= RCC_APB1ENR_USART5EN; 

    USART5->CR1 &= ~USART_CR1_UE; 
    USART5->CR1 &= ~USART_CR1_M; 
    USART5->CR2 &= ~USART_CR2_STOP; 
    USART5->CR1 &= ~USART_CR1_PCE;
    USART5->CR1 &= ~USART_CR1_OVER8; 
    USART5->BRR = 416; // 48M/115200
    USART5->CR1 |= USART_CR1_TE; 
    USART5->CR1 |= USART_CR1_RE; 

    USART5->CR1 |= USART_CR1_UE; 

    while (!(USART5->ISR & USART_ISR_TEACK));
    while (!(USART5->ISR & USART_ISR_REACK));
}

void enable_tty_interrupt(void) {
    RCC->AHBENR |= RCC_AHBENR_DMA2EN; 
    DMA2->CSELR |= DMA2_CSELR_CH2_USART5_RX; 

    // switch off 
    DMA2_Channel2->CCR &= ~DMA_CCR_EN; 
    USART5->CR1 &= ~USART_CR1_UE;

    DMA2_Channel2->CMAR = serfifo; 
    DMA2_Channel2->CPAR = &(USART5->RDR);
    DMA2_Channel2->CNDTR = FIFOSIZE; 
    DMA2_Channel2->CCR &= ~DMA_CCR_DIR; 
    DMA2_Channel2->CCR &= ~DMA_CCR_TCIE; 
    DMA2_Channel2->CCR &= ~DMA_CCR_HTIE; 

    DMA2_Channel2->CCR &= ~DMA_CCR_MSIZE;
    DMA2_Channel2->CCR &= ~DMA_CCR_PSIZE;
    DMA2_Channel2->CCR |= (0x00 << DMA_CCR_MSIZE_0);
    DMA2_Channel2->CCR |= (0x00 << DMA_CCR_PSIZE_0);

    DMA2_Channel2->CCR |= DMA_CCR_MINC; 
    DMA2_Channel2->CCR &= ~DMA_CCR_PINC; 

    DMA2_Channel2->CCR |= DMA_CCR_CIRC; 
    DMA2_Channel2->CCR &= ~DMA_CCR_MEM2MEM; 

    DMA2_Channel2->CCR |= DMA_CCR_PL_1; 

    DMA2_Channel2->CCR |= DMA_CCR_EN; 

    USART5->CR3 |= USART_CR3_DMAR; 
    USART5->CR1 |= USART_CR1_RXNEIE; 
    NVIC_EnableIRQ(USART3_8_IRQn);

    // turn back on 
    USART5->CR1 |= USART_CR1_UE; 
}

// Works like line_buffer_getchar(), but does not check or clear ORE nor wait on new characters in USART
char interrupt_getchar() {
    while (!fifo_newline(&input_fifo)) asm volatile ("wfi");

    return fifo_remove(&input_fifo);
}

int __io_putchar(int c) {
    if (c == "\n") { 
        while(!(USART5->ISR & USART_ISR_TXE));
        USART5->TDR = '\r'; 
        while(!(USART5->ISR & USART_ISR_TXE));
        USART5->TDR = '\n'; 
    } else {
        while(!(USART5->ISR & USART_ISR_TXE));
        USART5->TDR = c;
    }
    return c;
}

int __io_getchar(void) {
    char c = interrupt_getchar();
    return c;
}

void USART3_8_IRQHandler(void) {
    while(DMA2_Channel2->CNDTR != sizeof serfifo - seroffset) {
        if (!fifo_full(&input_fifo))
            insert_echo_char(serfifo[seroffset]);
        seroffset = (seroffset + 1) % sizeof serfifo;
    }
}


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

int game_over = 0; 

void play(){
    parse_command("clean_f"); 
    parse_command("gird"); 
    fill_grid(COLOR_WHITE); 

    while(!game_over){
        dots_loc_color(cursor_x, cursor_y, COLOR_GREEN);
    }
}

void setup(){
    parse_command("lcd_init");
    parse_command("grid");
    parse_command("dots");

}



void init_systick() {
  SysTick->LOAD= 32000000-1; //should scan it 32 times per sec. Decently Responsive as well. Might be better since does not check systick interrupt as often as before. 
  SysTick->VAL = 0;
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
  SysTick->CTRL &=~ SysTick_CTRL_CLKSOURCE_Msk; //here we are setting the clksrc bit to 0 to get 6MHz.
}


void SysTick_Handler(){
    cursor_x += 1; 
        dots_loc_color(5, cursor_y, COLOR_GREEN); 

    // if(current_col == 3)
    // {
        
    //   if(GPIOC->IDR & 0x1)
    //   {
    //     //# symbol. R4, C3
    //     /*
    //       This is where code for Confirm Place would go.
    //     */
    //     // confirm();
    //     // dots_loc_color(cursor_x, cursor_y, COLOR_WHITE);
        
    //     GPIOB->BRR = GPIO_BRR_BR_10;
    //     GPIOB->BRR = GPIO_BRR_BR_9;
    //     GPIOB->BRR = GPIO_BRR_BR_8;
    //     GPIOB->BSRR = GPIO_BSRR_BS_10;
    //   }
    //   else if(GPIOC->IDR & 0x4)
    //   {
    //     //6. R2, C3
    //     /*
    //       This is where code for Mv Right would go.
    //     */
    //     // mv_right();
    //     GPIOB->BRR = GPIO_BRR_BR_10;
    //     GPIOB->BRR = GPIO_BRR_BR_9;
    //     GPIOB->BRR = GPIO_BRR_BR_8;
    //     GPIOB->BSRR = GPIO_BSRR_BS_9;
    //   }
      
    // }
    // if(current_col == 2)
    // {
    //     //8 R3, C2
    //     /*
    //       This is where code for Mv Down would go.
    //     */
    // //    mv_down();
    //   if(GPIOC->IDR &  0x2)
    //   {
    //     GPIOB->BRR = GPIO_BRR_BR_10;
    //     GPIOB->BRR = GPIO_BRR_BR_9;
    //     GPIOB->BRR = GPIO_BRR_BR_8;
    //     GPIOB->BSRR = GPIO_BSRR_BS_9;
    //     GPIOB->BSRR = GPIO_BSRR_BS_10;

    //   }
    //   else if(GPIOC->IDR & 0x4)
    //   {
    //     //5, R2, C2
    //     /*
    //       This is where code for Rotate would go.
    //     */
    //     // mv_rot();
    //     GPIOB->BRR = GPIO_BRR_BR_10;
    //     GPIOB->BRR = GPIO_BRR_BR_9;
    //     GPIOB->BRR = GPIO_BRR_BR_8;
    //     GPIOB->BSRR = GPIO_BSRR_BS_8;

    //   }
    //   else if(GPIOC->IDR & 0x8)
    //   {
    //     //2, R1, C2
    //     /*
    //       This is where code for Mv Up would go.
    //     */
    //     // mv_up();
    //     GPIOB->BRR = GPIO_BRR_BR_10;
    //     GPIOB->BRR = GPIO_BRR_BR_9;
    //     GPIOB->BRR = GPIO_BRR_BR_8;
    //     GPIOB->BSRR = GPIO_BSRR_BS_8;
    //     GPIOB->BSRR = GPIO_BSRR_BS_10;
    //   }
    // }
    // if(current_col == 1)
    // {
    //   if(GPIOC->IDR & 0x4)
    //   {
    //     //4, R2, C1
    //     /*
    //       This is where code for Mv Left would go.
    //     */
    //     // mv_left();
    //     GPIOB->BRR = GPIO_BRR_BR_10;
    //     GPIOB->BRR = GPIO_BRR_BR_9;
    //     GPIOB->BRR = GPIO_BRR_BR_8;
    //     GPIOB->BSRR = GPIO_BSRR_BS_8;
    //     GPIOB->BSRR = GPIO_BSRR_BS_9;
    //   }
    //   else if(GPIOC->IDR & 0x1)
    //   {
    //     //* symbol. R4, C1
    //     /*
    //       This is where code for Start would go.
    //     */
    //     // start();
    //     GPIOB->BRR = GPIO_BRR_BR_10;
    //     GPIOB->BRR = GPIO_BRR_BR_9;
    //     GPIOB->BRR = GPIO_BRR_BR_8;
    //     GPIOB->BSRR = GPIO_BSRR_BS_8;
    //     GPIOB->BSRR = GPIO_BSRR_BS_9;
    //     GPIOB->BSRR = GPIO_BSRR_BS_10;
    //   }
    // }

    // current_col++;
    // if(current_col > 4)
    // {
    //   current_col = 1;
    // }
    // set_col(current_col);
}

void set_col(int col) {
    GPIOC->ODR &=~ (GPIO_ODR_4 | GPIO_ODR_5| GPIO_ODR_6| GPIO_ODR_7) ;

    GPIOC->ODR |= (1<< (8-col));
}

void initb() {
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
  GPIOB->MODER &= ~ (0x000003F3);
  GPIOB->PUPDR|= 0x000000A0;

  GPIOB->MODER |= (0x00550000); //the entire hex value is a bit mask

}

void initc() {
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
  //0-3 input: first 8 bits will be 00
  GPIOC->MODER &= ~ (0x000000FF); 
  GPIOC->PUPDR|= 0x000000AA;

  //4-9 as output: 
  GPIOC->MODER |= (0x00055500);
}

void inita() { 
    ;
}

int main() {
    internal_clock();
    init_usart5();
    initb();
    initc();
    inita();

    enable_tty_interrupt();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);

    setup();

    init_systick();

    play();
}

