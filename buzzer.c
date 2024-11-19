#include "stm32f0xx.h"

// void nano_wait(int);

// void setup_tim14_for_5s(void);
// void TIM14_IRQHandler(void);
// void activate_pwm_with_timer(void);


// 16-bits per digit.
// The most significant 8 bits are the digit number.
// The least significant 8 bits are the segments to illuminate.
// uint16_t msg[8] = { 0x0000,0x0100,0x0200,0x0300,0x0400,0x0500,0x0600,0x0700 };
// extern const char font[];
// Print an 8-character string on the 8 digits
// void print(const char str[]);
// Print a floating-point value.
// void printfloat(float f);

void autotest(void);

int count= 0;

//============================================================================
// PWM Lab Functions
//============================================================================
void setup_tim3(void) {
    RCC -> AHBENR |= RCC_AHBENR_GPIOCEN;
    RCC -> APB1ENR |= RCC_APB1ENR_TIM3EN;
    GPIOC -> MODER &= ~0xC0000; // clearing the bits for PC9
    GPIOC -> MODER |= 0x80000; // setup alternate function mode for GPIOC pins 9
    GPIOC -> AFR[1] &= ~(GPIO_AFRH_AFRH1);
    
    TIM3 -> PSC = 47999;
    TIM3 -> ARR = 999;
    // TIM3 -> CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
    // TIM3 -> CCMR1 &= ~ TIM_CCMR1_OC1M_0;
    // TIM3 -> CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;
    // TIM3 -> CCMR1 &= ~ TIM_CCMR1_OC2M_0;

    TIM3 -> CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1;
    TIM3 -> CCMR2 &= ~ TIM_CCMR2_OC3M_0;

    TIM3 -> CCMR2 |= TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1;
    TIM3 -> CCMR2 &= ~ TIM_CCMR2_OC4M_0;
    TIM3 -> CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E; // Configure TIM3 for PWM mode 1 so that each channel output can have a CCR value from 0 to 1000. 
    TIM3 -> CR1 = TIM_CR1_CEN; // start clock
    // TIM3 -> CCR1 = 800; //800
    // TIM3 -> CCR2 = 600; //600
    // TIM3 -> CCR3 = 400; //400

   TIM3 -> CCR4 = 1000; // 200
    
}   

// void setup_tim1(void) {
//     // Generally the steps are similar to those in setup_tim3
//     // except we will need to set the MOE bit in BDTR. 
//     // Be sure to do so ONLY after enabling the RCC clock to TIM1.
//     RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;
//     RCC -> APB2ENR |= RCC_APB2ENR_TIM1EN;
//     GPIOA -> MODER |= 0xAA0000; // setup alternate function mode for GPIOA pins 8 to 11
//     GPIOA -> AFR[1] &= ~0xffff;
//     GPIOA -> AFR[1] |= 0x2222;
//     TIM1 -> BDTR |= 1 << 15; // enable the MOE bit
//     TIM1 -> PSC = 0;
//     TIM1 -> ARR = 2399; 
//     // Configure the timer for PWM mode: 
//     TIM1 -> CR1 = TIM_CR1_CEN; // Enable Timer 1

//     TIM1 -> CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
//     TIM1 -> CCMR1 &= ~ TIM_CCMR1_OC1M_0;
//     TIM1 -> CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;
//     TIM1 -> CCMR1 &= ~ TIM_CCMR1_OC2M_0;
//     TIM1 -> CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1;
//     TIM1 -> CCMR2 &= ~ TIM_CCMR2_OC3M_0;
//     TIM1 -> CCMR2 |= TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1;
//     TIM1 -> CCMR2 &= ~ TIM_CCMR2_OC4M_0;

//     TIM1 -> CCMR2 |= TIM_CCMR2_OC4PE;

//     TIM1 -> CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;

//     TIM1 -> CR1 = TIM_CR1_CEN; // start clock
// }

// int getrgb(void);

