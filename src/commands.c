
#include "stm32f0xx.h"
#include "lcd.h"
#include "commands.h"
#include <string.h>
#include <stdio.h>

void led_on(int on)
{
    GPIOB->BSRR |= (on ? GPIO_BSRR_BS_12 : GPIO_BSRR_BR_12);
}

void led_wait(int on)
{
    GPIOB->BSRR |= (on ? GPIO_BSRR_BS_13 : GPIO_BSRR_BR_13);
}

void debug_draw(){
  for(int i = 0; i < SIZE; i ++ )
    for(int j = 0; j < SIZE; j ++)
      set_dot(i, j, grid[i][j] ? COLOR_GREEN : COLOR_PINK);
}

void lcd_init(int argc, char *argv[])
{
    LCD_Setup();
}

void clear(int argc, char *argv[])
{
    int value = 0;
    if (argc == 2)
        value = strtoul(argv[1], 0, 16);
    LCD_Clear(value);
}


void drawline(int argc, char *argv[])
{
    if (argc != 6) {
        printf("Wrong number of arguments: line x1 y1 x2 y2 color");
        return;
    }
    int x1 = strtoul(argv[1], 0, 10);
    int y1 = strtoul(argv[2], 0, 10);
    int x2 = strtoul(argv[3], 0, 10);
    int y2 = strtoul(argv[4], 0, 10);
    int c = strtoul(argv[5], 0, 16);
    LCD_DrawLine(x1,y1,x2,y2,c);
}

void drawrect(int argc, char *argv[])
{
    if (argc != 6) {
        printf("Wrong number of arguments: drawrect x1 y1 x2 y2 color");
        return;
    }
    int x1 = strtoul(argv[1], 0, 10);
    int y1 = strtoul(argv[2], 0, 10);
    int x2 = strtoul(argv[3], 0, 10);
    int y2 = strtoul(argv[4], 0, 10);
    int c = strtoul(argv[5], 0, 16);
    LCD_DrawRectangle(x1,y1,x2,y2,c);
}

void drawfillrect(int argc, char *argv[])
{
    if (argc != 6) {
        printf("Wrong number of arguments: drawfillrect x1 y1 x2 y2 color");
        return;
    }
    int x1 = strtoul(argv[1], 0, 10);
    int y1 = strtoul(argv[2], 0, 10);
    int x2 = strtoul(argv[3], 0, 10);
    int y2 = strtoul(argv[4], 0, 10);
    int c = strtoul(argv[5], 0, 16);
    LCD_DrawFillRectangle(x1,y1,x2,y2,c);
}

void LCD_DrawGrid() { 
    for (int i = 30; i <= 240; i += 30) { 
        LCD_DrawLine(i-1, 0, i-1, 320, 0000);
        LCD_DrawLine(i, 0, i, 320, 0000);
        LCD_DrawLine(i+1, 0, i+1, 320, 0000);
    }
    for (int i = 40; i <= 320; i += 40) { 
        LCD_DrawLine(0, i-1, 320, i-1, 0000);
        LCD_DrawLine(0, i, 320, i, 0000);
        LCD_DrawLine(0, i+1, 320, i+1, 0000);
    }
}

void get_dot_bbox(int i, int j, int *x1, int *y1, int *x2, int *y2) {
    int x = 15 + (j * 30); 
    int y = 20 + (i * 40); 

    *x1 = x - 5;
    *y1 = y - 5;
    *x2 = x + 5;
    *y2 = y + 5;
}

void set_dot(int x, int y, Color color) { 
    int x1, y1, x2, y2; 
    get_dot_bbox(x, y, &x1, &y1, &x2, &y2);
    LCD_DrawFillRectangle(x1, y1, x2, y2, color); 
}

void LCD_DrawCoords() {
    for (int x = 15; x <= 240; x += 30) {       
        for (int y = 20; y <= 320; y += 40) {   
            int x1 = x - 5;
            int y1 = y - 5;
            int x2 = x + 5;
            int y2 = y + 5;
            LCD_DrawFillRectangle(x1, y1, x2, y2, COLOR_BLUE); 
        }
    }
}

