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
#include "commands.h"
#include <stdio.h>

void internal_clock();

// Uncomment only one of the following to test each step
// #define STEP1
// #define STEP2
// #define STEP3
// #define STEP4


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


#ifdef STEP1
int main(void){
    internal_clock();
    init_usart5();
    for(;;) {
        while (!(USART5->ISR & USART_ISR_RXNE)) { }
        char c = USART5->RDR;
        while(!(USART5->ISR & USART_ISR_TXE)) { }
        USART5->TDR = c;
    }
}
#endif

#ifdef STEP2
#include <stdio.h>

int __io_putchar(int c) {
    if (c == '\n') { 
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
    while (!(USART5->ISR & USART_ISR_RXNE));
    char c = USART5->RDR;
    if (c == '\r') c = '\n';
    __io_putchar(c);
    return c;
}

int main() {
    internal_clock();
    init_usart5();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    printf("Enter your name: ");
    char name[80];
    fgets(name, 80, stdin);
    printf("Your name is %s", name);
    printf("Type any characters.\n");
    for(;;) {
        char c = getchar();
        putchar(c);
    }
}
#endif

#ifdef STEP3
#include <stdio.h>
#include "fifo.h"
#include "tty.h"

int __io_putchar(int c) {
    if (c == '\n') { 
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
    return line_buffer_getchar();
}

int main() {
    internal_clock();
    init_usart5();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    printf("Enter your name: ");
    char name[80];
    fgets(name, 80, stdin);
    printf("Your name is %s", name);
    printf("Type any characters.\n");
    for(;;) {
        char c = getchar();
        putchar(c);
    }
}
#endif

#ifdef STEP4

#include <stdio.h>
#include "fifo.h"
#include "tty.h"

#define FIFOSIZE 16

char serfifo[FIFOSIZE]; 
int seroffset = 0; 


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

int main() {
    internal_clock();
    init_usart5();
    enable_tty_interrupt();

    setbuf(stdin,0); // These turn off buffering; more efficient, but makes it hard to explain why first 1023 characters not dispalyed
    setbuf(stdout,0);
    setbuf(stderr,0);
    printf("Enter your name: "); // Types name but shouldn't echo the characters; USE CTRL-J to finish
    char name[80];
    fgets(name, 80, stdin);
    printf("Your name is %s", name);
    printf("Type any characters.\n"); // After, will type TWO instead of ONE
    for(;;) {
        char c = getchar();
        putchar(c);
    }
}
#endif

void parse_command(char *c)
{
    char *argv[20];
    int argc=0;
    int skipspace=1;
    for(; *c; c++) {
        if (skipspace) {
            if (*c != ' ' && *c != '\t') {
                argv[argc++] = c;
                skipspace = 0;
            }
        } else {
            if (*c == ' ' || *c == '\t') {
                *c = '\0';
                skipspace=1;
            }
        }
    }
    if (argc > 0) {
        argv[argc] = "";
        exec(argc, argv);
    }
}

void exec(int argc, char *argv[])
{
    //for(int i=0; i<argc; i++)
    //    printf("%d: %s\n", i, argv[i]);
    for(int i=0; usercmds[i].cmd != 0; i++)
        if (strcmp(usercmds[i].cmd, argv[0]) == 0) {
            usercmds[i].fn(argc, argv);
            return;
        }
    for(int i=0; i<sizeof cmds/sizeof cmds[0]; i++)
        if (strcmp(cmds[i].cmd, argv[0]) == 0) {
            cmds[i].fn(argc, argv);
            return;
        }
    printf("%s: No such command.\n", argv[0]);
}

void command_shell(void)
{
  char line[100];
  int len = strlen(line);
  puts("This is the STM32 command shell.");
  for(;;) {
      printf("> ");
      fgets(line, 99, stdin);
      line[99] = '\0';
      len = strlen(line);
      if (line[len-1]) == '\n'
          line[len-1] = '\0';
      parse_command(line);
  }
}

void init_spi1_slow() {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;   
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;    

    GPIOB->MODER &= ~(GPIO_MODER_MODER3 | GPIO_MODER_MODER4 | GPIO_MODER_MODER5); 
    GPIOB->MODER |= GPIO_MODER_MODER3_1 | GPIO_MODER_MODER4_1 | GPIO_MODER_MODER5_1;

    GPIOB->AFR[0] &= ~((0xF << (3 * 4)) | (0xF << (4 * 4)) | (0xF << (5 * 4))); 
    GPIOB->AFR[0] |= (5 << (3 * 4)) | (5 << (4 * 4)) | (5 << (5 * 4));

    SPI1->CR1 = SPI_CR1_MSTR             
              | SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0  
              | SPI_CR1_SSM | SPI_CR1_SSI; 

    SPI1->CR2 = SPI_CR2_FRXTH;           

    SPI1->CR1 |= SPI_CR1_SPE;            
}

void enable_sdcard() {
    GPIOB->ODR &= ~(1 << 2);  
}

void disable_sdcard() {
    GPIOB->ODR |= (1 << 2); 
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
    SPI1->CR1 |= SPI_CR1_BR_1;           

    SPI1->CR1 |= SPI_CR1_SPE;            
}

void init_lcd_spi() {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    GPIOB->MODER &= ~(GPIO_MODER_MODER8 | GPIO_MODER_MODER11 | GPIO_MODER_MODER14); 
    GPIOB->MODER |= GPIO_MODER_MODER8_0 | GPIO_MODER_MODER11_0 | GPIO_MODER_MODER14_0;

    init_spi1_slow();
    sdcard_io_high_speed();  
}


int main() {
    internal_clock();
    init_usart5();
    enable_tty_interrupt();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    command_shell();
    
}

