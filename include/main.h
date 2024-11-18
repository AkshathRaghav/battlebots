#include "setup.h"
#include "commands.h"
#include "intercomm.h"
#include "game.h"
#include "lcd.h"
#include <stdint.h>
#include <stdio.h>

const char* team;
volatile int current_col;
extern int done_first; 
extern int turn_flag; 
extern int ship_hit_counter;

void SysTick_Handler();
int main(); 