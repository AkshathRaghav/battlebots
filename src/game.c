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

int coord_array[5][4] = {
        {0, 0, 0, 5},
        {0, 0, 0, 4},
        {0, 0, 0, 3},
        {0, 0, 0, 3},
        {0, 0, 0, 2}
    };

void print_coord_array()
{
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%d ", coord_array[i][j]);
        }
        printf("\n");
    }
}

void printGrid(int grid[SIZE][SIZE]){
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            printf("%d", grid[i][j]);
        }
        printf("\n");
    }
}

void Update_Grid()
{
    //BASED ON COUNTER. SETS SHIP TO EITHER DOT (GREEN) TO SHOW PLACED. Draws the ships currently, being
    switch (counter)
    {
        case 0:
            LCD_Draw_Ship(  coord_array[counter][0],
                            coord_array[counter][1],
                            coord_array[counter][2],
                            coord_array[counter][3],
                            coord_array[counter][4],
                            (valid_flag ? COLOR_GREEN : COLOR_RED));

            Set_Dot(coord_array[counter + 1], COLOR_WHITE);
            Set_Dot(coord_array[counter + 2], COLOR_WHITE);
            Set_Dot(coord_array[counter + 3], COLOR_WHITE);
            Set_Dot(coord_array[counter + 4], COLOR_WHITE);
            break;
        case 1:
            Set_Dot(coord_array[counter - 1], COLOR_GREEN);
            LCD_Draw_Ship(  coord_array[counter][0],
                            coord_array[counter][1],
                            coord_array[counter][2],
                            coord_array[counter][3],
                            coord_array[counter][4],
                            (valid_flag ? COLOR_GREEN : COLOR_RED));
            Set_Dot(coord_array[counter + 1], COLOR_WHITE);
            Set_Dot(coord_array[counter + 2], COLOR_WHITE);
            Set_Dot(coord_array[counter + 3], COLOR_WHITE);
            break;
        case 2:
            Set_Dot(coord_array[counter - 2], COLOR_GREEN);
            Set_Dot(coord_array[counter - 1], COLOR_GREEN);
            LCD_Draw_Ship(  coord_array[counter][0],
                            coord_array[counter][1],
                            coord_array[counter][2],
                            coord_array[counter][3],
                            coord_array[counter][4],
                            (valid_flag ? COLOR_GREEN : COLOR_RED));
            Set_Dot(coord_array[counter + 1], COLOR_WHITE);
            Set_Dot(coord_array[counter + 2], COLOR_WHITE);
            break;
        case 3:
            Set_Dot(coord_array[counter - 3], COLOR_GREEN);
            Set_Dot(coord_array[counter - 2], COLOR_GREEN);
            Set_Dot(coord_array[counter - 1], COLOR_GREEN);
            LCD_Draw_Ship(  coord_array[counter][0],
                            coord_array[counter][1],
                            coord_array[counter][2],
                            coord_array[counter][3],
                            coord_array[counter][4],
                            (valid_flag ? COLOR_GREEN : COLOR_RED));
            Set_Dot(coord_array[counter + 1], COLOR_WHITE);
            break;
        case 4:
            Set_Dot(coord_array[counter - 4], COLOR_GREEN);
            Set_Dot(coord_array[counter - 3], COLOR_GREEN);
            Set_Dot(coord_array[counter - 2], COLOR_GREEN);
            Set_Dot(coord_array[counter - 1], COLOR_GREEN);
            LCD_Draw_Ship(  coord_array[counter][0],
                            coord_array[counter][1],
                            coord_array[counter][2],
                            coord_array[counter][3],
                            coord_array[counter][4],
                            (valid_flag ? COLOR_GREEN : COLOR_RED));
            break;
        default:
            break;
    }
}

