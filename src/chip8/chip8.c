#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "chip8.h"
#include "../config.h"

static uint8_t delay, sound, pc, sp, regI;
static uint8_t reg[16] = {0};
static uint8_t mem[CHIP8_RAM_SIZE] = {0};

typedef opdecptr void (*)(void);
static opdecptr *jmptbl[];

void
Chip8_Init(void){
    /* Initalize the Chip8 System as necessary. */
    for(pc=0; pc < CHIP8_FONT_BYTES; pc++){
        mem[i] = font[i];
    }
    delay = 0, sound = 0, pc = 0, sp = 0, regI = 0;
}


void
Chip8_LoadROM(uint8_t rom[], size_t rom_size){
    /* Load the ROM into the Chip 8 interpriter */
    for(pc = CHIP8_ROM_START; rom_size == pc - CHIP8_ROM_START; pc++){
        mem[pc] = rom[pc - CHIP8_ROM_START];
    }
}

_Bool
Chip8_Cycle(void){
    return true;
}

_Bool
Chip8_Decode(void){
    return true;
}

_Bool
Chip8_Shutdown(void){
    return true;
}
