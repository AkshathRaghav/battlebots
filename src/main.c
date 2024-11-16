#include <stdint.h>
#include <stdio.h>
#include "setup.h"
#include "commands.h"
#include "game.h"

const char* team = "battlebots";
volatile int current_col = 1; 

void mv_right(); 
void mv_left(); 
void mv_rot(); 
void mv_up(); 
void mv_down(); 
void start_up();

// Routes logic to game
void SysTick_Handler() {
    if(counter >= sizeof(ship_sizes)/sizeof(ship_sizes[0])){ // done placing ships - move on 
        state = 1; // after done with setting ships, bomb ships state starts
    }
    else{
        if(init_flag){
            init_ship();
            LCD_DrawShip(x1, y1, x2, y2, (valid_flag ? COLOR_GREEN : COLOR_RED));
            init_flag = 0;
        }
    }

    if(current_col == 3)
    {
      if(GPIOC_IDR & 0x1)
      {
        // # symbol. R4, C3

      }
      else if(GPIOC_IDR & 0x4)
      {
        //6. R2, C3
        mv_right();
      }
      
    }
    if(current_col == 2)
    {
      if(GPIOC_IDR & 0x2)
      {
        //8 R3, C2
        mv_down();
      }
      else if(GPIOC_IDR & 0x4)
      {
        //5, R2, C2
        mv_rot();
      }
      else if(GPIOC_IDR & 0x8)
      {
        //2, R1, C2
        mv_up();
      }
    }
    if(current_col == 1)
    {
      if(GPIOC_IDR & 0x4)
      {
        //4, R2, C1
        mv_left();
      }
      else if(GPIOC->IDR & 0x1) { 
        // *, Start Logic 
        if (state == -1) { 
            LCD_Clear(1111);
            LCD_DrawGrid(); 
            LCD_DrawCoords(); 
            state = 0; // Starts to Game Logic 
            
            Game_Start(); 

            // Draw Ships
        } else { 
            LCD_Clear(1111);
            LCD_StartScreen(); 
            state = -1; 
        }
      }
    }

    current_col++;
    if(current_col > 4)
    {
      current_col = 1;
    }
    set_col(current_col);
}

int main() {
    internal_clock();

    // SysTick + Keyboard 
    initb();
    initc();
    init_systick();

    // Terminal
    init_usart5();
    enable_tty_interrupt();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);

    // Setup TFT
    LCD_Setup();
    LCD_StartScreen(); 
}

