#include "game.h"
    
int init_flag = 0; // Ship Placement Toggle 
int counter = 0;
GameState state = LOADING_SCREEN; 
int grid[SIZE][SIZE] = {0};
int ship_sizes[5] = {5, 4, 3, 3, 2};
int orientation = 3;
int valid_flag = 1;

int coord_array[5][4] = {
        {0, 0, 0, 4},
        {0, 0, 0, 3},
        {0, 0, 0, 2},
        {0, 0, 0, 2},
        {0, 0, 0, 1}
    };

void Game_Reset() {
    init_flag = 0; 
    counter = 0;
    state = LOADING_SCREEN; 
    orientation = 3;
    valid_flag = 1;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            grid[i][j] = 0;
        }
    }
    int default_coord_array[5][4] = {
        {0, 0, 0, 4},
        {0, 0, 0, 3},
        {0, 0, 0, 2},
        {0, 0, 0, 2},
        {0, 0, 0, 1}
    };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 4; j++) {
            coord_array[i][j] = default_coord_array[i][j];
        }
    }
}

void print_coord_array() {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%d ", coord_array[i][j]);
        }
        printf("\n");
    }
}

void printGrid(int grid[SIZE][SIZE]) {
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            printf("%d", grid[i][j]);
        }
        printf("\n");
    }
}

void LCD_DrawShip(Color color) { 
    DrawShip(
        coord_array[counter][0],
        coord_array[counter][1],
        coord_array[counter][2],
        coord_array[counter][3],
        color
    );
}

void Game_MvUp() {
    check_mv_up();
    if(state == SET_SHIPS && can_move)
    {
        LCD_DrawShip(COLOR_WHITE); // Clears old boundary
        coord_array[counter][1] = coord_array[counter][1] - 1;  // y1 = y1 - 1; 
        coord_array[counter][3] = coord_array[counter][3] - 1;  // y2 = y2 - 1
        LCD_DrawShip((valid_flag ? COLOR_GREEN : COLOR_RED)); 
    }
    else if(state == BOMB_SHIPS && can_move)
    {
        y_bomb--;
    }
    // valid_flag = 1;
}

void Game_MvDown() {
    check_mv_down();
    if(state == SET_SHIPS && can_move)
    {
        LCD_DrawShip(COLOR_WHITE); // Clears old boundary
        coord_array[counter][1] = coord_array[counter][1] + 1; // y1 = y1 + 1
        coord_array[counter][3] = coord_array[counter][3] + 1; // y2 = y2 + 1   
        LCD_DrawShip((valid_flag ? COLOR_GREEN : COLOR_RED)); 
    }
    else if(state == BOMB_SHIPS && can_move)
    {
        //play_game state 
        y_bomb++;
    }
    // valid_flag = 1;
}

void Game_MvLeft() {
    check_mv_left();

    if(state == SET_SHIPS && can_move)
    {
        LCD_DrawShip(COLOR_WHITE);
        coord_array[counter][0] = coord_array[counter][0] - 1;
        coord_array[counter][2] = coord_array[counter][2] - 1;
        LCD_DrawShip((valid_flag ? COLOR_GREEN : COLOR_RED)); 
    }
    else if(state == BOMB_SHIPS && can_move)
    {
        //play_game state 
        x_bomb--;
    }
    // valid_flag = 1;
}

void Game_MvRight() {
    check_mv_right();

    if (state == SET_SHIPS && can_move) {
        LCD_DrawShip(COLOR_WHITE);
        coord_array[counter][0] = coord_array[counter][0] + 1; // x1 = x1 + 1
        coord_array[counter][2] = coord_array[counter][2] + 1; // x2 = x2 + 1
        LCD_DrawShip((valid_flag ? COLOR_GREEN : COLOR_RED)); 
    }
    else if(state == BOMB_SHIPS && can_move) {
        // play_game state.
        x_bomb++;
    }
    // valid_flag = 1;
}

void Game_MvRot() {
    check_mv_rot();

    // Orientation is changed to new_orientation.
    if (state == SET_SHIPS && can_move) {
        LCD_DrawShip(COLOR_WHITE);

        if (orientation == 4) orientation = 1; 
        else orientation++; 

        // Based on new_orientation, need to change the x2, y2 coordinates.
        // Since x1, y1 are the pivot points for the rotation.
        switch(orientation)
        {
            case(1): 
                    coord_array[counter][2] = coord_array[counter][0]; 
                    coord_array[counter][3] = coord_array[counter][1] - 1; 
                    break;  
            case(2):
                    coord_array[counter][2] = coord_array[counter][0] + 1;
                    coord_array[counter][3] = coord_array[counter][1];
                    break;
            case(3):
                    coord_array[counter][2] = coord_array[counter][0] ;
                    coord_array[counter][3] = coord_array[counter][1] + 1;
                    break;
            case(4):
                    coord_array[counter][2] = coord_array[counter][0] - 1;
                    coord_array[counter][3] = coord_array[counter][1];
                    break;
        }

        LCD_DrawShip((valid_flag ? COLOR_GREEN : COLOR_RED)); 
    }
}

