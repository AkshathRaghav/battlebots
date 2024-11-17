#include <stdio.h>
#include <stdlib.h>
#include "commands.h"

#define SIZE 8

typedef enum { 
    LOADING_SCREEN, 
    SET_SHIPS, 
    BOMB_SHIPS, 
    END_SCREEN
} GameState; 

// Global Variables which will be there in the code. 
int orientation;
int ship_sizes[5];
int grid[SIZE][SIZE];
int can_move;
int x_bomb, y_bomb; 
extern int valid_flag;
extern GameState state; 
extern int counter;
extern int init_flag; 
extern int coord_array[5][4];

void mv_right(); 
void mv_left(); 
void mv_rot(); 
void mv_up(); 
void mv_down(); 
void start_up();
void printGrid(int grid[SIZE][SIZE]);
void check_mv_up();
void check_mv_down();
void check_mv_left();
void check_mv_right();
void check_mv_rot();
void check_overlap(int temp_orientation, int temp_coord1, int temp_coord2);
void check_bounds(int x1, int x2, int y1, int y2);
void Game_Start_Ships(); 
void confirm();
