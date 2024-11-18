#include "main.h"

const char* team = "battlebots";
volatile int current_col = 1; 

// Interrupt handler 
// Main router interface on every interrupt-call
void SysTick_Handler() {

    // `state` tracks the part of the game logic we're in. 
    // coord_array stores the start and end of each ship. Used in SET_SHIPS mode. 

    if (counter >= 5) {
      state = BOMB_SHIPS; 
    }
    else{
      if (init_flag) {
        check_overlap(coord_array[counter][1], coord_array[counter][3], coord_array[counter][0], coord_array[counter][2]);
        LCD_DrawShip((valid_flag ? COLOR_GREEN : COLOR_RED));
        init_flag = 0;
      }
    }

    if (current_col == 3) { 
      if(GPIOC_IDR & 0x1) Game_Confirm(); // # 
      else if(GPIOC_IDR & 0x4) Game_MvRight(); // 6 
    }

    if (current_col == 2) {
      if (GPIOC_IDR & 0x2) Game_MvDown(); // 8
      else if (GPIOC_IDR & 0x4) Game_MvRot(); // 5
      else if (GPIOC_IDR & 0x8) Game_MvUp(); // 2
    }

    if (current_col == 1) {
      if(GPIOC_IDR & 0x4){
        Game_MvLeft(); // 4
      }
      else if(GPIOC->IDR & 0x1) { // * 
        if (state == LOADING_SCREEN) { 
            LCD_Clear(COLOR_WHITE);
            // LCD_DrawGrid(); 
            LCD_DrawCoords(); 
            state = SET_SHIPS; 
            Game_Start_Ships(); 
        } else { 
            LCD_Clear(COLOR_WHITE);
            LCD_StartScreen(); 
            Game_Reset();
            state = LOADING_SCREEN; 
        }
      }
    }

    current_col++;
    if (current_col > 4) current_col = 1; 
    set_col(current_col);
}

int main() {
    internal_clock();

    // SysTick + Keyboard 
    initb();
    initc();
    inita(); 
    init_systick();

    // Terminal
    init_usart1(); 

    // Setup TFT
    LCD_Setup();
    LCD_StartScreen(); 

    // set_dot(0, 0, COLOR_GREEN);
    // set_dot(4, 0, COLOR_WHITE);
    // set_dot(0, 4, COLOR_RED);
}
   