// // Helper function for you
// // Accept a byte in BCD format and convert it to decimal
// uint8_t bcd2dec(uint8_t bcd) {
//     // Lower digit
//     uint8_t dec = bcd & 0xF;

//     // Higher digit
//     dec += 10 * (bcd >> 4);
//     return dec;
// }

// void setrgb(int rgb) {
//     uint8_t b = bcd2dec(rgb & 0xFF);
//     uint8_t g = bcd2dec((rgb >> 8) & 0xFF);
//     uint8_t r = bcd2dec((rgb >> 16) & 0xFF);

//     // TODO: Assign values to TIM1->CCRx registers
//     // Remember these are all percentages
//     // Also, LEDs are on when the corresponding PWM output is low
//     // so you might want to invert the numbers.
//     TIM1 -> CCR1 = (100 - r) * 0.01 * 2400;
//     TIM1 -> CCR2 = (100 - g) * 0.01 * 2400;
//     TIM1 -> CCR3 = (100 - b) * 0.01 * 2400;
// }



//============================================================================
// Lab 4 code
// Add in your functions from previous lab
//============================================================================

// Part 3: Analog-to-digital conversion for a volume level.
// int volume = 2400;

// // Variables for boxcar averaging.
// #define BCSIZE 32
// int bcsum = 0;
// int boxcar[BCSIZE];
// int bcn = 0;

// void dialer(void);

// // Parameters for the wavetable size and expected synthesis rate.
// #define N 1000
// #define RATE 20000
// short int wavetable[N];
// int step0 = 0;
// int offset0 = 0;
// int step1 = 0;
// int offset1 = 0;

//============================================================================
// enable_ports()
//============================================================================
void enable_ports(void) {
    RCC -> AHBENR |= RCC_AHBENR_GPIOBEN; // enable RCC clock for GPIOB OK
    RCC -> AHBENR |= RCC_AHBENR_GPIOCEN; // enable RCC clock for GPIOC OK
    RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOB -> MODER |= 0x155555; //Configures pins PB0 – PB10 to be outputs OK
    GPIOC -> MODER |= 0x5500; // Configures pins PC4 – PC7 to be outputs OK
    GPIOC -> OTYPER |= 0xF0; // Configures pins PC4 – PC7 to have output type open-drain (using the OTYPER registers)
    GPIOC -> MODER &= ~0xFF;// PC0-PC3 as input pins OK
    GPIOC -> PUPDR |= 0x55; // Configures pins PC0 – PC3 to be internally pulled high


    // FOR THE PROJECT, SETTING PC9 FOR AUDIO  
    GPIOC -> MODER &=~ GPIO_MODER_MODER9;
    GPIOC -> MODER |= GPIO_MODER_MODER9_0;
    GPIOC -> ODR |= GPIO_ODR_9;
}

//============================================================================
// setup_dma() + enable_dma()
//============================================================================
// void setup_dma(void) {
//     RCC->AHBENR |= RCC_AHBENR_DMA1EN; // Enable the clock for DMA1
    
//     DMA1_Channel5 ->CCR &= ~DMA_CCR_EN; // Turn off the enable bit for the channel first, like with every other peripheral.
   
//     DMA1_Channel5 -> CMAR =(uint32_t) &msg;
//     DMA1_Channel5 -> CPAR = (uint32_t) &(GPIOB -> ODR);

//     DMA1_Channel5 -> CNDTR = 8;

//     DMA1_Channel5 -> CCR |= DMA_CCR_DIR;

//     DMA1_Channel5 -> CCR |= DMA_CCR_MINC; 

//     DMA1_Channel5->CCR &= ~DMA_CCR_MSIZE;   
//     DMA1_Channel5->CCR |= DMA_CCR_MSIZE_0;  

//     DMA1_Channel5->CCR &= ~DMA_CCR_PSIZE;   
//     DMA1_Channel5->CCR |= DMA_CCR_PSIZE_0; 

//     DMA1_Channel5 -> CCR |= DMA_CCR_CIRC; 
// }

// void enable_dma(void) {
//     DMA1_Channel5->CCR |= DMA_CCR_EN; // Enable DMA
// }