void LCD_SetValidDots(int x1, int y1, int x2, int y2) { 
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
            set_dot(i, y1, COLOR_GREEN);
            grid[i][y1] = 1; 
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
            set_dot(x1, i, COLOR_GREEN);
            grid[x1][i] = 1; 
        }
    }
}

void DrawShip(int x1_idx, int y1_idx, int x2_idx, int y2_idx, Color color) {
    int x1_start, y1_start, x1_end, y1_end;
    int x2_start, y2_start, x2_end, y2_end;
    get_dot_bbox(y1_idx, x1_idx, &x1_start, &y1_start, &x1_end, &y1_end);
    get_dot_bbox(y2_idx, x2_idx, &x2_start, &y2_start, &x2_end, &y2_end);

    int x_left = (x1_start < x2_start) ? x1_start : x2_start;
    int x_right = (x1_end > x2_end) ? x1_end : x2_end;
    int y_top = (y1_start < y2_start) ? y1_start : y2_start;
    int y_bottom = (y1_end > y2_end) ? y1_end : y2_end;

    for (int i = 2; i <= 4; i++) {
        LCD_DrawLine(x_left - i, y_top - i, x_left - i, y_bottom + i, color);   
        LCD_DrawLine(x_right + i, y_top - i, x_right + i, y_bottom + i, color); 
        LCD_DrawLine(x_left - i, y_top - i, x_right + i, y_top - i, color);     
        LCD_DrawLine(x_left - i, y_bottom + i, x_right + i, y_bottom + i, color); 
    }
}

void draw_B(int x_start, int y_start, int letter_height, int letter_width, Color color) {
    LCD_DrawFillRectangle(x_start, y_start, x_start + 5, y_start + letter_height, color);
    LCD_DrawFillRectangle(x_start + 5, y_start, x_start + 15, y_start + 5, color);      
    LCD_DrawFillRectangle(x_start + 5, y_start + 12, x_start + 15, y_start + 17, color); 
    LCD_DrawFillRectangle(x_start + 5, y_start + 25, x_start + 15, y_start + 30, color); 
    LCD_DrawFillRectangle(x_start + 15, y_start + 5, x_start + 20, y_start + 12, color); 
    LCD_DrawFillRectangle(x_start + 15, y_start + 17, x_start + 20, y_start + 25, color);
}

void draw_A(int x_start, int y_start, int letter_height, int letter_width, Color color) {
    LCD_DrawFillRectangle(x_start, y_start, x_start + 5, y_start + 30, color);
    LCD_DrawFillRectangle(x_start + 15, y_start, x_start + 20, y_start + 30, color);
    LCD_DrawFillRectangle(x_start + 5, y_start, x_start + 15, y_start + 5, color);
    LCD_DrawFillRectangle(x_start + 5, y_start + 15, x_start + 15, y_start + 20, color);
}

void draw_T(int x_start, int y_start, int letter_height, int letter_width, Color color) {
    LCD_DrawFillRectangle(x_start, y_start, x_start + 20, y_start + 5, color);
    LCD_DrawFillRectangle(x_start + 7, y_start, x_start + 13, y_start + 30, color);
}

void draw_L(int x_start, int y_start, int letter_height, int letter_width, Color color) {
    LCD_DrawFillRectangle(x_start, y_start, x_start + 5, y_start + 30, color);
    LCD_DrawFillRectangle(x_start, y_start + 25, x_start + 20, y_start + 30, color);
}

void draw_E(int x_start, int y_start, int letter_height, int letter_width, Color color) {
    LCD_DrawFillRectangle(x_start, y_start, x_start + 5, y_start + 30, color);
    LCD_DrawFillRectangle(x_start, y_start, x_start + 20, y_start + 5, color);
    LCD_DrawFillRectangle(x_start, y_start + 15, x_start + 15, y_start + 20, color);
    LCD_DrawFillRectangle(x_start, y_start + 25, x_start + 20, y_start + 30, color);
}

void draw_O(int x_start, int y_start, int letter_height, int letter_width, Color color) {
    LCD_DrawFillRectangle(x_start, y_start, x_start + 5, y_start + 30, color);
    LCD_DrawFillRectangle(x_start + 15, y_start, x_start + 20, y_start + 30, color);
    LCD_DrawFillRectangle(x_start + 5, y_start, x_start + 15, y_start + 5, color);
    LCD_DrawFillRectangle(x_start + 5, y_start + 25, x_start + 15, y_start + 30, color);
}

