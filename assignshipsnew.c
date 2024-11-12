#include <stdio.h>
#include <stdlib.h>

#define SIZE 8

void printGrid(int grid[SIZE][SIZE]);
// void addShip(int grid[SIZE][SIZE], int orientation, int x, int y, int currShipSize);
// int validateAndPlace(int grid[SIZE][SIZE], int orientation, int x1, int y1);
void check_mv_up();
void check_mv_down();
void check_mv_right();
void check_mv_left();
void check_rot();
void check_overlap(int temp_orientation, int temp_coord1, int temp_coord2);
void check_bounds(int x1, int x2, int y1, int y2);

//  ***** GLOBAL VARIABLES *****
int x1;
int x2;
int y1; 
int y2;
int orientation;
int currShipSize;
int valid_flag;
int grid[SIZE][SIZE] = {0};
// ******************************

int main(void){
    // int done = 0;
    // int orientation = 1; // initially vertical ship at 0,0 
    // printGrid(grid);
    // int x1 = 0;
    // int x2 = 0;
    // int y1 = 0;
    // int y2 = 0;
    // int counter = 0;
    // while(!done){  
    //     int currShipSize = 5 - counter;
    //     if(counter == 3){
    //         currShipSize = 3;
    //     }
    //     else if(counter == 4){
    //         currShipSize = 2;
    //     }

    //     // x1 is the bottom of a vertical ship, left side of a horizontal ship initially, recall the pivot stays the same
    //     x1 = 0; 
    //     y1 = 0;
    //     // We don't need an x2 and y2

    //     int repeat = 1;
    //     char input;
        
    //     // if the coordinates are valid, done = 1 and repeat = 0
    //     while(repeat){ // repeat to move the ship 
    //         printf("Enter a key: ");
    //         scanf("%c", &input);
    //         // printf("input is %c\n", input);

    //         if(input == '6'){ // right
    //             x1++;
    //             // printf("input is 6\n");
    //             validate();
    //         }
    //         else if(input == '4'){ // left
    //             x1--;
    //             validate();
    //         }
    //         else if(input == '8'){ // down
    //             y1--;
    //             validate();
    //         }
    //         else if(input == '2'){ // up
    //             y1++;
    //             validate();
    //         }
    //         else if(input == '5'){ // rotate by 90 degrees clockwise
    //             orientation++;
    //             if(orientation == 5){
    //                 orientation = 1;
    //             }
    //             validate();
    //         }
    //         else if(input == '#'){
    //             repeat = validateAndPlace(grid, orientation, x1, y1); // check is 1 is able to place, 0 if unable to place
    //         }

    //         repeat = 0; // hard setting repeat = 0 after 1 cycle, delete
    //     }
        
    //     if(counter == (5 - 1)){
    //         done = 1;
    //     }
    //     counter++;

    //     done = 1; // hard setting done = 1 after 1 cycle, delete
    // }
}
    
void printGrid(int grid[SIZE][SIZE]){
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            printf("%d", grid[i][j]);
        }
        printf("\n");
    }
}

/*
gridvalue = [[01010101], [01010101], [01010101], [01010101], [01010101], [01010101], [01010101], [01010101]]
check_bounds(x1_temp, x2_temp, y1_temp, y2_temp)
    // out of bounds check.
check_overlap(temp_orientation, temp_coord1, temp_coord2)
    //based on temp_orientation
        //loop over either row or column.
            //check if that gridvalue == 1
                //not_valid_flag
                //break.

    
check_mv_up
    //y1_temp =y1+1  and y2_temp = y2 +1 
    //check_bounds(x1, x2, y1_temp, y2_temp)
    //check_overlap(orientation, y1_temp, y2_temp)

    //reaches here if the new ship pos. is valid:
        //y1 = y1_temp
        //y2 = y2_temp
    
check_rot
    // temp_orientation = (orientation == 4)? 1: orientation ++;
    // set x2_temp and y2_temp based on temporary orientation. some case stmt or somn.
    // check_bounds(x1, x2_temp, y1, y2_temp)

eg:
mv_right()
    //check_mv_up
    if(validflag)-> x1++, x2++;
    validflag = 0;
*/

void check_mv_up(){
    int y1_temp = y1 - 1;
    int y2_temp = y2 - 1;
    check_bounds(x1, x2, y1_temp, y2_temp); // check if the move up operation will cause out of bounds error
    if(!valid_flag){
        return;
    }
    check_overlap(orientation, y1_temp, y2_temp);
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