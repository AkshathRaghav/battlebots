#include <stdio.h>
#include <stdlib.h>
#define SIZE 8

void printGrid(int grid[SIZE][SIZE]);
void check_mv_up();
void check_mv_down();
void check_mv_right();
void check_mv_left();
void check_rot();
void check_overlap(int temp_orientation, int temp_coord1, int temp_coord2);
void check_bounds(int x1, int x2, int y1, int y2);

//Global Variables which will be there in the code. 
int x1;
int x2;
int y1; 
int y2;
int orientation;
int ship_sizes [5] = {5, 4, 3, 3, 2};
int valid_flag;
int grid[SIZE][SIZE] = {0};
int state = 0; //0 is set_ship state. 1 is play_game state.
int x_bomb, y_bomb; //coordinates for the bomb.
int placed; // 0 if ship placed, 1 if ship isn't placed yet
int counter = 0;

int main(void){
    return 0;
}
    
void printGrid(int grid[SIZE][SIZE]){
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            printf("%d", grid[i][j]);
        }
        printf("\n");
    }
}

void mv_right()
{
    check_mv_right();
    if(state == 0 && valid_flag)
    {
        //set_ship state: mv_right refers to moving the ship rightwards.
        x1++;
        x2++;
    }
    else if(state == 1 && valid_flag)
    {
        //play_game state. //now the mv_right is referring to moving the bomb_coordinate. 
        x_bomb++;
    }
}

void mv_left()
{
    check_mv_left;
    if(state == 0 && valid_flag)
    {
        //set_ship state
        x1--;
        x2--;
        
    }
    else if(state == 1 && valid_flag)
    {
        //play_game state 
        x_bomb--;
    }
}

void mv_up()
{
    check_mv_up();
    if(state == 0 && valid_flag)
    {
        //set_ship state
        y1--;
        y2--;
        
    }
    else if(state == 1 && valid_flag)
    {
        //play_game state 
        y_bomb--;
    }
}

void mv_down()
{
    check_mv_down();
    if(state == 0 && valid_flag)
    {
        //set_ship state
        y1++;
        y2++;
        
    }
    else if(state == 1 && valid_flag)
    {
        //play_game state 
        y_bomb++;
    }
}

void mv_rot()
{
    check_mv_rot;
    //orientation is changed to new_orientation.
    orientation = (orientation == 4)? 1: orientation++;

    //based on new_orientation, need to change the x2, y2 coordinates. since x1, y1 are the pivot points
    //for the rotation.
    switch(orientation)
    {
        case(1): 
                x2 = x1;
                y2 = y1 + 1; 
                break;  
        case(2):
                x2 = x1 + 1;
                y2 = y1;
                break;
        case(3):
                x2 = x1 ;
                y2 = y1 - 1;
                break;
        case(4):
                x2 = x1 - 1;
                y2 = y1;
                break;
    }
}

void check_overlap(int temp_orientation, int temp_coord1, int temp_coord2){
    switch(temp_orientation){
        case 1:
            for(int i = temp_coord1; i >= temp_coord2; i--){
                if(grid[i][x1] == 1){
                    valid_flag = 0;
                    return;
                }
            }
            break;
        case 2:
            for(int i = temp_coord1; i <= temp_coord2; i++){
                if(grid[y1][i] == 1){
                    valid_flag = 0;
                    return;
                }
            }
            break;
        case 3:
            for(int i = temp_coord1; i <= temp_coord2; i++){
                if(grid[i][x1] == 1){
                    valid_flag = 0;
                    return;
                }
            }
            break;
        case 4:
            for(int i = temp_coord1; i >= temp_coord2; i--){
                if(grid[y1][i] == 1){
                    valid_flag = 0;
                    return;
                }
            }
        break;
    }
}

void check_bounds(int x1, int x2, int y1, int y2){
    if(y1 < 0 || y1 > (SIZE-1) || y2 < 0 || y2 > (SIZE-1) || x1 < 0 || x1 > (SIZE-1) || x2 < 0 || x2 > (SIZE-1)){
        valid_flag = 0;
    }
    else{
        valid_flag = 1;
    }
}

void check_mv_up(){
    int y1_temp = y1 - 1;
    int y2_temp = y2 - 1;
    check_bounds(x1, x2, y1_temp, y2_temp); // check if the move up operation will cause out of bounds error
    if(!valid_flag){
        return;
    }
    check_overlap(orientation, y1_temp, y2_temp);
}

void check_rot()
{
    int x2_temp, y2_temp;
    int temp_orientation = (orientation == 4)? 1: orientation++;
    switch(temp_orientation)
    {
        case(1): 
                x2_temp = x1;
                y2_temp = y1 + 1; 
                break;  
        case(2):
                x2_temp = x1 + 1;
                y2_temp = y1;
                break;
        case(3):
                x2_temp = x1 ;
                y2_temp = y1 - 1;
                break;
        case(4):
                x2_temp = x1 - 1;
                y2_temp = y1;
                break;
    }

    check_bounds(x1, x2_temp, y1, y2_temp);
    if(!valid_flag)
    {
        return;
    }
    check_overlap(temp_orientation, x2_temp, y2_temp);
    return;
}

void check_mv_down()
{
    int y1_temp = y1 + 1;
    int y2_temp = y2_temp + 1;
    check_bounds(x1, x2, y1_temp, y2_temp);
    if(!valid_flag)
    {
        return;
    }
    check_overlap(orientation, y1_temp, y2_temp);
    return;
}

void check_mv_left()
{
    int x1_temp = x1 - 1;
    int x2_temp = x2 - 1;
    check_bounds(x1_temp, x2_temp, y1, y2);
    if(!valid_flag)
    {
        return;
    }
    check_overlap(orientation, x1_temp, x2_temp);
    return;
}

void check_mv_right()
{
    int x1_temp = x1 + 1;
    int x2_temp = x2 + 1;
    check_bounds(x1_temp, x2_temp, y1, y2);
    if(!valid_flag)
    {
        return;
    }
    check_overlap(orientation, x1_temp, x2_temp);
    return;
}

void start()
{
    state = 0; // set_ship state
    // readyup()

    while(counter < sizeof(ship_sizes)/sizeof(ship_sizes[0])){
        init_ship(); // set location of ship to 0,0
        placed = 0;

        if(placed){
            counter++;
        }
    }
}

void init_ship()
{
    orientation = 3;
    x1 = 0;
    x2 = ship_sizes[counter] - 1;
    check_overlap(orientation, x1, x2);
}