#include <stdint.h>
#include <stdio.h>
#include "setup.h"

const char* team = "battlebots";

int main() {
    internal_clock();
    init_usart5();
    enable_tty_interrupt();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    command_shell();
}

