#include "setup.h"
#include "commands.h"
#include "game.h"
#include <stdint.h>
#include <stdio.h>

const char* team;
volatile int current_col;

void SysTick_Handler();
int main(); 