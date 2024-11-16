
#include "stm32f0xx.h"
#include "ff.h"
#include "lcd.h"
#include "tty.h"
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
            LCD_DrawFillRectangle(x1, y1, x2, y2, 0000); 
        }
    }
}

void LCD_DrawShip(int x1_idx, int y1_idx, int x2_idx, int y2_idx, Color color) {
    int dot_count = 0;
    
    if (x1_idx == x2_idx) {
        dot_count = abs(y2_idx - y1_idx) + 1;
    } else if (y1_idx == y2_idx) { 
        dot_count = abs(x2_idx - x1_idx) + 1;
    }
    
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

void draw_B(x_start, y_start, letter_height, letter_width, color) { 
    LCD_DrawFillRectangle(x_start, y_start, x_start + 5, y_start + letter_height, color); // Left vertical bar
    LCD_DrawFillRectangle(x_start + 5, y_start, x_start + 15, y_start + 5, color);       // Top horizontal bar
    LCD_DrawFillRectangle(x_start + 5, y_start + 12, x_start + 15, y_start + 17, color); // Middle horizontal bar
    LCD_DrawFillRectangle(x_start + 5, y_start + 25, x_start + 15, y_start + 30, color); // Bottom horizontal bar
    LCD_DrawFillRectangle(x_start + 15, y_start + 5, x_start + 20, y_start + 12, color); // Top right vertical bar
    LCD_DrawFillRectangle(x_start + 15, y_start + 17, x_start + 20, y_start + 25, color);// Bottom right vertical bar
}

void draw_A(x_start, y_start, letter_height, letter_width, color) { 
    LCD_DrawFillRectangle(x_start, y_start, x_start + 5, y_start + 30, color);
    LCD_DrawFillRectangle(x_start + 15, y_start, x_start + 20, y_start + 30, color);
    LCD_DrawFillRectangle(x_start + 5, y_start, x_start + 15, y_start + 5, color);
    LCD_DrawFillRectangle(x_start + 5, y_start + 15, x_start + 15, y_start + 20, color);
}

void draw_T(x_start, y_start, letter_height, letter_width, color) { 
    LCD_DrawFillRectangle(x_start, y_start, x_start + 20, y_start + 5, color);
    LCD_DrawFillRectangle(x_start + 7, y_start, x_start + 13, y_start + 30, color);
}

void draw_L(x_start, y_start, letter_height, letter_width, color) { 
    LCD_DrawFillRectangle(x_start, y_start, x_start + 5, y_start + 30, color);
    LCD_DrawFillRectangle(x_start, y_start + 25, x_start + 20, y_start + 30, color);
}

void draw_E(x_start, y_start, letter_height, letter_width, color) { 
    LCD_DrawFillRectangle(x_start, y_start, x_start + 5, y_start + 30, color);
    LCD_DrawFillRectangle(x_start, y_start, x_start + 20, y_start + 5, color);
    LCD_DrawFillRectangle(x_start, y_start + 15, x_start + 15, y_start + 20, color);
    LCD_DrawFillRectangle(x_start, y_start + 25, x_start + 20, y_start + 30, color);
}

void draw_O(x_start, y_start, letter_height, letter_width, color) { 
    LCD_DrawFillRectangle(x_start, y_start, x_start + 5, y_start + 30, color);
    LCD_DrawFillRectangle(x_start + 15, y_start, x_start + 20, y_start + 30, color);
    LCD_DrawFillRectangle(x_start + 5, y_start, x_start + 15, y_start + 5, color);
    LCD_DrawFillRectangle(x_start + 5, y_start + 25, x_start + 15, y_start + 30, color);
}

void draw_S(x_start, y_start, letter_height, letter_width, color) { 
    LCD_DrawFillRectangle(x_start, y_start, x_start + 20, y_start + 5, color);
    LCD_DrawFillRectangle(x_start, y_start, x_start + 5, y_start + 15, color);
    LCD_DrawFillRectangle(x_start, y_start + 12, x_start + 20, y_start + 17, color);
    LCD_DrawFillRectangle(x_start + 15, y_start + 15, x_start + 20, y_start + 30, color);
    LCD_DrawFillRectangle(x_start, y_start + 25, x_start + 20, y_start + 30, color);
}

void draw_R(x_start, y_start, letter_height, letter_width, color) { 
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
    x_start = 5; 
    x_start += letter_width + space + space;
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


struct commands_t cmds[] = {
        { "lcd_init", lcd_init },
        { "clear",    clear },
        { "clear_f",    clear_f },
        { "drawline", drawline },
        { "drawrect", drawrect },
        { "drawfillrect", drawfillrect },
        { "grid", LCD_DrawGrid },
        { "dots", LCD_DrawCoords },
        { "start", LCD_StartScreen },
        { "set", set_dot },
};

// A weak definition that can be overridden by a better one.
// Replace this with your own usercmds entirely.
__attribute((weak)) struct commands_t usercmds[] = {
        { 0, 0 }
};

void exec(int argc, char *argv[])
{
    //for(int i=0; i<argc; i++)
    //    printf("%d: %s\n", i, argv[i]);
    for(int i=0; usercmds[i].cmd != 0; i++)
        if (strcmp(usercmds[i].cmd, argv[0]) == 0) {
            usercmds[i].fn(argc, argv);
            return;
        }
    for(int i=0; i<sizeof cmds/sizeof cmds[0]; i++)
        if (strcmp(cmds[i].cmd, argv[0]) == 0) {
            cmds[i].fn(argc, argv);
            return;
        }
    printf("%s: No such command.\n", argv[0]);
}

void parse_command(char *c)
{
    char *argv[20];
    int argc=0;
    int skipspace=1;
    for(; *c; c++) {
        if (skipspace) {
            if (*c != ' ' && *c != '\t') {
                argv[argc++] = c;
                skipspace = 0;
            }
        } else {
            if (*c == ' ' || *c == '\t') {
                *c = '\0';
                skipspace=1;
            }
        }
    }
    if (argc > 0) {
        argv[argc] = "";
        exec(argc, argv);
    }
}

void dots_loc_color(int x, int y, Color c){
    int x1, y1, x2, y2; 
    get_dot_bbox(x, y, &x1, &y1, &x2, &y2);
    LCD_DrawFillRectangle(x1, y1, x2, y2, c); 
}


static void insert_echo_string(const char *s)
{
    // This puts a string into the *input* stream, so it can be edited.
    while(*s)
        insert_echo_char(*s++);
}

void command_shell(void)
{
    // printf("\nEnter current "); fflush(stdout);
    // insert_echo_string("date 20240213000000");
    fflush(stdout);
    char line[100];
    line[99] = '\0';
    int len = strlen(line);
    if (line[len-1] == '\n')
        line[len-1] = '\0';

    puts("This is the STM32 command shell.");
    puts("Type 'mount' before trying any file system commands.");
    puts("Type 'lcd_init' before trying any draw commands.");

    parse_command("lcd_init");
    parse_command("start");

    fgets(line, 99, stdin);
    // parse_command(line);

    parse_command("clear_f");
    parse_command("grid");
    parse_command("dots");

    for(;;) {
        printf("> ");
        fgets(line, 99, stdin);
        line[99] = '\0';
        len = strlen(line);
        if (line[len-1] == '\n')
            line[len-1] = '\0';
        parse_command(line);
    }
}
