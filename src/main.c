#include "main.h"

const char* team = "battlebots";
volatile int current_col = 1; 

// Interrupt handler 
// Main router interface on every interrupt-call
void SysTick_Handler() {
    // `state` tracks the part of the game logic we're in. 
    // coord_array stores the start and end of each ship. Used in SET_SHIPS mode. 

    if (counter >= len(ship_sizes)){
        state = BOMB_SHIPS; 
    }
    else{
        if(init_flag){
            check_overlap(orientation, coord_array[counter][1], coord_array[counter][3]);
            DrawShip(valid_flag ? COLOR_GREEN : COLOR_RED);
            init_flag = 0;
        }
    }

    if(current_col == 3)
    { 
      if(GPIOC_IDR & 0x1)
      {

        confirm();
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
        if (state == LOADING_SCREEN) { 
            LCD_Clear(COLOR_WHITE);
            LCD_DrawGrid(); 
            LCD_DrawCoords(); 
            state = SET_SHIPS; 
            Game_Start_Ships(); 
        } else { 
            LCD_Clear(COLOR_WHITE);
            LCD_StartScreen(); 
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
    init_usart5();
    init_usart1(); 

    enable_tty_interrupt();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);

    // Setup TFT
    LCD_Setup();
    LCD_StartScreen(); 
}



// we have a array which has 20 points. 4 * 5ships.
//say we do mv_right:
  // x1_new = x1 + 1 ; x2_new = x2++, y1_new = y1++; y2_new = y2++
  //verify with new coords. 
  //if can move
    //array[counter]: x1_new, x2_new, y1_new, y2_new
    //for the current ship we are controlling: LCD_Draw_Ship(array[counter]) : this would be current ship. For the other ships. Set_Dot(array[!counter]) * 4 times.

    
    //make it blank. Draw a white ship. at old_x1, old_x2, old_y1, old_y2

  //LCD_iterate_array ()
    //goes through the arrray of and plots the 5 ships (4points each).

//for the current ship we are controlling: LCD_Draw_Ship(array[counter]) : this would be current ship. For the other ships. Set_Dot(array[!counter]) * 4 times.

//array = [[x1, y1, x2, y2], [x1, y1, x2, y2], [x1, y1, x2, y2], [x1, y1, x2, y2], [x1, y1, x2, y2]]

//counter goes from 0,1,2,3,4
  //if counter is 0
    // LCD_Draw_Ship(array[counter]) : with the corresponding valid_flag based color
    //Set_Dot(array[counter + 1], white)
     //Set_Dot(array[counter + 2], white)
      //Set_Dot(array[counter + 3], white)

  //if counter is 1
    //Set_Dot(array[counter - 1], SAVED_COLOR = GREEN)
    // LCD_Draw_Ship(array[counter]) : with the corresponding valid_flag based color
     //Set_Dot(array[counter + 2], white)
      //Set_Dot(array[counter + 3], white)

    


    