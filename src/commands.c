
#include "stm32f0xx.h"
#include "lcd.h"
#include "commands.h"
#include <string.h>
#include <stdio.h>


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
    LCD_DrawFillRectangle(x_start, y_start, x_start + 5, y_start + letter_height, color); // Left vertical bar
    LCD_DrawFillRectangle(x_start + 5, y_start, x_start + 15, y_start + 5, color);       // Top horizontal bar
    LCD_DrawFillRectangle(x_start + 5, y_start + 12, x_start + 15, y_start + 17, color); // Middle horizontal bar
    LCD_DrawFillRectangle(x_start + 5, y_start + 25, x_start + 15, y_start + 30, color); // Bottom horizontal bar
    LCD_DrawFillRectangle(x_start + 15, y_start + 5, x_start + 20, y_start + 12, color); // Top right vertical bar
    LCD_DrawFillRectangle(x_start + 15, y_start + 17, x_start + 20, y_start + 25, color);// Bottom right vertical bar
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

void draw_star(x_center, y_center, color) { 
    LCD_DrawLine(x_center - 10, y_center - 10, x_center + 10, y_center + 10, color);
    LCD_DrawLine(x_center + 10, y_center - 10, x_center - 10, y_center + 10, color);
    LCD_DrawLine(x_center, y_center - 10, x_center, y_center + 10, color);
    LCD_DrawLine(x_center - 10, y_center, x_center + 10, y_center, color);
}

void LCD_StartScreen() {
    int x_start = 50;   // Starting x-coordinate
    int y_start = 60;   // Starting y-coordinate
    int letter_width = 20;
    int letter_height = 30;
    int space = 5;      // Space between letters
    int color = 0000;   // Color code for the text

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

void clear_f() { 
    LCD_Clear(1111);
}