//============================================================================
// init_tim15()
//============================================================================
void init_tim15(void) {
    RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
    TIM15 -> DIER |= TIM_DIER_UDE;
    TIM15 -> DIER &= ~TIM_DIER_UIE;
    TIM15 -> CR1 = TIM_CR1_CEN;
    TIM15 -> ARR = 23;
    TIM15 -> PSC = 1999;
}

//=============================================================================
// Part 2: Debounced keypad scanning.
//=============================================================================

uint8_t col; // the column being scanned

// void drive_column(int);   // energize one of the column outputs
// int  read_rows();         // read the four row inputs
// void update_history(int col, int rows); // record the buttons of the driven column
// char get_key_event(void); // wait for a button event (press or release)
// char get_keypress(void);  // wait for only a button press event.
// float getfloat(void);     // read a floating-point number from keypad
// void show_keys(void);     // demonstrate get_key_event()

//============================================================================
// The Timer 7 ISR
//============================================================================
// void TIM7_IRQHandler(){
//     TIM7 -> SR &= ~TIM_SR_UIF;
//     // Remember to acknowledge the interrupt here!
//     int rows = read_rows();
//     update_history(col, rows);
//     col = (col + 1) & 3;
//     drive_column(col);
// }

// //============================================================================
// // init_tim7()
// //============================================================================
// void init_tim7(void) {
//     RCC -> APB1ENR |= RCC_APB1ENR_TIM7EN;
//     TIM7 -> ARR = 95;
//     TIM7 -> PSC = 499;
//     TIM7->DIER |= TIM_DIER_UIE;
//     NVIC -> ISER[0] |= 1 << 18;
//     TIM7 -> CR1 = TIM_CR1_CEN;
// }

//============================================================================
// setup_adc()
//============================================================================
// void setup_adc(void) {
//     RCC -> AHBENR |= RCC_AHBENR_GPIOAEN; // enable RCC clock for GPIOA
//     GPIOA -> MODER |= 0xC; // Configure the pin associated with ADC_IN1 to be in analog mode (PA1)
//     RCC -> APB2ENR |= RCC_APB2ENR_ADC1EN; // Enable the clock to the ADC peripheral.
//     RCC -> CR2 |= RCC_CR2_HSI14ON; //Turn on the "high-speed internal" 14 MHz clock (HSI14).
//     while(!(RCC -> CR2 & RCC_CR2_HSI14RDY));// Wait for the 14 MHz clock to be ready.
//     ADC1 -> CR |= ADC_CR_ADEN; // Enable the ADC by setting the ADEN bit in the CR register.
//     while((ADC1 -> CR & ADC_CR_ADSTART)); // Wait for the ADC to be ready.
//     ADC1 -> CHSELR = 0; 
//     ADC1 -> CHSELR |= 1 << 1;// Select the corresponding channel for ADC_IN1 in the CHSELR.
//     while(!(ADC1 -> ISR & ADC_ISR_ADRDY)); // Wait for the ADC to be ready
// }


//============================================================================
// Timer 2 ISR
//============================================================================
// Write the Timer 2 ISR here.  Be sure to give it the right name.
// void TIM2_IRQHandler(){
//     TIM2 -> SR &= ~TIM_SR_UIF; // acknowledge the interrupt
//     ADC1 -> CR |= 0x4; // Start the ADC by turning on the ADSTART bit in the CR.
//     for(int x=0; x<10000; x++); // USELESS CODE
//     while(!(ADC1->ISR & ADC_ISR_EOC)); // Wait until the EOC bit is set in the ISR.
//     bcsum -= boxcar[bcn];
//     bcsum += boxcar[bcn] = ADC1->DR;
//     bcn += 1;
//     if (bcn >= BCSIZE)
//         bcn = 0;
//     volume = bcsum / BCSIZE;
// }

