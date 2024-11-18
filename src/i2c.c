/**
  ******************************************************************************
  * @file    main.c
  * @author  Niraj Menon
  * @date    Sep 23, 2024
  * @brief   ECE 362 I2C Lab Student template
  ******************************************************************************
*/

#include "stm32f0xx.h"
#include <stdio.h>

void internal_clock();
void enable_ports();
void init_i2c();
void i2c_start(uint32_t targadr, uint8_t size, uint8_t dir);
void i2c_stop();
void i2c_waitidle();
void i2c_clearnack();
int i2c_checknack();
void nano_wait(unsigned int n);

void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}
//===========================================================================
// Configure SDA and SCL.
//===========================================================================
void enable_ports(void) {
    //enable clk for i2c1 scl and sda.
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    //configure PB6 and PB9 for AF.
    GPIOB->MODER |= GPIO_MODER_MODER6_1 |GPIO_MODER_MODER9_1;
    GPIOB->AFR[0] |= (1<<GPIO_AFRL_AFRL6_Pos); //PB6 is AF1
    GPIOB->AFR[1] |= (1<<GPIO_AFRH_AFRH1_Pos); //PB9 is AF1
}

//===========================================================================
// Configure I2C1.
//===========================================================================
void init_i2c(void) {
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    I2C1->CR1 &=~ I2C_CR1_PE; //I2C disable
    I2C1->CR1 |= I2C_CR1_ANFOFF; //analog filtering off
    I2C1->CR1 |= I2C_CR1_ERRIE; //error interrupts enable
    I2C1->CR1 |= I2C_CR1_NOSTRETCH; //DISABLES clock stretching

    //clk stuff.    Need to check.
    // I2C1->TIMINGR = 5 << I2C_TIMINGR_PRESC_Pos; //48 /(5 + 1) = 8 Mhz
    // I2C1->TIMINGR |= (0x9) << I2C_TIMINGR_SCLL_Pos;
    // I2C1->TIMINGR |= (0x3) << I2C_TIMINGR_SCLH_Pos;
    // I2C1->TIMINGR |= (0x1) << I2C_TIMINGR_SDADEL_Pos;
    // I2C1->TIMINGR |= (0x3) << I2C_TIMINGR_SCLDEL_Pos;
    I2C1->TIMINGR = 0x00B01A4B;

    //addressing
    I2C1->CR2 &=~ I2C_CR2_ADD10; //7bit addressing mode
    I2C1->CR1 |= I2C_CR1_PE; //I2C enable


    
}

//===========================================================================
// Send a START bit.
//===========================================================================
void i2c_start(uint32_t targadr, uint8_t size, uint8_t dir) {
    // 0. Take current contents of CR2 register. 
    uint32_t tmpreg = I2C1->CR2;

    // 1. Clear the following bits in the tmpreg: SADD, NBYTES, RD_WRN, START, STOP
    tmpreg &=~ (I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP);
    // 2. Set read/write direction in tmpreg.
    if(dir == 0x0)
    {
        tmpreg &=~ I2C_CR2_RD_WRN;
    }   
    else 
    {
        tmpreg |= I2C_CR2_RD_WRN;

    }
    // 3. Set the target's address in SADD (shift targadr left by 1 bit) and the data size.
    tmpreg |= ((targadr<<1) & I2C_CR2_SADD) | ((size << 16) & I2C_CR2_NBYTES);

    // 4. Set the START bit.
    tmpreg |= I2C_CR2_START;

    // 5. Start the conversion by writing the modified value back to the CR2 register.
    I2C1->CR2 = tmpreg;
}

//===========================================================================
// Send a STOP bit.
//===========================================================================
void i2c_stop(void) {
// 0. If a STOP bit has already been sent, return from the function.
// Check the I2C1 ISR register for the corresponding bit.
    if((I2C1->ISR & I2C_ISR_STOPF))
    {
        return;
    }
    // if(I2C1->CR2 & I2C_CR2_STOP) TRY IN CASE SHRIKAR.
// 1. Set the STOP bit in the CR2 register.
    I2C1->CR2 |= I2C_CR2_STOP;
// 2. Wait until STOPF flag is reset by checking the same flag in ISR.
    while(!(I2C1->ISR & I2C_ISR_STOPF))
    {
        //DO !(I2C1->ISR & I2C_ISR_STOPF) IF GETTING STUCK HERE!
        //wait for stop to be asserted
    }
// 3. Clear the STOPF flag by writing 1 to the corresponding bit in the ICR.
    I2C1->ICR |= I2C_ICR_STOPCF;
}

