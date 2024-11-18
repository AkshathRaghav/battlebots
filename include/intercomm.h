#include "stm32f0xx.h"

extern int done_first; 

void send_data(uint8_t msg);
uint8_t read_data();

// Another grid needed. Has to track the attempts: Whether it was a hit or miss 
// not attempted, missed, and hit 

// Function for moving single dot. 1x1 size 
// Variables to keep track of movement. Cursor x, y.

// Rename the ship movement functions to Game_Mvxx_Ship 
// Make 4 new functions to just update cursor x, y. 

// # to confirm

// Data Packet: Y (4bits), X (4bits)
