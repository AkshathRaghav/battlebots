#ifndef __COMMANDS_H__
#define __COMMANDS_H__

typedef enum {
    COLOR_BLACK   = 0x0000,  // RGB(0, 0, 0)
    COLOR_WHITE   = 0xFFFF,  // RGB(31, 63, 31)
    COLOR_RED     = 0xF800,  // RGB(31, 0, 0)
    COLOR_GREEN   = 0x07E0,  // RGB(0, 63, 0)
    COLOR_BLUE    = 0x001F,  // RGB(0, 0, 31)
    COLOR_YELLOW  = 0xFFE0,  // RGB(31, 63, 0)
    COLOR_CYAN    = 0x07FF,  // RGB(0, 63, 31)
    COLOR_MAGENTA = 0xF81F,  // RGB(31, 0, 31)
    COLOR_ORANGE  = 0xFD20,  // RGB(31, 39, 0)
    COLOR_PURPLE  = 0x780F,  // RGB(15, 0, 15)
    COLOR_PINK    = 0xF81F,  // RGB(31, 0, 31)
    COLOR_BROWN   = 0x79E0,  // RGB(15, 31, 0)
    COLOR_GRAY    = 0x8410,  // RGB(16, 32, 16)
    COLOR_LIGHTGRAY = 0xC618  // RGB(25, 50, 25)
} Color;

struct commands_t {
    const char *cmd;
    void      (*fn)(int argc, char *argv[]);
};

void command_shell(void);




#endif /* __COMMANDS_H_ */
