/**
  ******************************************************************************
  * @file    main.c
  * @author  Weili An, Niraj Menon
  * @date    Feb 3, 2024
  * @brief   ECE 362 Lab 6 Student template
  ******************************************************************************
*/

/*******************************************************************************/

// Fill out your username, otherwise your completion code will have the 
// wrong username!
const char* username = "bhide1";

/*******************************************************************************/ 
#include <stdint.h>
#include <stdio.h>
#include "setup.h"
#include "stm32f0xx.h"

void set_char_msg(int, char);
void nano_wait(unsigned int);
void game(void);
void internal_clock();
void check_wiring();
void autotest();

//===========================================================================
// Configure GPIOC
//===========================================================================
void enable_ports(void) {
    // Only enable port C for the keypad
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC->MODER &= ~0xffff;
    GPIOC->MODER |= 0x55 << (4*2);
    GPIOC->OTYPER &= ~0xff;
    GPIOC->OTYPER |= 0xf0;
    GPIOC->PUPDR &= ~0xff;
    GPIOC->PUPDR |= 0x55;
}


uint8_t col; // the column being scanned

void drive_column(int);   // energize one of the column outputs
int  read_rows();         // read the four row inputs
void update_history(int col, int rows); // record the buttons of the driven column
char get_key_event(void); // wait for a button event (press or release)
char get_keypress(void);  // wait for only a button press event.
float getfloat(void);     // read a floating-point number from keypad
void show_keys(void);     // demonstrate get_key_event()



void small_delay(void) {
    nano_wait(50000);
}

//===========================================================================
// Configure timer 7 to invoke the update interrupt at 1kHz
// Copy from lab 4 or 5.
//===========================================================================

void init_tim7(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    TIM7->PSC = 2400-1;
    TIM7->ARR = 20-1;
    TIM7->DIER |= TIM_DIER_UIE;
    TIM7->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] |= (1<<TIM7_IRQn);

}

//===========================================================================
// Copy the Timer 7 ISR from lab 5
//===========================================================================
// TODO To be copied
void TIM7_IRQHandler()
{
    TIM7->SR &= ~TIM_SR_UIF;
    int rows = read_rows();
    update_history(col, rows);
    col = (col + 1) & 3;
    drive_column(col);
}


// 16 history bytes.  Each byte represents the last 8 samples of a button.
uint8_t hist[16];
char queue[2];  // A two-entry queue of button press/release events.
int qin;        // Which queue entry is next for input
int qout;       // Which queue entry is next for output

const char keymap[] = "DCBA#9630852*741";

void push_queue(int n) {
    queue[qin] = n;
    qin ^= 1;
}

char pop_queue() {
    char tmp = queue[qout];
    queue[qout] = 0;
    qout ^= 1;
    return tmp;
}

void update_history(int c, int rows)
{
    // We used to make students do this in assembly language.
    for(int i = 0; i < 4; i++) {
        hist[4*c+i] = (hist[4*c+i]<<1) + ((rows>>i)&1);
        if (hist[4*c+i] == 0x01)
            push_queue(0x80 | keymap[4*c+i]);
        if (hist[4*c+i] == 0xfe)
            push_queue(keymap[4*c+i]);
    }
}

void drive_column(int c)
{
    GPIOC->BSRR = 0xf00000 | ~(1 << (c + 4));
}

int read_rows()
{
    return (~GPIOC->IDR) & 0xf;
}

char get_key_event(void) {
    for(;;) {
        asm volatile ("wfi");   // wait for an interrupt
        if (queue[qout] != 0)
            break;
    }
    return pop_queue();
}

char get_keypress() {
    char event;
    for(;;) {
        // Wait for every button event...
        event = get_key_event();
        // ...but ignore if it's a release.
        if (event & 0x80)
            break;
    }
    return event & 0x7f;
}

void show_keys(void)
{
    char buf[] = "        ";
    for(;;) {
        char event = get_key_event();
        memmove(buf, &buf[1], 7);
        buf[7] = event;
        print(buf);
    }
}

