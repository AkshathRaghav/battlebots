/*
 This is a file to set up the location and orientation of your 5 battleships in a 8x8 Grid Space
 */
#include <stdio.h>
#include <stdlib.h>

#define SIZE 8

void printGrid(int grid[SIZE][SIZE]);
void addShip(int grid[SIZE][SIZE], int orientation, int x, int y, int currShipSize);

int main(void){
    printf("Welcome to Battleship! Here is your empty %dx%d ocean!\n", SIZE, SIZE);
    int grid [SIZE][SIZE] = {0};
    printGrid(grid);
    int orientation;
    int done = 0;
    int counter = 0;
    while(!done){
        int repeat = 1;
        printf("Battleship #%d: pick an orientation\n", counter + 1);
        while(repeat){
            printf("Press 1 for horizontal, 2 for vertical ship\n");
            scanf("%d", &orientation);
            if(orientation == 1 || orientation == 2){
                repeat = 0;
            }
            else{
                printf("Not a 1 or 0, please try again!\n");
            }
        }
        repeat = 1;
        int currShipSize = 5 - counter;
        if(counter == 3){
            currShipSize = 3;
        }
        else if(counter == 4){
            currShipSize = 2;
        }
        int x, y;
        printf("Now pick the rightmost/topmost coordinates of the ship of size %d, with the top left coordinate being (0,0)\n", currShipSize);
        while(repeat){
            printf("Type coords in form 'x,y', include commma\n");
            scanf("%d,%d", &x, &y);
            if(!(x < 0 || x > SIZE-1 || y < 0 || y > SIZE-1) || !(orientation==1 && (x - currShipSize + 1 < 0)) || !(orientation==0 && (y + currShipSize -1 < SIZE))){
                repeat = 0;
            }
            else{
                printf("Try again, pick coordinates between 0-%d and a battleship that fits within the grid\n", SIZE-1);
            }
        }

        addShip(grid, orientation, x, y, currShipSize);

        printf("grid now looks like\n");
        printGrid(grid);
        
        if(counter == (5 - 1)){
            done = 1;
        }

        counter++;
    }
}


void addShip(int grid[SIZE][SIZE], int orientation, int x, int y, int currShipSize){
    if(orientation == 1){ // horizontal
        for(int i = x; i > x - currShipSize; i--){
            grid[y][i] = 1;
        }
    }
    else{
        for(int i = y; i > y - currShipSize; i--){
            grid[i][x] = 1;
        }
    }
    return;
}

void printGrid(int grid[SIZE][SIZE]){
    for(int i = 0; i < SIZE; i++){
        printf("%c");
        for(int j = 0; j < SIZE; j++){
            printf("%d", grid[i][j]);
        }
        printf("\n");
    }
}