void draw_S(int x_start, int y_start, int letter_height, int letter_width, Color color) {
    LCD_DrawFillRectangle(x_start, y_start, x_start + 20, y_start + 5, color);
    LCD_DrawFillRectangle(x_start, y_start, x_start + 5, y_start + 15, color);
    LCD_DrawFillRectangle(x_start, y_start + 12, x_start + 20, y_start + 17, color);
    LCD_DrawFillRectangle(x_start + 15, y_start + 15, x_start + 20, y_start + 30, color);
    LCD_DrawFillRectangle(x_start, y_start + 25, x_start + 20, y_start + 30, color);
}

void draw_R(int x_start, int y_start, int letter_height, int letter_width, Color color) {
    LCD_DrawFillRectangle(x_start, y_start, x_start + 5, y_start + 30, color);
    LCD_DrawFillRectangle(x_start + 5, y_start, x_start + 15, y_start + 5, color);
    LCD_DrawFillRectangle(x_start + 5, y_start + 12, x_start + 15, y_start + 17, color);
    LCD_DrawFillRectangle(x_start + 15, y_start + 5, x_start + 20, y_start + 12, color);
    LCD_DrawFillRectangle(x_start + 10, y_start + 17, x_start + 15, y_start + 30, color);
}

void draw_W(int x_start, int y_start, int letter_height, int letter_width, Color color) {
    int line_thickness = letter_width / 5;  
    int mid_gap = letter_width / 4;       
    LCD_DrawFillRectangle(x_start, y_start, 
                          x_start + line_thickness, y_start + letter_height, color);
    LCD_DrawFillRectangle(x_start + letter_width - line_thickness, y_start, 
                          x_start + letter_width, y_start + letter_height, color);
    LCD_DrawFillRectangle(x_start + line_thickness + mid_gap, y_start + letter_height / 2, 
                          x_start + letter_width - line_thickness - mid_gap, y_start + letter_height, color);
    LCD_DrawFillRectangle(x_start, y_start + letter_height - line_thickness, 
                          x_start + letter_width, y_start + letter_height, color);
}

void draw_I(int x_start, int y_start, int letter_height, int letter_width, Color color) {
    int line_thickness = letter_width / 5; 
    LCD_DrawFillRectangle(x_start, y_start, 
                          x_start + letter_width, y_start + line_thickness, color);
    LCD_DrawFillRectangle(x_start + (letter_width / 2) - (line_thickness / 2), y_start, 
                          x_start + (letter_width / 2) + (line_thickness / 2), y_start + letter_height, color);
    LCD_DrawFillRectangle(x_start, y_start + letter_height - line_thickness, 
                          x_start + letter_width, y_start + letter_height, color);
}

void draw_N(int x_start, int y_start, int letter_height, int letter_width, Color color) {
    int line_thickness = letter_width / 10;  
    LCD_DrawFillRectangle(x_start, y_start, 
                          x_start + line_thickness, y_start + letter_height, color);
    LCD_DrawFillRectangle(x_start + letter_width - line_thickness, y_start, 
                          x_start + letter_width, y_start + letter_height, color);
    LCD_DrawLine(x_start + line_thickness, y_start, 
                 x_start + letter_width - line_thickness, y_start + letter_height, COLOR_BLACK);
    LCD_DrawLine(x_start + line_thickness + 1, y_start + 1, 
            x_start + letter_width - line_thickness + 1, y_start + letter_height +1, COLOR_BLACK);    
    LCD_DrawLine(x_start + line_thickness - 1, y_start - 1, 
            x_start + letter_width - line_thickness - 1, y_start + letter_height - 1, COLOR_BLACK);           
}

void draw_exclamation_point(int x_start, int y_start, int letter_height, int letter_width, Color color) {
    int line_thickness = letter_width / 5;  
    int dot_height = letter_height / 5;    
    y_start -= 2; 
    LCD_DrawFillRectangle(x_start + (letter_width / 2) - (line_thickness / 2), y_start, 
                          x_start + (letter_width / 2) + (line_thickness / 2), y_start + letter_height - dot_height, color);
    y_start += 3;
    LCD_DrawFillRectangle(x_start + (letter_width / 2) - (line_thickness / 2), y_start + letter_height - dot_height, 
                          x_start + (letter_width / 2) + (line_thickness / 2), y_start + letter_height, color);
}


