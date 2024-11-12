#include <stdio.h>
#include <stdlib.h>

#define SIZE 8

void printGrid(int grid[SIZE][SIZE]);
void addShip(int grid[SIZE][SIZE], int orientation, int x, int y, int currShipSize);

void send_key(char input);

void send_key(char input)
{

}

void mv_right()
{
    x1++;
    x2++;
}
int main(void){
    int done = 0;
    int orientation = 2; // initially vertical ship at 0,0 
    int grid[SIZE][SIZE] = {0};
    printGrid(grid);
    int x1 = 0;
    int x2 = 0;
    int y1 = 0;
    int y2 = 0;
    int counter = 0;
    while(!done){
        int currShipSize = 5 - counter;
        if(counter == 3){
            currShipSize = 3;
        }
        else if(counter == 4){
            currShipSize = 2;
        }

        x1 = 0;
        y1 = 0;
        x2 = 0;
        y2 = currShipSize - 1;

        int repeat = 1;
        int input;
        // if the coordinates are valid, done = 1 and repeat = 0
        while(repeat){ // repeat to move the ship 
            printf("Enter a key: ");
            scanf("%d", &input);
            if(input == 6){
                x1++;
                x2++;
            }
            repeat = 0; // hard setting repeat = 0 after 1 cycle, delete
        }
        
        if(counter == (5 - 1)){
            done = 1;
        }
        counter++;

        done = 1; // hard setting done = 1 after 1 cycle, delete
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

// void updatePins(char direction, ) // decrement the old pins and increment the new ones


/*
gridvalue = [[01010101], [01010101], [01010101], [01010101], [01010101], [01010101], [01010101], [01010101]]
check_overlap(temp_orientation, temp_coord1, temp_coord2)
    //based on temp_orientation
        //loop over either row or column.
            //check if that gridvalue == 1
                //not_valid_flag
                //break.
check_bounds(x1_temp, x2_temp, y1_temp, y2_temp)
    // out of bounds check.

    
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

//valid_check(move_type)
    //case(move_type)
    2 -> check_mv_up()
    4
    6
    8

    5 -> check_rot() //this has access to the current glob. orientation.

eg:
mv_right()
    //valid_check(right)
    if(validflag)-> x1++, x2++;
*/

