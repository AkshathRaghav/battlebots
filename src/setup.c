#include "stm32f0xx.h"
#include "fifo.h"
#include "tty.h"
#include "commands.h"

#define FIFOSIZE 16
char serfifo[FIFOSIZE]; 
int seroffset = 0; 

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
