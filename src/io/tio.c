#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#include "../config.h"
#include "io.h"


struct termios original;

_Bool IO_Init(void){
    struct termios cbreak;
    tcgetattr(STDIN_FILENO, &original);

    tcgetattr(STDIN_FILENO, &cbreak);
    cbreak.c_lflag &= ~(ECHO | ICANON);
    cbreak.c_cc[VMIN] = 0;
    cbreak.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &cbreak);
    return true;
}

void shutdown(void){
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
}

_Bool IO_Shutdown(void){
    shutdown();
    return true;
}

_Bool IO_UpdateScreen(_Bool display[CHIP8_DISPLAY_HEIGHT][CHIP8_DISPLAY_WIDTH]){
    int row = 0;
    int col = 0;
    for(col = 0; col < CHIP8_DISPLAY_WIDTH + 2; col++){
        printf("+");
    }
    printf("\n");
    for(row = 0; row < CHIP8_DISPLAY_HEIGHT; row++){
        printf("+");
        for(col = 0; col < CHIP8_DISPLAY_WIDTH; col++){
            if(display[row][col]){
                printf("*");
            }else{
                printf(" ");
            }
        }
        printf("+\n");
    }
    for(col = 0; col < CHIP8_DISPLAY_WIDTH + 2; col++){
        printf("+");
    }
    printf("\n");
    return true;
}

uint8_t IO_GetKeyPress(void){
    struct timeval tmout = {.tv_sec = 0, .tv_usec = 1};
    signed int input = 0;
    fd_set fdread;
    FD_ZERO(&fdread);
    FD_SET(STDIN_FILENO, &fdread);
    if((input = select(1, &fdread, NULL, NULL, &tmout)) > 0){
        return 0x10;
    }else{
        input = getchar();
        if(input == EOF || input > 0xF){
            input = 0x10;
        }
    }
    return input;
}