void draw_star(int x_center, int y_center, Color color) { 
    LCD_DrawLine(x_center - 10, y_center - 10, x_center + 10, y_center + 10, color);
    LCD_DrawLine(x_center + 10, y_center - 10, x_center - 10, y_center + 10, color);
    LCD_DrawLine(x_center, y_center - 10, x_center, y_center + 10, color);
    LCD_DrawLine(x_center - 10, y_center, x_center + 10, y_center, color);
}

void LCD_WaitScreen() { 
    int x_start = 60; 
    int y_start = 140; 
    int letter_width = 20; 
    int letter_height = 30; 
    int space = 10;
    Color color = COLOR_BLACK; 

    draw_W(x_start, y_start, letter_height, letter_width, color); 
    x_start += letter_width + space; 
    draw_A(x_start, y_start, letter_height, letter_width, color); 
    x_start += letter_width + space; 
    draw_I(x_start, y_start, letter_height, letter_width, color); 
    x_start += letter_width + space; 
    draw_T(x_start, y_start, letter_height, letter_width, color); 
    LCD_DrawFillRectangle(55, y_start + letter_height + 20, x_start + letter_width + 3, y_start + letter_height + 20 + 5, color);
}

void draw_digit(int x_start, int y_start, int digit, int segment_length, int segment_thickness, Color color) {
    // Error handling for invalid digits
    if (digit < 0 || digit > 9) return;

    // Define 7-segment patterns for digits 0-9
    const uint8_t segment_map[10] = {
        0b0111111, // "0" - a, b, c, d, e, f
        0b0000110, // "1" - b, c
        0b1011011, // "2" - a, b, d, e, g
        0b1001111, // "3" - a, b, c, d, g
        0b1100110, // "4" - b, c, f, g
        0b1101101, // "5" - a, c, d, f, g
        0b1111101, // "6" - a, c, d, e, f, g
        0b0000111, // "7" - a, b, c
        0b1111111, // "8" - a, b, c, d, e, f, g
        0b1101111  // "9" - a, b, c, d, f, g
    };

    // Extract the segments for the given digit
    uint8_t segments = segment_map[digit];

    // Define segment positions and sizes relative to the starting position
    int a_x1 = x_start + segment_thickness;             // Top horizontal segment (a)
    int a_y1 = y_start;
    int a_x2 = x_start + segment_length + segment_thickness;
    int a_y2 = y_start + segment_thickness;

    int b_x1 = x_start + segment_length;                // Top-right vertical segment (b)
    int b_y1 = y_start + segment_thickness;
    int b_x2 = b_x1 + segment_thickness;
    int b_y2 = b_y1 + segment_length;

    int c_x1 = x_start + segment_length;                // Bottom-right vertical segment (c)
    int c_y1 = b_y2;
    int c_x2 = c_x1 + segment_thickness;
    int c_y2 = c_y1 + segment_length;

    int d_x1 = a_x1;                                    // Bottom horizontal segment (d)
    int d_y1 = c_y2;
    int d_x2 = a_x2;
    int d_y2 = d_y1 + segment_thickness;

    int e_x1 = x_start;                                 // Bottom-left vertical segment (e)
    int e_y1 = c_y1;
    int e_x2 = e_x1 + segment_thickness;
    int e_y2 = e_y1 + segment_length;

    int f_x1 = x_start;                                 // Top-left vertical segment (f)
    int f_y1 = b_y1;
    int f_x2 = f_x1 + segment_thickness;
    int f_y2 = f_y1 + segment_length;

    int g_x1 = a_x1;                                    // Middle horizontal segment (g)
    int g_y1 = f_y2;
    int g_x2 = a_x2;
    int g_y2 = g_y1 + segment_thickness;

    // Draw each segment if it's active
    if (segments & 0b0000001) LCD_DrawFillRectangle(a_x1, a_y1, a_x2, a_y2, color); // a
    if (segments & 0b0000010) LCD_DrawFillRectangle(b_x1, b_y1, b_x2, b_y2, color); // b
    if (segments & 0b0000100) LCD_DrawFillRectangle(c_x1, c_y1, c_x2, c_y2, color); // c
    if (segments & 0b0001000) LCD_DrawFillRectangle(d_x1, d_y1, d_x2, d_y2, color); // d
    if (segments & 0b0010000) LCD_DrawFillRectangle(e_x1, e_y1, e_x2, e_y2, color); // e
    if (segments & 0b0100000) LCD_DrawFillRectangle(f_x1, f_y1, f_x2, f_y2, color); // f
    if (segments & 0b1000000) LCD_DrawFillRectangle(g_x1, g_y1, g_x2, g_y2, color); // g
}