void check_overlap(int x1, int x2, int y1, int y2) { 
    int smaller, larger; 

    if (y1 == y2) {
        if (x1 < x2) { 
            larger = x2; 
            smaller = x1; 
        } else {
            larger = x1; 
            smaller = x2; 
        } 
        for (int i = smaller; i <= larger; i++) { 
            if (grid[i][y1] == 1) { 
                valid_flag = 0; 
                return; 
            }
        }
    } else { 
        if (y1 < y2) { 
            larger = y2; 
            smaller = y1; 
        } else {
            larger = y1; 
            smaller = y2; 
        } 
        for (int i = smaller; i <= larger; i++) { 
            if (grid[x1][i] == 1) { 
                valid_flag = 0; 
                return;
            }
        }
    }
}

int _check(int coord) { 
    return (coord < 0 || coord > (SIZE-1));
}

void check_bounds(int x1, int x2, int y1, int y2) {
    return (_check(y1) || _check(y2) || _check(x1) || _check(x2)) ? 0 : 1; 
}

void check_mv_rot() {
    int x2_temp, y2_temp;
    int temp_orientation = (orientation == 4)? 1: orientation++;
    can_move = 1;
    
    switch (temp_orientation) {
        case(1): 
                x2_temp = coord_array[counter][0];
                y2_temp = coord_array[counter][1] - 1; 
                break;  
        case(2):
                x2_temp = coord_array[counter][0] + 1;
                y2_temp = coord_array[counter][1];
                break;
        case(3):
                x2_temp = coord_array[counter][0] ;
                y2_temp = coord_array[counter][1] + 1;
                break;
        case(4):
                x2_temp = coord_array[counter][0] - 1;
                y2_temp = coord_array[counter][1];
                break;
    }
    
    check_bounds(coord_array[counter][0], x2_temp, coord_array[counter][1], y2_temp);

    if (!valid_flag) can_move = 0;
    else check_overlap(coord_array[counter][0], x2_temp, coord_array[counter][1], y2_temp);
}

void check_mv_up() {
    int y1_temp = coord_array[counter][1] - 1;
    int y2_temp = coord_array[counter][3] - 1;

    can_move = 1;
    check_bounds(coord_array[counter][0], coord_array[counter][2], y1_temp, y2_temp); // check if the move up operation will cause out of bounds error
    
    valid_flag = 1; 
    check_overlap(y1_temp, y2_temp, coord_array[counter][0], coord_array[counter][2]);
}

void check_mv_down() {  
    int y1_temp = coord_array[counter][1] + 1;
    int y2_temp = coord_array[counter][3] + 1;

    can_move = 1;
    check_bounds(coord_array[counter][0], coord_array[counter][2], y1_temp, y2_temp);

    valid_flag = 1; 
    check_overlap(y1_temp, y2_temp, coord_array[counter][0], coord_array[counter][2]);
}

void check_mv_left() {
    int x1_temp = coord_array[counter][0] - 1;
    int x2_temp = coord_array[counter][2] - 1;

    can_move = 1;
    check_bounds(x1_temp, x2_temp, coord_array[counter][1], coord_array[counter][3]);

    valid_flag = 1; 
    check_overlap(coord_array[counter][1], coord_array[counter][3], x1_temp, x2_temp);   
}

void check_mv_right() {   
    int x1_temp = coord_array[counter][0] + 1;
    int x2_temp = coord_array[counter][2] + 1;

    can_move = 1;
    check_bounds(x1_temp, x2_temp, coord_array[counter][1], coord_array[counter][3]);

    valid_flag = 1; 
    check_overlap(coord_array[counter][1], coord_array[counter][3], x1_temp, x2_temp);   
}

void Game_Confirm() {
    valid_flag = 1; 
    check_overlap(coord_array[counter][1], coord_array[counter][3], coord_array[counter][0], coord_array[counter][2]);
    
    if (valid_flag) { 
        // Increment counter, toggle init_flag for main.c#SysTickHandler()
        LCD_DrawShip(COLOR_WHITE);
        LCD_SetValidDots( // Reverses the assumed XY orientation; Ping Akshath if needed.
            coord_array[counter][1],
            coord_array[counter][0],
            coord_array[counter][3],
            coord_array[counter][2]
        );
        counter++;
        init_flag = 1; 
    }

    valid_flag = 1; 
}

void Game_Start_Ships() {
    counter = 0;
    init_flag = 1;
}