//============================================================================
// init_tim2()
//============================================================================
// void init_tim2(void) {
//     RCC -> APB1ENR |= RCC_APB1ENR_TIM2EN;
//     TIM2 -> ARR = 1999;
//     TIM2 -> PSC = 2399;
//     TIM2->DIER |= TIM_DIER_UIE;
//     NVIC -> ISER[0] = 1 << TIM2_IRQn; // enable interrupt bit
//     TIM2 -> CR1 = TIM_CR1_CEN; // start clock
// }


//============================================================================
// setup_dac()
//============================================================================
// void setup_dac(void) {
//     RCC -> AHBENR |= RCC_AHBENR_GPIOAEN; // Enable the clock to GPIO Port A (reenabling is safe)
//     GPIOA->MODER |= 0x300; // Set the configuration for analog operation only for the pin that is associated with DAC_OUT1. 
//     RCC -> APB1ENR |= RCC_APB1ENR_DACEN; // Enable the RCC clock for the DAC
//     DAC -> CR &= ~DAC_CR_TSEL1; // Select a TIM6 TRGO trigger for the DAC with the TSEL field of the CR register. 
//     DAC -> CR |= DAC_CR_TEN1; // enable trigger for the DAC
//     DAC -> CR |= DAC_CR_EN1; // Enable the DAC
// }

//============================================================================
// Timer 6 ISR
//============================================================================
// Write the Timer 6 ISR here.  Be sure to give it the right name.
// void TIM6_DAC_IRQHandler(){
//     TIM6 -> SR &= ~TIM_SR_UIF; // acknowledge the TIM6 interrupt first
//     // offset0 += step0;
//     // offset1 += step1;
//     // if(offset0 >= (N << 16)){
//     //     offset0 -= (N << 16);
//     // }
//     // if(offset1 >= (N << 16)){
//     //     offset1 -= (N << 16);
//     // }

//     // int samp = wavetable[offset0>>16] + wavetable[offset1>>16];
//     // // samp *= volume;
//     // // samp = samp >> 17;
//     // // samp += 2048;
//     // samp = ((samp * volume)>>18) + 1200;

//     // // DAC ->DHR12R1 = samp;
//     // // DAC -> TIM1_CCR4 = samp;
//     // TIM1 -> CCR4 = samp;
//     TIM3 -> CR1 &= ~ TIM_CR1_CEN;
// }

//============================================================================
// init_tim6()
//============================================================================
// void init_tim6(void) {
//     RCC -> APB1ENR |= RCC_APB1ENR_TIM6EN;
//     TIM6 -> ARR = (48000000/RATE)/60 - 1;
//     TIM6 -> PSC = 59;
//     TIM6 -> DIER |= TIM_DIER_UIE;
//     NVIC -> ISER[0] = 1 << TIM6_IRQn; // enable interrupt bit
//     TIM6 -> CR1 = TIM_CR1_CEN; // start clock
//     // TIM6 -> CR2 |= TIM_CR2_MMS_1; // Configure to trigger the DAC, set the value that enables a TRGO on update event
// }

//===========================================================================
// init_wavetable()
// Write the pattern for a complete cycle of a sine wave into the
// wavetable[] array.
//===========================================================================
// void init_wavetable(void) {
//     for(int i=0; i < N; i++)
//         wavetable[i] = 32767 * sin(2 * M_PI * i / N);
// }

//============================================================================
// set_freq()
//============================================================================
// void set_freq(int chan, float f) {
//     if (chan == 0) {
//         if (f == 0.0) {
//             step0 = 0;
//             offset0 = 0;
//         } else
//             step0 = (f * N / RATE) * (1<<16);
//     }
//     if (chan == 1) {
//         if (f == 0.0) {
//             step1 = 0;
//             offset1 = 0;
//         } else
//             step1 = (f * N / RATE) * (1<<16);
//     }
// }


