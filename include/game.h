#include <stdio.h>
#include <stdlib.h>
#include "commands.h"

#define SIZE 8

void printGrid(int grid[SIZE][SIZE]);
void check_mv_up();
void check_mv_down();
void check_mv_left();
void check_mv_right();
void check_mv_rot();
void check_overlap(int temp_orientation, int temp_coord1, int temp_coord2);
void check_bounds(int x1, int x2, int y1, int y2);
void Game_Start(); 

// Global Variables which will be there in the code. 
int orientation;
int ship_sizes[5];
int grid[SIZE][SIZE];
int can_move;
int x_bomb, y_bomb; //coordinates for the bomb.

extern int valid_flag;
// extern int x1;
// extern int x2;
// extern int y1; 
// extern int y2;
extern int state; // extern because from other file. -1 is idle state, 0 is set_ship state. 1 is play_game state.
extern int counter;
extern int init_flag; 

extern int coord_array[5][4];


