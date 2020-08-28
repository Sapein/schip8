#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define chip8
#include "../config.h"

#define BIT_1(byte) ((((byte / 16) / 4) / 2))
#define BIT_2(byte) ((((byte / 16) / 4) / 2)) - (2 * ((((byte / 16) / 4) / 2)))
#define BIT_3(byte) (((byte / 16) - (4 * ((byte / 16) / 4))) / 2)
#define BIT_4(byte) (((byte / 16) - (4 * ((byte / 16) / 4))) - (2 * (((byte / 16) - (4 * ((byte / 16) / 4))) / 2)))
#define BIT_5(byte) (((byte - (16 * (byte / 16))) / 4) / 2)
#define BIT_6(byte) (((byte - (16 * (byte / 16))) / 4) - (2 * (((byte - (16 * (byte / 16))) / 4) / 2)))
#define BIT_7(byte) (((byte - (16 * (byte / 16))) - (4 * ((byte - (16 * (byte / 16))) / 4))) / 2)
#define BIT_8(byte) (((byte - (16 * (byte / 16))) - (4 * ((byte - (16 * (byte / 16))) / 4))) - (2 * ((((byte - (16 * (byte / 16))) - (4 * ((byte - (16 * (byte / 16))) / 4))) / 2))))


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
        if(display[row][col] == true && bit == true){
            display[row][col] = false;
            reg[15] = 1;
        }else if(display[row][col] == false && bit == true){
            display[row][col] = bit;
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
    pc+=2;
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
    size_t col = 0;
    switch(mem[pc-1]){
        case 0xE0:
            /* Clear the Display */
            for(row = 0; row < CHIP8_DISPLAY_HEIGHT; row++){
                for(col = 0; col < CHIP8_DISPLAY_WIDTH; col++){
                    display[row][col] = false;
                }
            }
            disp_chng = true;
            break;
        case 0xEE:
            break;
        default:
            /* No Op */
            break;
    }
}

inline static void
Decode_1(void){
    pc += ((mem[pc - 2] - 0x10) * 256) + mem[pc-1]; /* pc += ((mem[pc-2] & 0x0F) << 8) | b */
}

inline static void
Decode_6(void){
    reg[mem[pc - 2] - 0x60] = mem[pc-1]; /* reg[mem[pc-2] & 0x0F] = mem[pc-1] */
}

inline static void
Decode_7(void){
    reg[mem[pc-2] - 0x70] += mem[pc-1];
}

inline static void
Decode_A(void){
    regI = ((mem[pc - 2] - 0xA0) * 256) + mem[pc-1]; /* regI += ((mem[pc-2] & 0x0F) << 8) | b */
}

inline static void
Decode_D(void){
    /*TODO: Support wraparound Semantics */
    /*TODO: Actually detect edge of screen */
    /*TODO: Actually use bitwise operations */
    uint8_t x = reg[mem[pc - 2] - 0xD0]; /* reg[mem[pc-2] && 0x0F] */
    uint8_t y = reg[mem[pc - 1] / 16]; /* reg[mem[pc-1] >> 4] */
    uint8_t sprite_height = mem[pc-1] - (16 * (mem[pc-1] / 16)); /* mem[pc - 1] & 0xF0 */
    uint8_t row = 0;
    uint8_t bit = 0;

    reg[15] = 0;
    for(row = 0; row < sprite_height; row++, regI++){
        bit = BIT_1(mem[regI]);
        updatedisp(y + row, x+0, bit);

        bit = BIT_2(mem[regI]);
        updatedisp(y + row, x+1, bit);

        bit = BIT_3(mem[regI]);
        updatedisp(y + row, x+2, bit);

        bit = BIT_4(mem[regI]);
        updatedisp(y + row, x+3, bit);

        bit = BIT_5(mem[regI]);
        updatedisp(y + row, x+4, bit);

        bit = BIT_6(mem[regI]);
        updatedisp(y + row, x+5, bit);

        bit = BIT_7(mem[regI]);
        updatedisp(y + row, x+6, bit);

        bit = BIT_8(mem[regI]);
        updatedisp(y + row, x+7, bit);
    }
    regI -= sprite_height;
    disp_chng = true;
}


inline static void
Decode_Unimplemented(void){
    fprintf(stderr, "Opcode %x%x not implemented!\n", mem[pc - 2], mem[pc - 1]);
}

#define Decode_2 Decode_Unimplemented
#define Decode_3 Decode_Unimplemented
#define Decode_4 Decode_Unimplemented
#define Decode_5 Decode_Unimplemented
#define Decode_8 Decode_Unimplemented
#define Decode_9 Decode_Unimplemented
#define Decode_B Decode_Unimplemented
#define Decode_C Decode_Unimplemented
#define Decode_E Decode_Unimplemented
#define Decode_F Decode_Unimplemented

_Bool
Chip8_OpExec(void){
    switch((mem[pc - 2] / 16)){
        case 0:
            Decode_0();
            break;
        case 1:
            Decode_1();
            break;
        case 2:
            Decode_2();
            break;
        case 3:
            Decode_3();
            break;
        case 4:
            Decode_4();
            break;
        case 5:
            Decode_5();
            break;
        case 6:
            Decode_6();
            break;
        case 7:
            Decode_7();
            break;
        case 8:
            Decode_8();
            break;
        case 9:
            Decode_9();
            break;
        case 0xA:
            Decode_A();
            break;
        case 0xB:
            Decode_B();
            break;
        case 0xC:
            Decode_C();
            break;
        case 0xD:
            Decode_D();
            break;
        case 0xE:
            Decode_E();
            break;
        case 0xF:
            Decode_F();
            break;
        default:
            fprintf(stderr, "Unknown OpCode: 0x%x%x\n", (unsigned int)mem[pc - 2], (unsigned int)mem[pc - 1]);
            break;
    }
    return true;
}
