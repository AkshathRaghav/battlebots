#include "game.h"
#include "intercomm.h"

GameState state = LOADING_SCREEN; 
int init_flag = 0; 
int counter = 0;
int hits[SIZE][SIZE] = {0}; // 0 is unattempted; 1 for missed; 2 is for hit
int grid[SIZE][SIZE] = {0}; // Ships, 1 = ship is there, 0 = no ship 
int ship_sizes[5] = {5, 4, 3, 3, 2};
int orientation = 3;
int valid_flag = 1;
int cursor_x = 0; 
int cursor_y = 0; 
int ship_hit_counter = 0;

int coord_array[5][4] = {
        {0, 0, 0, 4},
        {0, 0, 0, 3},
        {0, 0, 0, 2},
        {0, 0, 0, 2},
        {0, 0, 0, 1}
    };

int turn_flag = 0; // Gets initially set in main.c#SystickHandler


void Game_Reset() {
    init_flag = 0; 
    counter = 0;
    state = LOADING_SCREEN; 
    orientation = 3;
    valid_flag = 1;
    cursor_y = 0; 
    ship_hit_counter = 0;
    cursor_x = 0; 
    
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

void Game_ReadBomb(){
    uint8_t recived_data = COMM_ReadData(); 
    
    // if (recived_data == (uint8_t)(69)) { 
    //     state = END
    // }

    int boomb_x = recived_data & 0xF; 
    int boomb_y = (recived_data & 0xF0) >> 4; // Dumbass

    // If there is a ship, send 255, else 0 
    COMM_SendData(grid[boomb_x][boomb_y] == 1 ? (uint8_t)(255) : (uint8_t)0);
}

void Game_Confirm_Cursor() { 
    // Only confirm move if not played before 
    if (turn_flag) { 
        // if your turn, send the x y coord
        if (hits[cursor_x][cursor_y] == 0) { 
            turn_flag = 0; 
            COMM_SendData(((cursor_y << 4) | cursor_x));                                          
            uint8_t received = COMM_ReadData();     

            // Missed data packet =  0
            // Hit data packet = 255 
            hits[cursor_x][cursor_y] = (received == 0) ? 1 : (received == 255) ? 2 : 0; 
            ship_hit_counter = (received == (uint8_t) 255) ? (ship_hit_counter + 1) : ship_hit_counter;
            
            // not an error, do not change
            set_dot(cursor_y, cursor_x, ((received == 0) ? COLOR_BLACK : (received == 255) ? COLOR_RED : COLOR_PINK));
        }
    }
}


void check_overlap_hits(int x, int y) { 
    if (hits[x][y] != 0) valid_flag = 0; 
}

void Game_MvRight_Cursor() {
    can_move = 1; 
    check_bounds(0, 0, cursor_x + 1, cursor_y); 

    valid_flag = 1; 
    check_overlap_hits(cursor_x + 1, cursor_y);

    if (can_move) { 
        LCD_DrawCursor(COLOR_WHITE);
        cursor_x++; 
        LCD_DrawCursor((valid_flag ? COLOR_GREEN : COLOR_RED));
    }
}

void Game_MvDown_Cursor() { 
    can_move = 1; 
    check_bounds(0, 0, cursor_x, cursor_y + 1); 

    valid_flag = 1; 
    check_overlap_hits(cursor_x, cursor_y + 1);

    if (can_move) { 
        LCD_DrawCursor(COLOR_WHITE);
        cursor_y++; 
        LCD_DrawCursor((valid_flag ? COLOR_GREEN : COLOR_RED));
    }
}

void Game_MvUp_Cursor() { 
    can_move = 1; 
    check_bounds(0, 0, cursor_x, cursor_y - 1); 

    valid_flag = 1; 
    check_overlap_hits(cursor_x, cursor_y - 1);

    if (can_move) { 
        LCD_DrawCursor(COLOR_WHITE);
        cursor_y--; 
        LCD_DrawCursor((valid_flag ? COLOR_GREEN : COLOR_RED));
    }
}

void Game_MvLeft_Cursor() { 
    can_move = 1; 
    check_bounds(0, 0, cursor_x - 1, cursor_y); 

    valid_flag = 1; 
    check_overlap_hits(cursor_x - 1, cursor_y);

    if (can_move) { 
        LCD_DrawCursor(COLOR_WHITE);
        cursor_x--; 
        LCD_DrawCursor((valid_flag ? COLOR_GREEN : COLOR_RED));
    }
}

void LCD_DrawCursor(Color color) { 
    DrawShip( 
        cursor_x,
        cursor_y, 
        cursor_x, 
        cursor_y, 
        color
    );
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
    if (can_move) {
        LCD_DrawShip(COLOR_WHITE); 
        coord_array[counter][1] = coord_array[counter][1] - 1;  
        coord_array[counter][3] = coord_array[counter][3] - 1; 
        LCD_DrawShip((valid_flag ? COLOR_GREEN : COLOR_RED)); 
    }
}

void Game_MvDown() {
    check_mv_down();
    if (can_move) {
        LCD_DrawShip(COLOR_WHITE);
        coord_array[counter][1] = coord_array[counter][1] + 1; 
        coord_array[counter][3] = coord_array[counter][3] + 1;   
        LCD_DrawShip((valid_flag ? COLOR_GREEN : COLOR_RED)); 
    }
}

void Game_MvLeft() {
    check_mv_left();
    if (can_move) {
        LCD_DrawShip(COLOR_WHITE);
        coord_array[counter][0] = coord_array[counter][0] - 1;
        coord_array[counter][2] = coord_array[counter][2] - 1;
        LCD_DrawShip((valid_flag ? COLOR_GREEN : COLOR_RED)); 
    }
}

void Game_MvRight() {
    check_mv_right();
    if (can_move) {
        LCD_DrawShip(COLOR_WHITE);
        coord_array[counter][0] = coord_array[counter][0] + 1; 
        coord_array[counter][2] = coord_array[counter][2] + 1; 
        LCD_DrawShip((valid_flag ? COLOR_GREEN : COLOR_RED)); 
    }
}

void Game_MvRot() {
    check_mv_rot();

    // Orientation is changed to new_orientation.
    if (state == SET_SHIPS && can_move) {
        LCD_DrawShip(COLOR_WHITE);

        orientation = (orientation == 4) ? 1 : (orientation + 1); 
        
        // Updating orientation 
        // Based on new_orientation, need to change the x2, y2 coordinates.
        // Since x1, y1 are the pivot points for the rotation.
        switch(orientation)
        {
            //                0    1    2    3
            // coord_array : x_1, y_1, x_2, y_2 
            case(1): 
                    // (x_1, y_1, x_2, y_2) -> (x_1, y_1, x_1, y_1 + ship_size -1 )
                    coord_array[counter][2] = coord_array[counter][0]; 
                    coord_array[counter][3] = coord_array[counter][1] - ship_sizes[counter] + 1; 
                    break;  
            case(2):
                    // (x_1, y_1, x_2, y_2) -> (x_1, y_1, x_1 + ship_size - 1, y_1)
                    coord_array[counter][2] = coord_array[counter][0] + ship_sizes[counter] - 1;
                    coord_array[counter][3] = coord_array[counter][1];
                    break;
            case(3):
                    // (x_1, y_1, x_2, y_2) -> (x_1, y_1, x_1, y_1 + 1) ?? 
                    coord_array[counter][2] = coord_array[counter][0] ;
                    coord_array[counter][3] = coord_array[counter][1] + ship_sizes[counter] - 1;
                    break;
            case(4):
                    // (x_1, y_1, x_2, y_2) -> (x_1, y_1, x_1 - ship_size + 1, y_1); 
                    coord_array[counter][2] = coord_array[counter][0] - ship_sizes[counter] + 1; 
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
    can_move = (_check(y1) || _check(y2) || _check(x1) || _check(x2)) ? 0 : 1; 
}

void check_mv_rot() {
    int x1_temp = coord_array[counter][0];
    int y1_temp = coord_array[counter][1];


    int x2_temp = coord_array[counter][2];
    int y2_temp = coord_array[counter][3];

    int shipsize = ship_sizes[counter]; 

    int temp_orientation = (orientation == 4)? 1: (orientation+ 1);
    
    switch (temp_orientation) {
        case(1): 
                x2_temp = x1_temp;
                y2_temp = y1_temp - shipsize + 1; 
                break;  
        case(2):
                x2_temp = x1_temp + shipsize - 1; 
                y2_temp = y1_temp; 
                break;
        case(3):
                x2_temp = x1_temp;
                y2_temp = y1_temp + shipsize - 1; 
                break;
        case(4):
                x2_temp = x1_temp - shipsize + 1; 
                y2_temp = y1_temp;
                break;
    }

    can_move = 1;
    valid_flag = 1; 

    check_bounds(x1_temp, x2_temp, y1_temp, y2_temp);
    check_overlap(y1_temp, y2_temp, x1_temp, x2_temp); 
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