//===========================================================================
// Wait until the I2C bus is not busy. (One-liner!)
//===========================================================================
void i2c_waitidle(void) {
  while(I2C1->ISR & I2C_ISR_BUSY){
    //wait till i2c is not busy.
  }
}

//===========================================================================
// Send each char in data[size] to the I2C bus at targadr.
//===========================================================================
int8_t i2c_senddata(uint8_t targadr, uint8_t data[], uint8_t size) {
    i2c_waitidle(); //waits until i2c is idle
    // i2c_start(uint32_t targadr, uint8_t size, uint8_t dir) 
    i2c_start(targadr, size, 0); //check 0 for dir. 0: write transfer
    
    for(int i = 0; i<size; i++)
    {
        int count = 0;
        while ((I2C1->ISR & I2C_ISR_TXIS) == 0) {
            count += 1;
            if (count > 1000000)
                return -1;
            if (i2c_checknack()) {
                i2c_clearnack();
                i2c_stop();
                return -1;
            }
        }

        I2C1->TXDR = data[i] & I2C_TXDR_TXDATA; //masking data[i] with I2C_TXDR_TXDATA. CHECK?
    }

    while (!(I2C1->ISR & I2C_ISR_TC) && !(I2C1->ISR & I2C_ISR_NACKF))
    {
        //wait till TC and NACKF flags are not set.
    }

    if((I2C1->ISR & I2C_ISR_NACKF))
    {
        return -1; //target device did not ack data
    }

    i2c_stop();

    return 0;
}

//===========================================================================
// Receive size chars from the I2C bus at targadr and store in data[size].
//===========================================================================
int i2c_recvdata(uint8_t targadr, void *data, uint8_t size) {
    char* temp_data = (char*) data;
    i2c_waitidle();
    i2c_start(targadr, size, 1); //check 1 for dir. 1: read transfer

    for(int i = 0; i< size ; i++)
    {
        int count = 0;
        while ((I2C1->ISR & I2C_ISR_RXNE) == 0) {
            count += 1;
            if (count > 1000000)
                return -1;
            if (i2c_checknack()) {
                i2c_clearnack();
                i2c_stop();
                return -1;
            }
        }
        temp_data[i] = I2C1->RXDR & I2C_RXDR_RXDATA;
    }
    return 0;

}

//===========================================================================
// Clear the NACK bit. (One-liner!)
//===========================================================================
void i2c_clearnack(void) {
    I2C1->ICR &=~ I2C_ICR_NACKCF;
}

//===========================================================================
// Check the NACK bit. (One-liner!)
//===========================================================================
int i2c_checknack(void) {
    if(I2C1->ISR & I2C_ISR_NACKF) return 1;
    else return 0;
}

//===========================================================================
// EEPROM functions
// We'll give these so you don't have to figure out how to write to the EEPROM.
// These can differ by device.

#define EEPROM_ADDR 0x57
//LOC HAS TO BE MULTIPLE OF 32
//WRITE TO EEPROM
//wait for nanowait(10000000) * 2
//READ EEPROM
//DISPLAY
void eeprom_write(uint16_t loc, const char* data, uint8_t len) {
    uint8_t bytes[34];
    bytes[0] = loc>>8;
    bytes[1] = loc&0xFF;
    for(int i = 0; i<len; i++){
        bytes[i+2] = data[i];
    }
    i2c_senddata(EEPROM_ADDR, bytes, len+2);
}

void eeprom_read(uint16_t loc, char data[], uint8_t len) {
    // ... your code here
    uint8_t bytes[2];
    bytes[0] = loc>>8;
    bytes[1] = loc&0xFF;
    i2c_senddata(EEPROM_ADDR, bytes, 2);
    i2c_recvdata(EEPROM_ADDR, data, len);
}

