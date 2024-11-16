#include "game.h"
    
int init_flag = 0; // 1 if place ship in top left, 0 if not initializing position of ship
int counter = 0;
int state = -1; 
int grid[SIZE][SIZE] = {0};
int ship_sizes[5] = {5, 4, 3, 3, 2};
int x1 = 0; 
int x2 = 0; 
int y1 = 0; 
int y2 = 0; 
int valid_flag = 1;

void printGrid(int grid[SIZE][SIZE]){
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            printf("%d", grid[i][j]);
        }
        printf("\n");
    }
}

void mv_up()
{
    check_mv_up();
    if(state == 0 && can_move)
    {
        //set_ship state
        y1--;
        y2--;     
    }
    else if(state == 1 && can_move)
    {
        //play_game state 
        y_bomb--;
    }
    // valid_flag = 1;
}

void mv_down()
{
    check_mv_down();
    if(state == 0 && can_move)
    {
        //set_ship state
        y1++;
        y2++;
        
    }
    else if(state == 1 && can_move)
    {
        //play_game state 
        y_bomb++;
    }
    // valid_flag = 1;
}

void mv_left()
{
    check_mv_left;
    if(state == 0 && can_move)
    {
        //set_ship state
        x1--;
        x2--;
        
    }
    else if(state == 1 && can_move)
    {
        //play_game state 
        x_bomb--;
    }
    // valid_flag = 1;
}

void mv_right()
{
    check_mv_right();
    if(state == 0 && can_move)
    {
        //set_ship state: mv_right refers to moving the ship rightwards.
        x1++;
        x2++;
    }
    else if(state == 1 && can_move)
    {
        //play_game state. //now the mv_right is referring to moving the bomb_coordinate. 
        x_bomb++;
    }
    // valid_flag = 1;
}

void mv_rot()
{
    check_mv_rot();
    //orientation is changed to new_orientation.
    if(can_move){
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

void check_mv_rot()
{
    int x2_temp, y2_temp;
    int temp_orientation = (orientation == 4)? 1: orientation++;
    can_move = 1;
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
        can_move = 0;
        return;
    }
    check_overlap(temp_orientation, x2_temp, y2_temp);
    return;
}

void check_mv_up(){
    int y1_temp = y1 - 1;
    int y2_temp = y2 - 1;
    can_move = 1;
    check_bounds(x1, x2, y1_temp, y2_temp); // check if the move up operation will cause out of bounds error
    if(!valid_flag){
        can_move = 0;
        return;
    }
    check_overlap(orientation, y1_temp, y2_temp);
}

void check_mv_down()
{  
    int y1_temp = y1 + 1;
    int y2_temp = y2_temp + 1;
    can_move = 1;
    check_bounds(x1, x2, y1_temp, y2_temp);
    if(!valid_flag)
    {
        can_move = 0;
        return;
    }
    check_overlap(orientation, y1_temp, y2_temp);
    return;
}

void check_mv_left()
{
    int x1_temp = x1 - 1;
    int x2_temp = x2 - 1;
    can_move = 1;
    check_bounds(x1_temp, x2_temp, y1, y2);
    if(!valid_flag)
    {
        can_move = 0;
        return;
    }
    check_overlap(orientation, x1_temp, x2_temp);
    return;
}

void check_mv_right()
{   
    int x1_temp = x1 + 1;
    int x2_temp = x2 + 1;
    can_move = 1;
    check_bounds(x1_temp, x2_temp, y1, y2);
    if(!valid_flag)
    {   
        can_move = 0;
        return;
    }
    check_overlap(orientation, x1_temp, x2_temp);
    return;
}

void init_ship() {
    orientation = 3;
    y1 = 0;
    y2 = ship_sizes[counter] - 1;
    x1 = 0;
    x2 = 0;
    check_overlap(orientation, y1, y2);\
    // draw
}

void confirm() {
    check_overlap(orientation, x1, x2);
    check_bounds(x1, x2, y1, y2);
    if(valid_flag == 1){ // if it's valid, move on to the next ship, iterate the counter
        counter++;
        init_flag = 1; // ready to initialize position of next ship
    }
}

void Game_Start() {
    counter = 0;
    init_flag = 1;
}