/**
  ******************************************************************************
  * @file    main.c
  * @author  Weili An, Niraj Menon
  * @date    Jan 15, 2024
  * @brief   ECE 362 Lab 2 Student template
  ******************************************************************************
*/


/**
******************************************************************************/

// Fill out your username, otherwise your completion code will have the 
// wrong username!
const char* username = "bhide1";

/******************************************************************************
*/ 

#include "stm32f0xx.h"
#include <stdint.h>
#include <mvmnt_verification_blocks.c>
void initc();
void initb();
void togglexn(GPIO_TypeDef *port, int n);
void init_exti();
void set_col(int col);
void SysTick_Handler();
void init_systick();
void adjust_priorities();

extern void autotest();
extern void internal_clock();
extern void nano_wait(int);

int main(void) {
    internal_clock();
    // Uncomment when most things are working
     //autotest();
    
    initb();
    initc();
    //init_exti();
    init_systick();
    //adjust_priorities();

    // Slowly blinking
    // for(;;) {
    //     togglexn(GPIOC, 9);
    //     nano_wait(500000000);
    // }
}

/**
 * @brief Init GPIO port C
 *        PC0-PC3 as input pins with the pull down resistor enabled
 *        PC4-PC9 as output pins
 * 
 */
void initc() {
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
  //0-3 input: first 8 bits will be 00
  GPIOC->MODER &= ~ (0x000000FF); 
  GPIOC->PUPDR|= 0x000000AA;

  //4-9 as output: 
  GPIOC->MODER |= (0x00055500);
}

/**
 * @brief Init GPIO port B
 *        PB0, PB2, PB3, PB4 as input pins
 *          enable pull down resistor on PB2 and PB3
 *        PB8-PB11 as output pins
 * 
 */
void initb() {
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
  GPIOB->MODER &= ~ (0x000003F3);
  GPIOB->PUPDR|= 0x000000A0;

  GPIOB->MODER |= (0x00550000); //the entire hex value is a bit mask

}


/**
 * @brief Change the ODR value from 0 to 1 or 1 to 0 for a specified 
 *        pin of a port.
 * 
 * @param port : The passed in GPIO Port
 * @param n    : The pin number
 */
// void togglexn(GPIO_TypeDef *port, int n) {
//   // port->ODR &=~ (0xFFFFFFFF); //clears everything
//   port->ODR |= (1<<n); //turns on that led.
// }

/**
 * @brief Enable the SysTick interrupt to occur every 1/16 seconds.
 * 
 */
void init_systick() {
  // SysTick->LOAD = 374999; //scans it 16times per sec
  // SysTick->LOAD= 93750-1; //should scan it 64 times per sec. Very responsive.
  SysTick->LOAD= 187500-1; //should scan it 32 times per sec. Decently Responsive as well. Might be better since does not check systick interrupt as often as before. 
  SysTick->VAL = 0;
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
  SysTick->CTRL &=~ SysTick_CTRL_CLKSOURCE_Msk; //here we are setting the clksrc bit to 0 to get 6MHz.
}

volatile int current_col = 1;

/**
 * @brief The ISR for the SysTick interrupt.
 * 
 */
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

/**
 * @brief For the keypad pins, 
 *        Set the specified column level to logic "high".
 *        Set the other three three columns to logic "low".
 * 
 * @param col 
 */
void set_col(int col) {
      // Set PC4-7 (i.e. all columns) output to be 0
    // Set the column `col` output to be 1
    //  if col = 1, PC7 will be set to 1 as 
    //  it is connected to column 1 of the keypad 
    //  Likewise, if col = 4, PC4 will be set to 1

    GPIOC->ODR &=~ (GPIO_ODR_4 | GPIO_ODR_5| GPIO_ODR_6| GPIO_ODR_7) ;

    GPIOC->ODR |= (1<< (8-col));

}