int score = 0;
char disp1[17] = "                ";
char disp2[17] = "                ";
volatile int pos = 0;
void TIM17_IRQHandler(void)
{
    TIM17->SR &= ~TIM_SR_UIF;
    // memmove(disp1, &disp1[1], 16);
    // memmove(disp2, &disp2[1], 16);
    // if (pos == 0) {
    //     if (disp1[0] != ' ')
    //         score -= 1;
    //     if (disp2[0] != ' ')
    //         score += 1;
    //     disp1[0] = '>';
    // } else {
    //     if (disp2[0] != ' ')
    //         score -= 1;
    //     if (disp1[0] != ' ')
    //         score += 1;
    //     disp2[0] = '>';
    // }
    // int create = random() & 3;
    // if (create == 0) { // one in four chance
    //     int line = random() & 1;
    //     if (line == 0) { // pick a line
    //         disp1[15] = 'x';
    //         disp2[15] = ' ';
    //     } else {
    //         disp1[15] = ' ';
    //         disp2[15] = 'x';
    //     }
    // } else {
    //     disp1[15] = ' ';
    //     disp2[15] = ' ';
    // }
    // if (pos == 0)
    //     disp1[0] = '>';
    // else
    //     disp2[0] = '>';
    // if (score >= 100) {
    //     print("Score100");
    //     spi1_dma_display1("Game over");
    //     spi1_dma_display2("You win");
    //     NVIC->ICER[0] = 1<<TIM17_IRQn;
    //     return;
    // }
    // char buf[9];
    // snprintf(buf, 9, "Score% 3d", score);
    // print(buf);
    // spi1_dma_display1(disp1);
    // spi1_dma_display2(disp2);
    TIM17->ARR = 250 - 1 - 2*score;
}

void init_tim17(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;
    TIM17->PSC = 48000 - 1;
    TIM17->ARR = 250 - 1;
    TIM17->CR1 |= TIM_CR1_ARPE;
    TIM17->DIER |= TIM_DIER_UIE;
    TIM17->CR1 |= TIM_CR1_CEN;
}

void init_spi2(void);
void spi2_setup_dma(void);
void spi2_enable_dma(void);
void init_spi1(void);
void spi1_init_oled(void);
void spi1_setup_dma(void);
void spi1_enable_dma(void);

void game(void)
{
    print("Score  0");
    init_spi2();
    spi2_setup_dma();
    spi2_enable_dma();
    spi1_dma_display1("Hit key to play");
    spi1_dma_display2("Hit A/B to move");
    init_spi1();
    spi1_init_oled();
    spi1_setup_dma();
    spi1_enable_dma();
    init_tim17(); // start timer
    get_keypress(); // Wait for key to start
    spi1_dma_display1(">               ");
    spi1_dma_display2("                ");
    // Use the timer counter as random seed...
    srandom(TIM17->CNT);
    // Then enable interrupt...
    NVIC->ISER[0] = 1<<TIM17_IRQn;
    for(;;) {
        char key = get_keypress();
        if (key == 'A' || key == 'B') {
            // If the A or B key is pressed, disable interrupts while
            // we update the display.
            asm("cpsid i");
            if (key == 'A') {
                pos = 0;
                disp1[0] = '>';
                disp2[0] = ' ';
            } else {
                pos = 1;
                disp1[0] = ' ';
                disp2[0] = '>';
            }
            spi1_dma_display1(disp1);
            spi1_dma_display2(disp2);
            asm("cpsie i");
        }
    }
}

void initc() {
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
  //0-3 input: first 8 bits will be 00
  GPIOC->MODER &= ~ (0x000000FF); 
  GPIOC->PUPDR|= 0x000000AA;

  //4-9 as output: 
  GPIOC->MODER |= (0x00055500);
}

void initb() {
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
  GPIOB->MODER &= ~ (0x000003F3);
  GPIOB->PUPDR|= 0x000000A0;

  GPIOB->MODER |= (0x00550000); //the entire hex value is a bit mask

}

void init_systick() {
  // SysTick->LOAD = 374999; //scans it 16times per sec
  // SysTick->LOAD= 93750-1; //should scan it 64 times per sec. Very responsive.
  SysTick->LOAD= 187500-1; //should scan it 32 times per sec. Decently Responsive as well. Might be better since does not check systick interrupt as often as before. 
  SysTick->VAL = 0;
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
  SysTick->CTRL &=~ SysTick_CTRL_CLKSOURCE_Msk; //here we are setting the clksrc bit to 0 to get 6MHz.
}

volatile int current_col = 1;

