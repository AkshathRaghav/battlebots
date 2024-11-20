#include "main.h"

#include "i2c.c"

const char* team = "battlebots";
volatile int current_col = 1; 
int done_first = 1;

// Interrupt handler 
// Main router interface on every interrupt-call
void SysTick_Handler() {
    if (state == LOADING_SCREEN) { 
        if ((current_col == 1) & (GPIOC_IDR & 0x1)) { // * 
        LCD_Clear(COLOR_WHITE);
        LCD_DrawCoords(); 
        state = SET_SHIPS; 
        Game_Start_Ships(); 
      }
    }

    else if (state == SET_SHIPS) {
      if (counter >= 5) {
          nano_wait(1200);
          state = WAIT_SCREEN; 
          LCD_Clear(COLOR_WHITE);
          LCD_WaitScreen(); 
      }
      else {
          if (init_flag) {
            check_overlap(coord_array[counter][1], coord_array[counter][3], coord_array[counter][0], coord_array[counter][2]);
            LCD_DrawShip((valid_flag ? COLOR_GREEN : COLOR_RED));
            init_flag = 0;
          }

        // Keypad 
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
          if (GPIOC_IDR & 0x4){
            Game_MvLeft(); // 4
          } else if (GPIOC_IDR & 0x1) { // * 
            LCD_Clear(COLOR_WHITE);
            LCD_StartScreen(); 
            Game_Reset();
            state = LOADING_SCREEN; 
          }
        }
      }
    }

    else if (state == WAIT_SCREEN) { 
      init_flag = 1; 

      COMM_SendData(255);
      if (COMM_ReadData() == (uint8_t)255) { 
        turn_flag = done_first; 
        state = BOMB_SHIPS; 
        LCD_Clear(COLOR_WHITE);
        LCD_DrawCoords();
      }  
    }

    else if (state == BOMB_SHIPS) { 
      if (init_flag) { 
        valid_flag = 1; 
        ship_hit_counter = 0; 
        check_overlap_hits(cursor_x, cursor_y);
        LCD_DrawCursor(valid_flag ? COLOR_GREEN : COLOR_RED);
        init_flag = 0; 
      }

      if (turn_flag) {
        led_on(1);
        led_wait(0);
        LCD_DrawCursor(valid_flag ? COLOR_GREEN : COLOR_RED);
        if (ship_hit_counter >= 3){
          COMM_SendData(69);  
          state = END_SCREEN; 
          ship_hit_counter = 0; 
        }
        else{
          if (current_col == 3) {
            if (GPIOC_IDR & 0x1) Game_Confirm_Cursor(); // #
            else if (GPIOC_IDR & 0x4) Game_MvRight_Cursor(); // 6
          }
          else if (current_col == 2) {
              if(GPIOC_IDR &  0x2) Game_MvDown_Cursor(); // 8
              else if(GPIOC_IDR & 0x8) Game_MvUp_Cursor(); // 2
          }
          else if (current_col == 1) {
            if(GPIOC_IDR & 0x4) Game_MvLeft_Cursor(); // 4
          }
        }
      } else { 
        led_on(0); 
        led_wait(1);
        LCD_DrawCursor(COLOR_WHITE);
        Game_ReadBomb();
        turn_flag = 1; 
      }
    }

    else if (state == END_SCREEN){
      if (!init_flag){
        LCD_Clear(COLOR_WHITE);
        LCD_EndScreen();
        // ATHARVA: Update it here.  

        int num_games = Num_Games_Updater();
        init_flag = 1; 
      } 

      if ((current_col == 1) && (GPIOC_IDR & 0x1)) {
        state = LOADING_SCREEN; 
        LCD_Clear(COLOR_WHITE); 
        LCD_StartScreen(); 
        Game_Reset(); 
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
}
   