// void activate_pwm_with_timer(void) {
//     TIM3->CCR4 = 100;                     // Set duty cycle (e.g., 50%)
//     TIM3->CR1 = TIM_CR1_CEN;             // Start TIM3
//     setup_tim14_for_5s();               // Start TIM14 to count 5 seconds
// }
// void setup_tim14_for_5s(void) {
//     RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;  // Enable TIM14 clock
//     TIM14->PSC = 47999;                   // Prescaler for 1 kHz timer clock
//     TIM14->ARR = 5000 - 1;                // Auto-reload for 5 seconds
//     TIM14->DIER |= TIM_DIER_UIE;          // Enable update interrupt
//     TIM14->CR1 |= TIM_CR1_CEN;            // Start TIM14
//     NVIC_EnableIRQ(TIM14_IRQn);           // Enable TIM14 interrupt in NVIC
// }
// void TIM14_IRQHandler(void) {
//     if (TIM14->SR & TIM_SR_UIF) {         // Check update interrupt flag
//         TIM14->SR &= ~TIM_SR_UIF;         // Clear the interrupt flag
//         TIM14->CR1 &= ~TIM_CR1_CEN;       // Stop Timer 14
//         TIM3->CCR4 = 0;                   // Turn off the buzzer (set duty cycle to 0)
//     }
// }

void delay_ms(uint32_t ms){
    //config tim 6 for delay
    RCC -> APB2ENR |= RCC_APB2ENR_TIM15EN;
    TIM15 -> PSC = 47;
    TIM15 -> ARR = ms - 1;
    TIM15 -> DIER |= TIM_DIER_UIE;
    TIM15 -> CR1 |= TIM_CR1_CEN;

    NVIC->ISER[0] |= 1 << TIM15_IRQn;

}


void TIM15_IRQHandler(){
    TIM15->SR &= ~TIM_SR_UIF;

    count++;

    if(count > 10){
        TIM3->CCR4 = 0;
        count = 0;
    }
}

// More robust TIM14 Irq Handler
// void TIM14_IRQHandler(void) {
//     if (TIM14->SR & TIM_SR_UIF) {         // Check update interrupt flag
//         TIM14->SR &= ~TIM_SR_UIF;         // Clear interrupt flag
//         TIM3->CR1 &= ~TIM_CR1_CEN;        // Stop TIM3
//         TIM3->CCR4 = 0;                   // Clear duty cycle
//         TIM3->CCER &= ~TIM_CCER_CC4E;     // Disable TIM3 output
//         TIM14->CR1 &= ~TIM_CR1_CEN;       // Stop TIM14
//         TIM14->SR = 0;                    // Clear all TIM14 flags
//     }
// }



//============================================================================
// All the things you need to test your subroutines.
//============================================================================
int main(void) {
    internal_clock();

    // Uncomment autotest to get the confirmation code.
    // autotest();

    // Demonstrate part 1
// #define TEST_TIMER3

// #ifdef TEST_TIMER3
//     setup_tim3();
     
//     for(;;) { }
// #endif
// enable_ports();
// setup_tim14_for_5s();

enable_ports();

// if 
setup_tim3();
delay_ms(100000000000000000000);
// int pwm_activated = 0;


//   // Configure PWM for the buzzer
//     setup_tim14_for_5s();  // Setup 5s timer functionality (if not redundant)
//         setup_tim3();
//         int pwm_activated = 0; // State variable to track PWM activation
//    while (1) {
//         if (!pwm_activated) {  // Ensure PWM is activated only once
//             activate_pwm_with_timer();
//             pwm_activated = 1; // Set flag to indicate PWM has been activated
//         }

    // Initialize the display to something interesting to get started.
    // msg[0] |= font['E'];
    // msg[1] |= font['C'];
    // msg[2] |= font['E'];
    // msg[3] |= font[' '];
    // msg[4] |= font['3'];
    // msg[5] |= font['6'];
    // msg[6] |= font['2'];
    // msg[7] |= font[' '];

    
    // setup_dma();
    // enable_dma();
    // init_tim15();
    // init_tim7();
    // setup_adc();
    // init_tim2();
    // init_wavetable();
    // // init_tim6();

    // setup_tim1();
   
    // demonstrate part 2

    // Have fun.
    // dialer();

}
