#include <stdio.h>
#include <stdlib.h>

#define SIZE 8

void printGrid(int grid[SIZE][SIZE]);
void addShip(int grid[SIZE][SIZE], int orientation, int x, int y, int currShipSize);

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
            else if(input == )
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