void mv_up()
{
    check_mv_up();
    if(state == 0 && can_move)
    {
        LCD_Draw_Ship(  coord_array[counter][0],
        coord_array[counter][1],
        coord_array[counter][2],
        coord_array[counter][3],
        coord_array[counter][4],
        COLOR_WHITE);
        //set_ship state
        // y1--;
        // y2--;
        coord_array[counter][1] = coord_array[counter][0] - 1;
        //need to update the ship drawn.
        Update_Grid();  
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
        LCD_Draw_Ship(  coord_array[counter][0],
        coord_array[counter][1],
        coord_array[counter][2],
        coord_array[counter][3],
        coord_array[counter][4],
        COLOR_WHITE);
        //set_ship state
        // y1++;
        // y2++;
        coord_array[counter][1] = coord_array[counter][0] + 1;
        coord_array[counter][3] = coord_array[counter][2] + 1;
                //need to update the ship drawn.
        Update_Grid();
        
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
    check_mv_left();
    if(state == 0 && can_move)
    {
        LCD_Draw_Ship(  coord_array[counter][0],
        coord_array[counter][1],
        coord_array[counter][2],
        coord_array[counter][3],
        coord_array[counter][4],
        COLOR_WHITE);
        //update the array based on new x1, x2 coords. //x1 -- , x2--
        coord_array[counter][0] = coord_array[counter][0] - 1;
        coord_array[counter][2] = coord_array[counter][2] - 1;

        //need to update the ship drawn.
        Update_Grid();
        
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
        //make the ship's old position white.
        LCD_Draw_Ship(  coord_array[counter][0],
                coord_array[counter][1],
                coord_array[counter][2],
                coord_array[counter][3],
                coord_array[counter][4],
                COLOR_WHITE);
        //update the array based on new x1, x2 coords. x1 ++, x2 ++
        coord_array[counter][0] = coord_array[counter][0] + 1;
        coord_array[counter][2] = coord_array[counter][2] + 1;

        //need to update the ship drawn.
        Update_Grid();

    }
    else if(state == 1 && can_move)
    {
        //play_game state. //now the mv_right is referring to moving the bomb_coordinate. 
        //MIGHT HAVE TO CHECK OUT OF BOUNDS FOR BOMBING.
        x_bomb++;
    }
    // valid_flag = 1;
}

void mv_rot()
{
    check_mv_rot();
    //orientation is changed to new_orientation.
    if(state == 0 && can_move){
            //make the old ship white.
                LCD_Draw_Ship(  coord_array[counter][0],
                coord_array[counter][1],
                coord_array[counter][2],
                coord_array[counter][3],
                coord_array[counter][4],
                COLOR_WHITE);
        orientation = (orientation == 4)? 1: orientation++;

        //based on new_orientation, need to change the x2, y2 coordinates. since x1, y1 are the pivot points
        //for the rotation.
        switch(orientation)
        {
            case(1): 
                    coord_array[counter][2] = coord_array[counter][0];
                    coord_array[counter][3] = coord_array[counter][1] + 1; 
                    break;  
            case(2):
                    coord_array[counter][2] = coord_array[counter][0] + 1;
                    coord_array[counter][3] = coord_array[counter][1];
                    break;
            case(3):
                    coord_array[counter][2] = coord_array[counter][0] ;
                    coord_array[counter][3] = coord_array[counter][1] - 1;
                    break;
            case(4):
                    coord_array[counter][2] = coord_array[counter][0] - 1;
                    coord_array[counter][3] = coord_array[counter][1];
                    break;
        }
        Update_Grid();
    }
}

void check_overlap(int temp_orientation, int temp_coord1, int temp_coord2){
    switch(temp_orientation){
        case 1:
            for(int i = temp_coord1; i >= temp_coord2; i--){
                if(grid[i][coord_array[counter][0]] == 1){
                    valid_flag = 0;
                    return;
                }
            }
            break;
        case 2:
            for(int i = temp_coord1; i <= temp_coord2; i++){
                if(grid[coord_array[counter][1]][i] == 1){
                    valid_flag = 0;
                    return;
                }
            }
            break;
        case 3:
            for(int i = temp_coord1; i <= temp_coord2; i++){
                if(grid[i][coord_array[counter][0]] == 1){
                    valid_flag = 0;
                    return;
                }
            }
            break;
        case 4:
            for(int i = temp_coord1; i >= temp_coord2; i--){
                if(grid[coord_array[counter][1]][i] == 1){
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
                x2_temp = coord_array[counter][0];
                y2_temp = coord_array[counter][1] + 1; 
                break;  
        case(2):
                x2_temp = coord_array[counter][0] + 1;
                y2_temp = coord_array[counter][1];
                break;
        case(3):
                x2_temp = coord_array[counter][0] ;
                y2_temp = coord_array[counter][1] - 1;
                break;
        case(4):
                x2_temp = coord_array[counter][0] - 1;
                y2_temp = coord_array[counter][1];
                break;
    }
    check_bounds(coord_array[counter][0], x2_temp, coord_array[counter][1], y2_temp);
    if(!valid_flag)
    {   
        can_move = 0;
        return;
    }
    check_overlap(temp_orientation, x2_temp, y2_temp);
    return;
}

void check_mv_up(){
    int y1_temp = coord_array[counter][1] - 1;
    int y2_temp = coord_array[counter][3] - 1;
    can_move = 1;
    check_bounds(coord_array[counter][0], coord_array[counter][2], y1_temp, y2_temp); // check if the move up operation will cause out of bounds error
    if(!valid_flag){
        can_move = 0;
        return;
    }
    check_overlap(orientation, y1_temp, y2_temp);
}

void check_mv_down()
{  
    int y1_temp = coord_array[counter][1] + 1;
    int y2_temp = coord_array[counter][3] + 1;
    can_move = 1;
    check_bounds(coord_array[counter][0], coord_array[counter][2], y1_temp, y2_temp);
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
    int x1_temp = coord_array[counter][0] - 1;
    int x2_temp = coord_array[counter][2] - 1;
    can_move = 1;
    check_bounds(x1_temp, x2_temp, coord_array[counter][1], coord_array[counter][3]);
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
    int x1_temp = coord_array[counter][0] + 1;
    int x2_temp = coord_array[counter][2] + 1;
    can_move = 1;
    check_bounds(x1_temp, x2_temp, coord_array[counter][1], coord_array[counter][3]);
    if(!valid_flag)
    {   
        can_move = 0;
        return;
    }
    check_overlap(orientation, x1_temp, x2_temp);
    return;
}

// void init_ship() {
//     orientation = 3;
//     y1 = 0;
//     y2 = ship_sizes[counter] - 1;
//     x1 = 0;
//     x2 = 0;
//     check_overlap(orientation, y1, y2);
//     // draw
// }

void confirm() {
    check_overlap(orientation, coord_array[counter][0], coord_array[counter][2]);
    check_bounds(coord_array[counter][0], coord_array[counter][1],coord_array[counter][2], coord_array[counter][3]);
    if(valid_flag == 1){ // if it's valid, move on to the next ship, iterate the counter
        counter++;
        init_flag = 1; // ready to initialize position of next ship
    }
}

void Game_Start() {
    counter = 0;
    init_flag = 1;
}