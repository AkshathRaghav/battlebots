#include <stdio.h>
#include <stdlib.h>
#include "commands.h"

#define SIZE 8

int x1;
int x2;
int y1; 
int y2;
int orientation;
int ship_sizes[5];
int valid_flag;
int grid[SIZE][SIZE];
int x_bomb, y_bomb; 
int placed;
int counter;
int init_flag; 

extern int state; 

void printGrid(int grid[SIZE][SIZE]);
void check_mv_up();
void check_mv_down();
void check_mv_right();
void check_mv_left();
void check_rot();
void check_overlap(int temp_orientation, int temp_coord1, int temp_coord2);
void check_bounds(int x1, int x2, int y1, int y2);
