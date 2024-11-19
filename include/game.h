#include <stdio.h>
#include <stdlib.h>
#include "commands.h"

#define SIZE 8

typedef enum { 
    LOADING_SCREEN, 
    SET_SHIPS, 
    WAIT_SCREEN,
    BOMB_SHIPS, 
    END_SCREEN
} GameState; 

// Global Variables which will be there in the code. 
int orientation;
int ship_sizes[5];
extern int grid[SIZE][SIZE];
extern int hits[SIZE][SIZE];
int can_move;
int cursor_x, cursor_y; 
extern int valid_flag;
extern GameState state; 
extern int counter;
extern int init_flag; 
extern int coord_array[5][4];
extern int turn_flag;
extern int boomb_x; 
extern int boomb_y; 

extern int ship_hit_counter;

void Game_ReadBomb(); 

void Game_End_Screen(); 
void LCD_DrawShip(Color color);
void Game_MvRight(); 
void Game_MvLeft(); 
void Game_MvRot(); 
void Game_MvUp(); 
void Game_MvDown(); 
void start_up();
void printGrid(int grid[SIZE][SIZE]);
void check_mv_up();
void check_mv_down();
void check_mv_left();
void check_mv_right();
void check_mv_rot();
void check_overlap(int, int, int, int);
void check_bounds(int x1, int x2, int y1, int y2);
void Game_Start_Ships(); 
void Game_Confirm();
void Game_Reset();
void LCD_DrawCursor(Color color); 
void check_overlap_hits(int x, int y);
void Game_ReadBomb();
void Game_Confirm_Cursor();
void Game_MvRight_Cursor();
void Game_MvDown_Cursor();
void Game_MvUp_Cursor();
void Game_MvLeft_Cursor();
