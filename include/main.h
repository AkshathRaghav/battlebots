#include "setup.h"
#include "commands.h"
#include "intercomm.h"
#include "i2c.h"
#include "game.h"
#include "lcd.h"
#include <stdint.h>
#include <stdio.h>

#define LOC 0x0

const char* team;
volatile int current_col;
extern int done_first; 
extern int turn_flag; 
extern int ship_hit_counter;
extern int cum_games_played; 

void SysTick_Handler();
int main(); 