void SysTick_Handler() {
    // 1. Read the row pins using GPIOC->IDR
    //    You can check the pins used for rows 
    //    of keypad in lab 1 manual
    // 2. If the var `current_col` corresponds to
    //    the row value, toggle one of the leds connected 
    //    to PB8-11.
    //    Basically the same we have done in lab 1
    // 3. Increment the `current_col` and wrap around
    //    to 1 if `current_col` > 4. So that next time
    //    we scan the next column
    // 4. Set the changed column pin designated by `current_col`
    //    to 1 and rest of the column pins to 0 to energized that
    //    particular column for next read of keypad.

    //If want to ignore when two buttons pressed together, GPIOC->IDR will have 2 1's. Therefore, if xor(GPIOC->IDR) is not 1. That means there are 2 1's.
      //However, if three are pressed then it will pass thru. 

    if(current_col == 3)
    {
      if(GPIOC->IDR & 0x1)
      {
        //# symbol. R4, C3
        /*
          This is where code for Confirm Place would go.
        */
        confirm();
        GPIOB->BRR = GPIO_BRR_BR_10;
        GPIOB->BRR = GPIO_BRR_BR_9;
        GPIOB->BRR = GPIO_BRR_BR_8;
        GPIOB->BSRR = GPIO_BSRR_BS_10;
      }
      else if(GPIOC->IDR & 0x4)
      {
        //6. R2, C3
        /*
          This is where code for Mv Right would go.
        */
        mv_right();
        GPIOB->BRR = GPIO_BRR_BR_10;
        GPIOB->BRR = GPIO_BRR_BR_9;
        GPIOB->BRR = GPIO_BRR_BR_8;
        GPIOB->BSRR = GPIO_BSRR_BS_9;
      }
      
    }
    if(current_col == 2)
    {
        //8 R3, C2
        /*
          This is where code for Mv Down would go.
        */
       mv_down();
      if(GPIOC->IDR &  0x2)
      {
        GPIOB->BRR = GPIO_BRR_BR_10;
        GPIOB->BRR = GPIO_BRR_BR_9;
        GPIOB->BRR = GPIO_BRR_BR_8;
        GPIOB->BSRR = GPIO_BSRR_BS_9;
        GPIOB->BSRR = GPIO_BSRR_BS_10;

      }
      else if(GPIOC->IDR & 0x4)
      {
        //5, R2, C2
        /*
          This is where code for Rotate would go.
        */
        mv_rot();
        GPIOB->BRR = GPIO_BRR_BR_10;
        GPIOB->BRR = GPIO_BRR_BR_9;
        GPIOB->BRR = GPIO_BRR_BR_8;
        GPIOB->BSRR = GPIO_BSRR_BS_8;

      }
      else if(GPIOC->IDR & 0x8)
      {
        //2, R1, C2
        /*
          This is where code for Mv Up would go.
        */
        mv_up();
        GPIOB->BRR = GPIO_BRR_BR_10;
        GPIOB->BRR = GPIO_BRR_BR_9;
        GPIOB->BRR = GPIO_BRR_BR_8;
        GPIOB->BSRR = GPIO_BSRR_BS_8;
        GPIOB->BSRR = GPIO_BSRR_BS_10;
      }
    }
    if(current_col == 1)
    {
      if(GPIOC->IDR & 0x4)
      {
        //4, R2, C1
        /*
          This is where code for Mv Left would go.
        */
        mv_left();
        GPIOB->BRR = GPIO_BRR_BR_10;
        GPIOB->BRR = GPIO_BRR_BR_9;
        GPIOB->BRR = GPIO_BRR_BR_8;
        GPIOB->BSRR = GPIO_BSRR_BS_8;
        GPIOB->BSRR = GPIO_BSRR_BS_9;
      }
      else if(GPIOC->IDR & 0x1)
      {
        //* symbol. R4, C1
        /*
          This is where code for Start would go.
        */
        start();
        GPIOB->BRR = GPIO_BRR_BR_10;
        GPIOB->BRR = GPIO_BRR_BR_9;
        GPIOB->BRR = GPIO_BRR_BR_8;
        GPIOB->BSRR = GPIO_BSRR_BS_8;
        GPIOB->BSRR = GPIO_BSRR_BS_9;
        GPIOB->BSRR = GPIO_BSRR_BS_10;
      }
    }

    current_col++;
    if(current_col > 4)
    {
      current_col = 1;
    }
    set_col(current_col);
}

void set_col(int col) {
      // Set PC4-7 (i.e. all columns) output to be 0
    // Set the column `col` output to be 1
    //  if col = 1, PC7 will be set to 1 as 
    //  it is connected to column 1 of the keypad 
    //  Likewise, if col = 4, PC4 will be set to 1

    GPIOC->ODR &=~ (GPIO_ODR_4 | GPIO_ODR_5| GPIO_ODR_6| GPIO_ODR_7) ;

    GPIOC->ODR |= (1<< (8-col));

}


//===========================================================================
// Main function
//===========================================================================

int main(void) {
    internal_clock();
    init_usart5();
    enable_tty_interrupt();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);

    initb();
    initc();
    init_systick();
 
    // GPIO enable
    enable_ports();
    // setup keyboard
    init_tim7();


    command_shell();

    // Uncomment when you are ready to generate a code.
    //autotest();

    // Game on!  The goal is to score 100 points.
    game();
}
