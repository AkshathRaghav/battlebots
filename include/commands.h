#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include "lcd.h"
#include "i2c.h"
#define SIZE 8 

typedef enum {
    COLOR_BLACK   = 0x0000,  // RGB(0, 0, 0)
    COLOR_RED     = 0xF900,  // RGB(23, 0, 0) - A darker red
    COLOR_WHITE   = 0xFFFF,  // RGB(31, 63, 31) - Keeping white unchanged
    COLOR_GREEN   = 0x07E0,  // RGB(0, 63, 0) - A more vibrant green
    COLOR_BLUE    = 0x001C,  // RGB(0, 0, 28) - A lighter blue
    COLOR_YELLOW  = 0xFFE0,  // RGB(31, 63, 0)
    COLOR_CYAN    = 0x07FF,  // RGB(0, 63, 31)
    COLOR_MAGENTA = 0xF81F,  // RGB(31, 0, 31)
    COLOR_ORANGE  = 0xFD20,  // RGB(31, 39, 0)
    COLOR_PURPLE  = 0x780F,  // RGB(15, 0, 15)
    COLOR_PINK    = 0xF81F,  // RGB(31, 0, 31)
    COLOR_BROWN   = 0x79E0,  // RGB(15, 31, 0)
    COLOR_GRAY    = 0x8410,  // RGB(16, 32, 16)
    COLOR_LIGHTGRAY = 0xC618  // RGB(25, 50, 25)
} Color;


struct commands_t {
    const char *cmd;
    void      (*fn)(int argc, char *argv[]);
};

extern int grid[SIZE][SIZE];
extern int cum_games_played; 

void led_on(int);
void led_wait(int);
void debug_draw(); 
void command_shell(void);
void LCD_Setup();
void LCD_StartScreen(); 
void LCD_EndScreen(); 
void LCD_DrawGrid(); 
void LCD_SetValidDots(int x1, int y1, int x2, int y2);
void LCD_DrawCoords(); 
void DrawShip(int x1_idx, int y1_idx, int x2_idx, int y2_idx, Color color);
void set_dot(int x, int y, Color color);
void LCD_WaitScreen();
void Game_Confirm_Cursor();
void Game_MvRight_Cursor();
void Game_MvDown_Cursor();
void Game_MvUp_Cursor();
void Game_MvLeft_Cursor(); 

#endif /* __COMMANDS_H_ */