void LCD_StartScreen() { 
    int x_start = 60; 
    int y_start = 60; 
    int letter_width = 20; 
    int letter_height = 30; 
    int space = 10;
    Color color = COLOR_BLACK; 

    draw_E(x_start, y_start, letter_height, letter_width, color); 
    x_start += letter_width + space; 
    draw_N(x_start, y_start, letter_height, letter_width, color); 
    x_start += letter_width + space; 
    draw_O(x_start, y_start, letter_height, letter_width, color); 
    x_start += letter_width + space; 
    draw_exclamation_point(x_start, y_start, letter_height, letter_width, color); 
    LCD_DrawFillRectangle(55, y_start + letter_height + 20, x_start + letter_width + 3, y_start + letter_height + 20 + 5, color);
    
    x_start = 120; 
    y_start += letter_height + 40; 
    draw_digit(x_start, y_start, 8, letter_height, letter_width, color);
    x_start += 40;
    draw_digit(x_start, y_start, 2, letter_height, letter_width, color); 

    x_start = 120; 
    y_start += letter_height + 10; 
    draw_digit(x_start, y_start, 0, letter_height, letter_width, color); 
    x_start += 40;
    draw_digit(x_start, y_start, 1, letter_height, letter_width, color); 
}

void LCD_EndScreen() {
    int x_start = 50;   
    int y_start = 60;   
    int letter_width = 20;
    int letter_height = 30;
    int space = 5;      
    Color color = COLOR_BLACK; 

    draw_B(x_start, y_start, letter_height, letter_width, color); 
    x_start += letter_width + space;
    // A
    draw_A(x_start, y_start, letter_height, letter_width, color); 
    x_start += letter_width + space;
    // T
    draw_T(x_start, y_start, letter_height, letter_width, color); 
    x_start += letter_width + space;
    // T
    draw_T(x_start, y_start, letter_height, letter_width, color); 
    x_start += letter_width + space;
    // L
    draw_L(x_start, y_start, letter_height, letter_width, color); 
    x_start += letter_width + space;
    // E
    draw_E(x_start, y_start, letter_height, letter_width, color); 
    x_start += letter_width + space;

    y_start += letter_height + space;
    x_start = 75; 
    // B
    draw_B(x_start, y_start, letter_height, letter_width, color); 
    x_start += letter_width + space;
    // o
    draw_O(x_start, y_start, letter_height, letter_width, color); 
    x_start += letter_width + space;
    // T
    draw_T(x_start, y_start, letter_height, letter_width, color); 
    x_start += letter_width + space;
    // s
    draw_S(x_start, y_start, letter_height, letter_width, color); 
    x_start += letter_width + space;

    y_start += letter_height *3;
    x_start = 120; 
    draw_star(x_start + 5, y_start + 2, 0);
    y_start += letter_height;
    x_start = 4; 
    x_start += letter_width + space;
    draw_T(x_start, y_start, letter_height, letter_width, color); 
    x_start += letter_width + space;
    draw_O(x_start, y_start, letter_height, letter_width, color); 
    x_start += letter_width + letter_width;

    draw_S(x_start, y_start, letter_height, letter_width, color); 
    x_start += letter_width + space;
    draw_T(x_start, y_start, letter_height, letter_width, color); 
    x_start += letter_width + space;
    draw_A(x_start, y_start, letter_height, letter_width, color); 
    x_start += letter_width + space;
    draw_R(x_start, y_start, letter_height, letter_width, color); 
    x_start += letter_width + space;
    draw_T(x_start, y_start, letter_height, letter_width, color); 
}


