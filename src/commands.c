
#include "stm32f0xx.h"
#include "ff.h"
#include "lcd.h"
#include "tty.h"
#include "commands.h"
#include <string.h>
#include <stdio.h>

// Data structure for the mounted file system.
FATFS fs_storage;

typedef union {
    struct {
        unsigned int bisecond:5; // seconds divided by 2
        unsigned int minute:6;
        unsigned int hour:5;
        unsigned int day:5;
        unsigned int month:4;
        unsigned int year:7;
    };
} fattime_t;

// Current time in the FAT file system format.
static fattime_t fattime;

#define RED 0xFF0000
#define BLUE 0x0000FF
#define BACKGROUND 0xFFFFFF
#define BLACK 0x000000
#define GREEN 0x00FF00

void set_fattime(int year, int month, int day, int hour, int minute, int second)
{
    fattime_t newtime;
    newtime.year = year - 1980;
    newtime.month = month;
    newtime.day = day;
    newtime.hour = hour;
    newtime.minute = minute;
    newtime.bisecond = second/2;
    int len = sizeof newtime;
    memcpy(&fattime, &newtime, len);
}

void advance_fattime(void)
{
    fattime_t newtime = fattime;
    newtime.bisecond += 1;
    if (newtime.bisecond == 30) {
        newtime.bisecond = 0;
        newtime.minute += 1;
    }
    if (newtime.minute == 60) {
        newtime.minute = 0;
        newtime.hour += 1;
    }
    if (newtime.hour == 24) {
        newtime.hour = 0;
        newtime.day += 1;
    }
    if (newtime.month == 2) {
        if (newtime.day >= 29) {
            int year = newtime.year + 1980;
            if ((year % 1000) == 0) { // we have a leap day in 2000
                if (newtime.day > 29) {
                    newtime.day -= 28;
                    newtime.month = 3;
                }
            } else if ((year % 100) == 0) { // no leap day in 2100
                if (newtime.day > 28)
                newtime.day -= 27;
                newtime.month = 3;
            } else if ((year % 4) == 0) { // leap day for other mod 4 years
                if (newtime.day > 29) {
                    newtime.day -= 28;
                    newtime.month = 3;
                }
            }
        }
    } else if (newtime.month == 9 || newtime.month == 4 || newtime.month == 6 || newtime.month == 10) {
        if (newtime.day == 31) {
            newtime.day -= 30;
            newtime.month += 1;
        }
    } else {
        if (newtime.day == 0) { // cannot advance to 32
            newtime.day = 1;
            newtime.month += 1;
        }
    }
    if (newtime.month == 13) {
        newtime.month = 1;
        newtime.year += 1;
    }

    fattime = newtime;
}

uint32_t get_fattime(void)
{
    union FattimeUnion {
        fattime_t time;
        uint32_t value;
    };

    union FattimeUnion u;
    u.time = fattime;
    return u.value;
}



void print_error(FRESULT fr, const char *msg)
{
    const char *errs[] = {
            [FR_OK] = "Success",
            [FR_DISK_ERR] = "Hard error in low-level disk I/O layer",
            [FR_INT_ERR] = "Assertion failed",
            [FR_NOT_READY] = "Physical drive cannot work",
            [FR_NO_FILE] = "File not found",
            [FR_NO_PATH] = "Path not found",
            [FR_INVALID_NAME] = "Path name format invalid",
            [FR_DENIED] = "Permision denied",
            [FR_EXIST] = "Prohibited access",
            [FR_INVALID_OBJECT] = "File or directory object invalid",
            [FR_WRITE_PROTECTED] = "Physical drive is write-protected",
            [FR_INVALID_DRIVE] = "Logical drive number is invalid",
            [FR_NOT_ENABLED] = "Volume has no work area",
            [FR_NO_FILESYSTEM] = "Not a valid FAT volume",
            [FR_MKFS_ABORTED] = "f_mkfs aborted",
            [FR_TIMEOUT] = "Unable to obtain grant for object",
            [FR_LOCKED] = "File locked",
            [FR_NOT_ENOUGH_CORE] = "File name is too large",
            [FR_TOO_MANY_OPEN_FILES] = "Too many open files",
            [FR_INVALID_PARAMETER] = "Invalid parameter",
    };
    if (fr < 0 || fr >= sizeof errs / sizeof errs[0])
        printf("%s: Invalid error\n", msg);
    else
        printf("%s: %s\n", msg, errs[fr]);
}

void append(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Specify only one file name to append to.");
        return;
    }
    FIL fil;        /* File object */
    char line[100]; /* Line buffer */
    FRESULT fr;     /* FatFs return code */
    fr = f_open(&fil, argv[1], FA_WRITE|FA_OPEN_EXISTING|FA_OPEN_APPEND);
    if (fr) {
        print_error(fr, argv[1]);
        return;
    }
    printf("To end append, enter a line with a single '.'\n");
    for(;;) {
        fgets(line, sizeof(line)-1, stdin);
        if (line[0] == '.' && line[1] == '\n')
            break;
        int len = strlen(line);
        if (line[len-1] == '\004')
            len -= 1;
        UINT wlen;
        fr = f_write(&fil, (BYTE*)line, len, &wlen);
        if (fr)
            print_error(fr, argv[1]);
    }
    f_close(&fil);
}

