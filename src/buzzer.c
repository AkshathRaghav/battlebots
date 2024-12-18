// #include "stm32f0xx.h"

// void autotest(void);

// int count= 0;

// void setup_tim3(void) {
//     RCC -> AHBENR |= RCC_AHBENR_GPIOCEN;
//     RCC -> APB1ENR |= RCC_APB1ENR_TIM3EN;
//     GPIOC -> MODER &= ~0xC0000; // clearing the bits for PC9
//     GPIOC -> MODER |= 0x80000; // setup alternate function mode for GPIOC pins 9
//     GPIOC -> AFR[1] &= ~(GPIO_AFRH_AFRH1);
    
//     TIM3 -> PSC = 47999;
//     TIM3 -> ARR = 999;

//     TIM3 -> CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1;
//     TIM3 -> CCMR2 &= ~ TIM_CCMR2_OC3M_0;

//     TIM3 -> CCMR2 |= TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1;
//     TIM3 -> CCMR2 &= ~ TIM_CCMR2_OC4M_0;
//     TIM3 -> CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E; // Configure TIM3 for PWM mode 1 so that each channel output can have a CCR value from 0 to 1000. 
//     TIM3 -> CR1 = TIM_CR1_CEN; // start clock

//    TIM3 -> CCR4 = 1000; // 200
    
// }   

// void enable_ports(void) {
//     RCC -> AHBENR |= RCC_AHBENR_GPIOBEN; // enable RCC clock for GPIOB OK
//     RCC -> AHBENR |= RCC_AHBENR_GPIOCEN; // enable RCC clock for GPIOC OK
//     // RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;
//     // GPIOB -> MODER |= 0x155555; //Configures pins PB0 – PB10 to be outputs OK
//     // GPIOC -> MODER |= 0x5500; // Configures pins PC4 – PC7 to be outputs OK
//     // GPIOC -> OTYPER |= 0xF0; // Configures pins PC4 – PC7 to have output type open-drain (using the OTYPER registers)
//     // GPIOC -> MODER &= ~0xFF;// PC0-PC3 as input pins OK
//     // GPIOC -> PUPDR |= 0x55; // Configures pins PC0 – PC3 to be internally pulled high


//     // FOR THE PROJECT, SETTING PC9 FOR AUDIO  
//     GPIOC -> MODER &=~ GPIO_MODER_MODER9;
//     GPIOC -> MODER |= GPIO_MODER_MODER9_0;
// }

// void init_tim15(void) {
//     RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
//     TIM15 -> DIER |= TIM_DIER_UDE;
//     TIM15 -> DIER &= ~TIM_DIER_UIE;
//     TIM15 -> CR1 = TIM_CR1_CEN;
//     TIM15 -> ARR = 23;
//     TIM15 -> PSC = 1999;
// }

// uint8_t col; // the column being scanned

// void delay_ms(uint32_t ms){
//     //config tim 6 for delay
//     RCC -> APB2ENR |= RCC_APB2ENR_TIM15EN;
//     TIM15 -> PSC = 47;
//     TIM15 -> ARR = ms - 1;
//     TIM15 -> DIER |= TIM_DIER_UIE;
//     TIM15 -> CR1 |= TIM_CR1_CEN;

//     NVIC->ISER[0] |= 1 << TIM15_IRQn;

// }


// void TIM15_IRQHandler(){
//     TIM15->SR &= ~TIM_SR_UIF;

//     count++;

//     if(count > 10){
//         TIM3->CCR4 = 0;
//         count = 0;
//     }
// }

// int main(void) {
//     internal_clock();

// enable_ports();
// setup_tim3();
// delay_ms(100000000000000000000);

// }