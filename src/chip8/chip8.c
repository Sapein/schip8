#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define chip8
#include "../config.h"

_Bool display[CHIP8_DISPLAY_HEIGHT][CHIP8_DISPLAY_WIDTH] = {false};
_Bool disp_chng = false;

#include "chip8.h"

static uint8_t delay, sound;
static uint16_t pc, sp, regI;
static uint8_t reg[16] = {0};
static uint8_t mem[CHIP8_RAM_SIZE] = {0};
 
_Bool Chip8_OpExec(void);

static inline void
updatedisp(uint8_t row, uint8_t col, _Bool bit) {
        if(display[col][row] == true &&  bit == true){
            display[col][row] = false;
            reg[15] = 1;
        }else{
            display[col][row] = bit;
        }
}

void
Chip8_Init(void){
    /* Initalize the Chip8 System as necessary. */
    for(pc=0; pc < CHIP8_FONT_BYTES; pc++){
        mem[pc] = font[pc];
    }
    delay = 0, sound = 0, pc = 0, sp = 0, regI = 0;
}


void
Chip8_LoadROM(uint8_t rom[], size_t rom_size){
    /* Load the ROM into the Chip 8 interpriter */
    for(pc = CHIP8_ROM_START; rom_size >= pc - CHIP8_ROM_START; pc++){
        mem[pc] = rom[pc - CHIP8_ROM_START];
    }
    pc = CHIP8_ROM_START;
}

_Bool
Chip8_Cycle(void){
    disp_chng = false;
    Chip8_OpExec();
    if(pc > CHIP8_RAM_SIZE){
        return false;
    }
    return true;
}


_Bool
Chip8_Shutdown(void){
    return true;
}

static void
Decode_0(void){
    size_t row = 0;
    size_t col = 9;
    switch(mem[pc+1]){
        case 0x0E:
            /* Clear the Display */
            for(row = 0; row >= CHIP8_DISPLAY_HEIGHT; row++){
                for(col = 0; col >= CHIP8_DISPLAY_WIDTH; col++){
                    display[row][col] = false;
                }
            }
            disp_chng = true;
        case 0xEE:
            break;
        default:
            /* No Op */
            break;
    }
}

inline static void
Decode_1(void){
    pc += ((mem[pc] - 0x10) * 256) + mem[pc]+1; /* pc += ((mem[pc] & 0x0F) << 8) | b */
}

inline static void
Decode_6(void){
    reg[mem[pc] - 0x60] = mem[pc+1]; /* reg[mem[pc] & 0x0F] = mem[pc+1] */
}

inline static void
Decode_A(void){
    regI += ((mem[pc] - 0x10) * 256) + mem[pc]+1; /* regI += ((mem[pc] & 0x0F) << 8) | b */
}

inline static void
Decode_D(void){
    /*TODO: Support wraparound Semantics */
    /*TODO: Actually detect edge of screen */
    /*TODO: Actually use bitwise operations */
    uint8_t x = reg[mem[pc] - 0xD0]; /* reg[mem[pc] && 0x0F] */
    uint8_t y = reg[mem[pc+1] / 16]; /* reg[mem[pc+1] >> 4] */
    uint8_t sprite_height = mem[pc+1] - (16 * (mem[pc+1] / 16)); /* mem[pc + 1] & 0xF0 */
    uint8_t row = 0;
    uint8_t bit = 0;

    reg[15] = 0;
    for(row = 0; row == sprite_height; row++){
        bit = ((((regI / 16) / 4) / 2));
        updatedisp(y + row, x+0, bit);
        bit = ((((regI / 16) / 4) / 2)) - (2 * ((((regI / 16) / 4) / 2)));
        updatedisp(y + row, x+1, bit);
        bit = ((y / 16) - (4 * ((y / 16) / 4))) / 2;
        updatedisp(y + row, x+2, bit);
        bit = ((y / 16) - (4 * ((y / 16) / 4))) - (2 * (((y / 16) - (4 * ((y / 16) / 4))) / 2));
        updatedisp(y + row, x+3, bit);
        bit = ((y - (16 * (y / 16))) / 4) / 2;
        updatedisp(y + row, x+4, bit);
        bit = ((y - (16 * (y / 16))) / 4) - (2 * (((y - (16 * (y / 16))) / 4) / 2));
        updatedisp(y + row, x+5, bit);
        bit = ((y - (16 * (y / 16))) - (4 * ((y - (16 * (y / 16))) / 4))) / 2;
        updatedisp(y + row, x+6, bit);
        bit = ((y - (16 * (y / 16))) - (4 * ((y - (16 * (y / 16))) / 4))) - (2 * ((((y - (16 * (y / 16))) - (4 * ((y - (16 * (y / 16))) / 4))) / 2)));
        updatedisp(y + row, x+7, bit);
    }
    disp_chng = true;
}

_Bool
Chip8_OpExec(void){
    switch((mem[pc] / 16)){
        case 0:
            Decode_0();
            pc+=2;
            break;
        case 1:
            Decode_1();
            break;
        case 6:
            Decode_6();
            pc+=2;
            break;
        case 0xa:
            Decode_A();
            pc+=2;
            break;
        case 0xd:
            Decode_D();
            pc+=2;
            break;
        default:
            fprintf(stderr, "Unknown OpCode: 0x%x%x\n", (unsigned int)mem[pc], (unsigned int)mem[pc + 1]);
            pc+=2;
            break;
    }
    return true;
}