void cat(int argc, char *argv[])
{
    for(int i=1; i<argc; i++) {
        FIL fil;        /* File object */
        char line[100]; /* Line buffer */
        FRESULT fr;     /* FatFs return code */

        /* Open a text file */
        fr = f_open(&fil, argv[i], FA_READ);
        if (fr) {
            print_error(fr, argv[i]);
            return;
        }

        /* Read every line and display it */
        while(f_gets(line, sizeof line, &fil))
            printf(line);
        /* Close the file */
        f_close(&fil);
    }
}

void cd(int argc, char *argv[])
{
    if (argc > 2) {
        printf("Too many arguments.");
        return;
    }
    FRESULT res;
    if (argc == 1) {
        res = f_chdir("/");
        if (res)
            print_error(res, "(default path)");
        return;
    }
    res = f_chdir(argv[1]);
    if (res)
        print_error(res, argv[1]);
}

int to_int(char *start, char *end, int base)
{
    int n = 0;
    for( ; start != end; start++)
        n = n * base + (*start - '0');
    return n;
}

static const char *month_name[] = {
        [1] = "Jan",
        [2] = "Feb",
        [3] = "Mar",
        [4] = "Apr",
        [5] = "May",
        [6] = "Jun",
        [7] = "Jul",
        [8] = "Aug",
        [9] = "Sep",
        [10] = "Oct",
        [11] = "Nov",
        [12] = "Dec",
};

void set_fattime(int,int,int,int,int,int);
void date(int argc, char *argv[])
{
    if (argc == 2) {
        char *d = argv[1];
        if (strlen(d) != 14) {
            printf("Date format: YYYYMMDDHHMMSS\n");
            return;
        }
        for(int i=0; i<14; i++)
            if (d[i] < '0' || d[i] > '9') {
                printf("Date format: YYYMMDDHHMMSS\n");
                return;
            }
        int year = to_int(d, &d[4], 10);
        int month = to_int(&d[4], &d[6], 10);
        int day   = to_int(&d[6], &d[8], 10);
        int hour  = to_int(&d[8], &d[10], 10);
        int minute = to_int(&d[10], &d[12], 10);
        int second = to_int(&d[12], &d[14], 10);
        set_fattime(year, month, day, hour, minute, second);
        return;
    }
    int integer = get_fattime();
    union {
        int integer;
        fattime_t ft;
    } u;
    u.integer = integer;
    fattime_t ft = u.ft;
    int year = ft.year + 1980;
    int month = ft.month;
    printf("%d-%s-%02d %02d:%02d:%02d\n",
            year, month_name[month], ft.day, ft.hour, ft.minute, ft.bisecond*2);
}

void dino(int argc, char *argv[])
{
    const char str[] =
    "   .-~~^-.\n"
    " .'  O    \\\n"
    "(_____,    \\\n"
    " `----.     \\\n"
    "       \\     \\\n"
    "        \\     \\\n"
    "         \\     `.             _ _\n"
    "          \\       ~- _ _ - ~       ~ - .\n"
    "           \\                              ~-.\n"
    "            \\                                `.\n"
    "             \\    /               /       \\    \\\n"
    "              `. |         }     |         }    \\\n"
    "                `|        /      |        /       \\\n"
    "                 |       /       |       /          \\\n"
    "                 |      /`- _ _ _|      /.- ~ ^-.     \\\n"
    "                 |     /         |     /          `.    \\\n"
    "                 |     |         |     |             -.   ` . _ _ _ _ _ _\n"
    "                 |_____|         |_____|                ~ . _ _ _ _ _ _ _ >\n";
    puts(str);
}

void input(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Specify only one file name to create.");
        return;
    }
    FIL fil;        /* File object */
    char line[100]; /* Line buffer */
    FRESULT fr;     /* FatFs return code */
    fr = f_open(&fil, argv[1], FA_WRITE|FA_CREATE_NEW);
    if (fr) {
        print_error(fr, argv[1]);
        return;
    }
    printf("To end input, enter a line with a single '.'\n");
    for(;;) {
        fgets(line, sizeof(line)-1, stdin);
        if (line[0] == '.' && line[1] == '\n')
            break;
        int len = strlen(line);
        if (line[len-1] == '\004')
            len -= 1;
        UINT wlen;
        fr = f_write(&fil, (BYTE*)line, len, &wlen);
        if (fr)
            print_error(fr, argv[1]);
    }
    f_close(&fil);
}

void lcd_init(int argc, char *argv[])
{
    LCD_Setup();
}

