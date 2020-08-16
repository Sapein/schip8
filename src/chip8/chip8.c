#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "chip8.h"
#include "../config.h"

static uint8_t delay, sound;
static uint16_t pc, sp, regI;
static uint8_t reg[16] = {0};
static uint8_t mem[CHIP8_RAM_SIZE] = {0};
_Bool display[CHIP8_DISPLAY_HEIGHT][CHIP8_DISPLAY_WIDTH] = {false};

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
    pc += 2;
    Chip8_OpExec();
    return true;
}


_Bool
Chip8_OpExec(void){
    return true;
};

_Bool
Chip8_Shutdown(void){
    return true;
}

void
Decode_0(void){
    switch(mem[pc+1]){
        case 0x0E:
            /* Clear the Display */
            0:
            break;
        case 0xEE:
            break;
        default:
            /* No Op */
            break
    }
}

inline void
Decode_1(void){
    pc = (mem[pc] && 0x0F) * 256; /*(mem[pc] && 0x0F) << 4*/
    pc += mem[pc+1];
}

inline void
Decode_6(void){
    reg[mem[pc] && 0x0F] = mem[pc+1];
}

inline void
Decode_A(void){
    regI = (mem[pc] && 0x0F) * 256; /*(mem[pc] && 0x0F) << 4*/
    regI += (mem[pc+1]);
}

inline void
Decode_D(void){
    ; /* Currently a no-op, put the actual code here */
}