//===========================================================================
// Copy in code from Lab 7 - USART
//===========================================================================

#include "fifo.h"
#include "tty.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FIFOSIZE 16
char serfifo[FIFOSIZE];
int seroffset = 0;

//===========================================================================
// init_usart5
//===========================================================================
void init_usart5() {
    // TODO
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIODEN;
    GPIOC->MODER &=~ (GPIO_MODER_MODER12_1 | GPIO_MODER_MODER2_1);
    GPIOC->MODER |= GPIO_MODER_MODER12_1; //setting to alt func.
    GPIOD->MODER |= GPIO_MODER_MODER2_1;
    GPIOC->AFR[1] |= 2 << 16;
    GPIOD->AFR[0] |= 2<< 8;

    RCC->APB1ENR |= RCC_APB1ENR_USART5EN;

    USART5->CR1 &=~ (USART_CR1_UE);

    USART5->CR1 &=~ (USART_CR1_M1 | USART_CR1_M0);
    USART5->CR2 &=~ USART_CR2_STOP;
    USART5->CR1 &=~ (USART_CR1_OVER8);

    USART5->BRR = 0x1A1;

    USART5->CR1 |= USART_CR1_TE | USART_CR1_RE;

    USART5->CR1 |= (USART_CR1_UE);

    //USART_ISR_TEACK and USART_ISR_REACK check
    while(!(USART5->ISR & (USART_ISR_TEACK | USART_ISR_TEACK)));
}


//===========================================================================
// enable_tty_interrupt
//===========================================================================
void enable_tty_interrupt(void) {
    // TODO
    //config dma and usart to check  receive data register   
    USART5->CR1|= USART_CR1_RXNEIE;
    USART5->CR3 |= USART_CR3_DMAR;
    NVIC->ISER[0] = 1<< USART3_8_IRQn;

    RCC->AHBENR |= RCC_AHBENR_DMA2EN;
    DMA2->CSELR |= DMA2_CSELR_CH2_USART5_RX;
    DMA2_Channel2->CCR &= ~DMA_CCR_EN;  // First make sure DMA is turned off
    
    // The DMA channel 2 configuration goes here
    DMA2_Channel2->CMAR = serfifo;
    DMA2_Channel2->CPAR = &(USART5->RDR);
    DMA2_Channel2->CNDTR = FIFOSIZE;
    DMA2_Channel2->CCR &=~ DMA_CCR_DIR;
    DMA2_Channel2->CCR &=~ (DMA_CCR_MSIZE | DMA_CCR_PSIZE);
    DMA2_Channel2->CCR |= DMA_CCR_MINC;
    DMA2_Channel2->CCR &=~ DMA_CCR_PINC;
    DMA2_Channel2->CCR |= DMA_CCR_CIRC;
    DMA2_Channel2->CCR |= DMA_CCR_PL_Msk;
    DMA2_Channel2->CCR |= DMA_CCR_EN;

}

//===========================================================================
// interrupt_getchar
//===========================================================================

char interrupt_getchar() {
    // TODO

    // Wait for a newline to complete the buffer.
    while(fifo_newline(&input_fifo) == 0) //is 0 until no newline
    { 
        asm volatile ("wfi"); // wait for an interrupt
    }
    // Return a character from the line buffer.
    char ch = fifo_remove(&input_fifo);
    return ch;
}


//===========================================================================
// __io_putchar
//===========================================================================

int __io_putchar(int c) {
    // TODO copy from STEP2
        // TODO
    if(c == '\n'){
        while(!(USART5->ISR & USART_ISR_TXE));
        USART5->TDR = '\r';
    }
    while(!(USART5->ISR & USART_ISR_TXE)); //will only be !(1) when the TXE bit in ISR has become 1.
    
    USART5->TDR = c;
    return c;
}
//===========================================================================
// __io_getchar
//===========================================================================