void ls(int argc, char *argv[])
{
    FRESULT res;
    DIR dir;
    static FILINFO fno;
    const char *path = "";
    int info = 0;
    int i=1;
    do {
        if (argv[i][0] == '-') {
            for(char *c=&argv[i][1]; *c; c++)
                if (*c == 'l')
                    info=1;
            if (i+1 < argc) {
                i += 1;
                continue;
            }
        } else {
            path = argv[i];
        }

        res = f_opendir(&dir, path);                       /* Open the directory */
        if (res != FR_OK) {
            print_error(res, argv[1]);
            return;
        }
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (info) {
                printf("%04d-%s-%02d %02d:%02d:%02d %6ld %c%c%c%c%c ",
                        (fno.fdate >> 9) + 1980,
                        month_name[fno.fdate >> 5 & 15],
                        fno.fdate & 31,
                        fno.ftime >> 11,
                        fno.ftime >> 5 & 63,
                        (fno.ftime & 31) * 2,
                        fno.fsize,
                        (fno.fattrib & AM_DIR) ? 'D' : '-',
                        (fno.fattrib & AM_RDO) ? 'R' : '-',
                        (fno.fattrib & AM_HID) ? 'H' : '-',
                        (fno.fattrib & AM_SYS) ? 'S' : '-',
                        (fno.fattrib & AM_ARC) ? 'A' : '-');
            }
            if (path[0] != '\0')
                printf("%s/%s\n", path, fno.fname);
            else
                printf("%s\n", fno.fname);
        }
        f_closedir(&dir);
        i += 1;
    } while(i<argc);
}

void mkdir(int argc, char *argv[])
{
    for(int i=1; i<argc; i++) {
        FRESULT res = f_mkdir(argv[i]);
        if (res != FR_OK) {
            print_error(res, argv[i]);
            return;
        }
    }
}

void mount(int argc, char *argv[])
{
    FATFS *fs = &fs_storage;
    if (fs->id != 0) {
        print_error(FR_DISK_ERR, "Already mounted.");
        return;
    }
    int res = f_mount(fs, "", 1);
    if (res != FR_OK)
        print_error(res, "Error occurred while mounting");
}

void pwd(int argc, char *argv[])
{
    char line[100];
    FRESULT res = f_getcwd(line, sizeof line);
    if (res != FR_OK)
        print_error(res, "pwd");
    else
        printf("%s\n", line);
}

void rm(int argc, char *argv[])
{
    FRESULT res;
    for(int i=1; i<argc; i++) {
        res = f_unlink(argv[i]);
        if (res != FR_OK)
            print_error(res, argv[i]);
    }
}

void shout(int argc, char *argv[])
{
    char arr[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz0123456789\n";
    for(int i=0; i<1000; i++)
        for(int c=0; c<sizeof arr; c++)
            putchar(arr[c]);
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

void add(int argc, char *argv[])
{
  int sum = 0;
  for(int i=1; i < argc; i++) {
      sum += strtol(argv[i], 0, 0);
  }
  printf("The sum is %d\n", sum);
}

void mul(int argc, char *argv[])
{
  int prod = 1;
  for(int i=1; i < argc; i++) {
    prod *= strtol(argv[i], 0, 0);
  }
  printf("The product is %d\n", prod);
}

void draw_grid() { 
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

void set_dot() { 
    int x, y, color;
    x = 0; 
    y = 0;
    color = RED;   
    int x1, y1, x2, y2; 
    get_dot_bbox(x, y, &x1, &y1, &x2, &y2);
    LCD_DrawFillRectangle(x1, y1, x2, y2, 1445); 
}

void draw_dots() {
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

void draw_ship() {
    int x1_idx, y1_idx, x2_idx, y2_idx; 
    x1_idx = 0; 
    y1_idx = 0; 
    x2_idx = 0; 
    y2_idx = 1; 

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
        LCD_DrawLine(x_left - i, y_top - i, x_left - i, y_bottom + i, 0x0000);   
        LCD_DrawLine(x_right + i, y_top - i, x_right + i, y_bottom + i, 0x0000); 
        LCD_DrawLine(x_left - i, y_top - i, x_right + i, y_top - i, 0x0000);     
        LCD_DrawLine(x_left - i, y_bottom + i, x_right + i, y_bottom + i, 0x0000); 
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

void draw_start() {
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
        { "append", append },
        { "cat", cat },
        { "cd", cd },
        { "date", date },
        { "dino", dino },
        { "input", input },
        { "lcd_init", lcd_init },
        { "ls", ls },
        { "mkdir", mkdir },
        { "mount", mount },
        { "pwd", pwd },
        { "rm", rm },
        { "shout", shout },
        { "clear",    clear },
        { "clear_f",    clear_f },
        { "drawline", drawline },
        { "drawrect", drawrect },
        { "drawfillrect", drawfillrect },
        { "grid", draw_grid },
        { "dots", draw_dots },
        { "start", draw_start },
        { "set", set_dot },
        { "draw_ship", draw_ship }
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