int __io_getchar(void) {
    // TODO Use interrupt_getchar() instead of line_buffer_getchar()
    return(interrupt_getchar());
}
//===========================================================================
// IRQHandler for USART5
//===========================================================================
void USART3_8_IRQHandler(void) {
    while(DMA2_Channel2->CNDTR != sizeof serfifo - seroffset) {
        if (!fifo_full(&input_fifo))
            insert_echo_char(serfifo[seroffset]);
        seroffset = (seroffset + 1) % sizeof serfifo;
    }
}

//===========================================================================
// Command functions for the shell
// These are the functions that are called when a command is entered, in 
// order to parse the command being entered, split into argc/argv calling 
// convention, and then execute the command by calling the respective 
// function.  We implement "write" and "read" for you to easily test your 
// EEPROM functions from a terminal.
//===========================================================================

void write(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: write <addr> <data>\n");
        printf("Ensure the address is a hexadecimal number.  No need to include 0x.\n");
        return;
    }
    uint32_t addr = atoi(argv[1]); 
    // concatenate all args from argv[2], until empty string is found, to a string
    char data[32] = "";
    int i = 0;
    int j = 2;
    while (strcmp(argv[j], "") != 0 && i < 32) {
        for (char c = argv[j][0]; c != '\0'; c = *(++argv[j])) {
            data[i++] = c;
        }
        if (strcmp(argv[j+1], "") != 0)
            data[i++] = ' ';
        j++;
    }
    // ensure addr is a multiple of 32
    if ((addr % 10) != 0) {
        printf("Address 0x%ld is not evenly divisible by 32.  Your address must be a hexadecimal value.\n", addr);
        return;
    }
    int msglen = strlen(data);
    if (msglen > 32) {
        printf("Data is too long. Max length is 32.\n");
        return;
    }
    printf("Writing to address 0x%ld: %s\n", addr, data);
    eeprom_write(addr, data, msglen);
}

void read(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: read <addr>\n");
        printf("Ensure the address is a hexadecimal number.  No need to include 0x.\n");
        return;
    }
    uint32_t addr = atoi(argv[1]); 
    char data[32];
    // ensure addr is a multiple of 32
    if ((addr % 10) != 0) {
        printf("Address 0x%ld is not evenly divisible by 32.  Your address must be a hexadecimal value.\n", addr);
        return;
    }
    eeprom_read(addr, data, 32);
    printf("String at address 0x%ld: %s\n", addr, data);
}

struct commands_t {
    const char *cmd;
    void      (*fn)(int argc, char *argv[]);
};

struct commands_t cmds[] = {
    { "write", write },
    { "read", read }
};

void exec(int argc, char *argv[])
{
    for(int i=0; i<sizeof cmds/sizeof cmds[0]; i++)
        if (strcmp(cmds[i].cmd, argv[0]) == 0) {
            cmds[i].fn(argc, argv);
            return;
        }
    printf("%s: No such command.\n", argv[0]);
}

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

//===========================================================================
// main()
//===========================================================================

int main() {
    internal_clock();
    
    // I2C specific
    enable_ports();
    init_i2c();

    // If you don't want to deal with the command shell, you can 
    // comment out all code below and call 
    // eeprom_read/eeprom_write directly.
    init_usart5();
    enable_tty_interrupt();
    // These turn off buffering.
    setbuf(stdin,0); 
    setbuf(stdout,0);
    setbuf(stderr,0);

    printf("I2C Command Shell\n");
    printf("This is a simple shell that allows you to write to or read from the I2C EEPROM at %d.\n", EEPROM_ADDR);
    for(;;) {
        printf("\n> ");
        char line[100];
        fgets(line, 99, stdin);
        line[99] = '\0';
        int len = strlen(line);
        if (line[len-1] == '\n')
            line[len-1] = '\0';
        parse_command(line);
    }


    // void eeprom_write(uint16_t loc, const char* data, uint8_t len) {
    uint16_t location = 0x20;
    char* data = "Player 1 Won!";
    uint8_t len = sizeof(data);
    eeprom_write(location, data, len);
    nano_wait(100000000);
    char * data_buf;
    eeprom_read(location, data_buf, len);

    for(int i = 0; i< len; i++)
    {
        printf(data[i]);
    }
    nano_wait(100000000);
}
