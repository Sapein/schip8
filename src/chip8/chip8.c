#include <stdint.h>
#include <stdio.h>
#include <time.h>
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

#define Decode_1 Decode_Unimplemented
#define Decode_2 Decode_Unimplemented
#define Decode_3 Decode_Unimplemented
#define Decode_4 Decode_Unimplemented
#define Decode_5 Decode_Unimplemented
#define Decode_6 Decode_Unimplemented
#define Decode_7 Decode_Unimplemented
#define Decode_8 Decode_Unimplemented
#define Decode_9 Decode_Unimplemented
#define Decode_A Decode_Unimplemented
#define Decode_B Decode_Unimplemented
#define Decode_C Decode_Unimplemented
#define Decode_D Decode_Unimplemented
#define Decode_E Decode_Unimplemented
#define Decode_F Decode_Unimplemented


_Bool display[CHIP8_DISPLAY_HEIGHT][CHIP8_DISPLAY_WIDTH] = {false};
_Bool disp_chng = false;
uint8_t key = 0;
_Bool key_pressed = false;

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

static inline _Bool
and(_Bool a, _Bool b){
    return (a == true && b == true);
}

static inline _Bool
or(_Bool a, _Bool b){
    return (a == true || b == true);
}

static inline _Bool
xor(_Bool a, _Bool b){
    return (a == true && b == false) || (a == false && b == true);
}

static inline uint8_t
byte_op(uint8_t a, uint8_t b, _Bool (*op)(_Bool, _Bool)){
    return ((op(BIT_1(a), BIT_1(b)) * 128) + (and(BIT_2(a), BIT_2(b)) * 64) + (and(BIT_3(a), BIT_2(b)) * 32) +
            (op(BIT_4(a), BIT_4(b)) * 16) + (and(BIT_5(a), BIT_5(b)) * 8) + (and(BIT_6(a), BIT_6(b)) * 4) +
            (op(BIT_7(a), BIT_7(b)) * 2) + (and(BIT_8(a), BIT_8(b)) * 1));
}


void
Chip8_Init(void){
    /* Initalize the Chip8 System as necessary. */
    for(pc=0; pc < CHIP8_FONT_BYTES; pc++){
        mem[pc] = font[pc];
    }
    delay = 0, sound = 0, pc = 0, sp = 0, regI = 0;
    srand(time(NULL));
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

#undef Decode_0
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

#undef Decode_1
inline static void
Decode_1(void){
    pc += ((mem[pc - 2] - 0x10) * 256) + mem[pc-1]; /* pc += ((mem[pc-2] & 0x0F) << 8) | b */
}

#undef Decode_2
inline static void
Decode_2(void){
    mem[CHIP8_STACK_START + (2 * sp)] = ((pc - 2) / 256);
    mem[CHIP8_STACK_START + (2 * sp) + 1] = (((pc - 2) / 256) * 256) - (pc - 2);
    pc = (256 * (0x20 - mem[pc - 2])) + mem[pc - 1];
    sp++;
}

#undef Decode_3
inline static void
Decode_3(void){
    if(reg[0x30 - mem[pc - 2]] == mem[pc - 1]){
        pc += 2;
    }
}

#undef Decode_4
inline static void
Decode_4(void){
    if(reg[0x40 - mem[pc - 2]] != mem[pc - 1]){
        pc += 2;
    }
}

#undef Decode_5
inline static void
Decode_5(void){
    if(reg[0x50 - mem[pc - 2]] == reg[mem[pc - 1]]){
        pc += 2;
    }
}

#undef Decode_6
inline static void
Decode_6(void){
    reg[mem[pc - 2] - 0x60] = mem[pc-1]; /* reg[mem[pc-2] & 0x0F] = mem[pc-1] */
}

#undef Decode_7
inline static void
Decode_7(void){
    reg[mem[pc-2] - 0x70] += mem[pc-1];
}

#undef Decode_8
inline static void
Decode_8(void){
    switch(mem[pc - 1] - ((mem[pc - 1] / 16) * 16)){
        case 0:
            reg[mem[pc - 2] - 0x80] = reg[mem[pc - 1] / 16];
            break;
        case 1:
            reg[mem[pc - 2] - 0x80] = byte_op(reg[mem[pc - 2] - 0x80], reg[mem[pc - 1] / 16], &or);
            break;
        case 2:
            reg[mem[pc - 2] - 0x80] = byte_op(reg[mem[pc - 2] - 0x80], reg[mem[pc - 1] / 16], &and);
            break;
        case 3:
            reg[mem[pc - 2] - 0x80] = byte_op(reg[mem[pc - 2] - 0x80], reg[mem[pc - 1] / 16], &xor);
            break;
        case 4:
            if(((uint16_t)reg[mem[pc - 2] - 0x80] + (uint16_t)reg[mem[pc - 2] / 16]) > 255){
                reg[0xF] = 1;
            }
            reg[mem[pc - 2] - 0x80] += reg[mem[pc - 2] / 16];
            break;
        case 5:
            if(((int16_t)reg[mem[pc - 2] - 0x80] - (int16_t)reg[mem[pc - 2] / 16]) < 0){
                reg[0xF] = 1;
            }
            reg[mem[pc - 2] - 0x80] -= reg[mem[pc - 2] / 16];
            break;
        case 6:
            reg[15] = BIT_8(reg[mem[pc - 1] / 16]);
            reg[mem[pc - 2] - 0x80] = (reg[mem[pc - 2] / 16]) / 2;
            break;
        case 7:
            if(((int16_t)reg[mem[pc - 2] / 16] - (int16_t)reg[mem[pc - 2] - 0x80]) < 0){
                reg[0xF] = 1;
            }
            reg[mem[pc - 2] - 0x80] = reg[mem[pc - 2] / 16] - reg[mem[pc - 2] - 0x80];
            break;
        case 0xE:
            reg[15] = BIT_1(reg[mem[pc - 1] / 16]);
            reg[mem[pc - 2] - 0x80] = (reg[mem[pc - 2] / 16]) * 2;
            break;
    }
}

#undef Decode_9
inline static void
Decode_9(void){
    if(reg[mem[pc - 2] - 0x90] != reg[mem[pc - 1] / 16]){
        pc += 2;
    }
}

#undef Decode_A
inline static void
Decode_A(void){
    regI = ((mem[pc - 2] - 0xA0) * 256) + mem[pc-1]; /* regI += ((mem[pc-2] & 0x0F) << 8) | b */
}

#undef Decode_B
inline static void
Decode_B(void){
    pc = reg[0] + (((mem[pc - 2] - 0xB0) * 256) + mem[pc - 1])
}

#undef Decode_C
Decode_C(void){
    int random = 0;
    do{
        random = rand();
    }while(random > 0xFF);
    random = (int) byte_op((uint8_t) random, mem[pc - 2], &and);
    reg[mem[pc - 2] - 0xC0] = (uint8_t) random;
}

#undef Decode_D
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

#undef Decode_E
inline static void
Decode_E(void){
    switch(mem[pc - 1]){
        case 0x9E:
            if(key_pressed == true && key == reg[mem[pc - 2] - 0xE0]){
                pc += 2;
            }
            break;
        case 0xA1:
            if(key_pressed == false || key != reg[mem[pc - 2] - 0xE0]){
                pc += 2;
            }
            break;
        default:
            Decode_Unimplemented();
            break;
    }
}

#undef Decode_F
inline static void
Decode_F(void){
    switch(mem[pc - 1]){
        case 0x07:
            reg[mem[pc - 2] - 0xF0] = delay;
            break;
        case 0x0A:
            if(key_pressed != true){
                pc -= 2;
                delay++;
                sound++;
            }else{
                reg[mem[pc - 2] - 0xF0] = key;
            }
            break;
        case 0x15:
            delay = reg[mem[pc - 2] - 0xF0];
            break;
        case 0x18:
            sound = reg[mem[pc - 2] - 0xF0];
            break;
        case 0x1E:
            sound += reg[mem[pc - 2] - 0xF0];
            break;
        case 0x29:
            regI = reg[mem[pc - 2] - 0xF0] * 4;
            break;
        case 0x33:
            mem[regI] = (reg[mem[pc - 2] - 0xF0] / 100);
            mem[regI + 1] = (reg[mem[pc - 2] - 0xF0] / 10) - (mem[regI] * 10);
            mem[regI + 2] = reg[mem[pc - 2] - 0xF0] - ((reg[mem[pc - 2] - 0xF0] / 10) * 10);
            break;
        case 0x55:
            if((CHIP8_STACK_START + (2 * sp + 2)) >= CHIP8_ROM_START){
                /* If there is space in the stack, we can temporarily borrow it for a counter */
                for(mem[CHIP8_STACK_START + (2 * sp + 2)] += (mem[pc - 2] - 0xF0); mem[CHIP8_STACK_START + (2 * sp + 2)] > 0 ; mem[CHIP8_STACK_START + (2 * sp + 2)]--, regI++){
                    mem[regI] = reg[(mem[pc - 2] - 0xF0) - mem[CHIP8_STACK_START + (2 * sp + 2)] = 0];
                }
                mem[regI] = reg[mem[mem[pc - 2] - 0xF0]];
                regI++;
            }else if(mem[CHIP8_STACK_START - 1] - ((mem[CHIP8_STACK_START - 1] / 16) * 16) == 0){
                /* Otherwise, we can borrow space in the last nibble before the stack if it is a 0*/
                for(mem[CHIP8_STACK_START - 1] += (mem[pc - 2] - 0xF0); (mem[CHIP8_STACK_START - 1] - (16 * (mem[CHIP8_STACK_START - 1] / 16))) > 0; mem[CHIP8_STACK_START]--, regI++){
                    mem[regI] = reg[mem[CHIP8_STACK_START - 1] - (16 * (mem[CHIP8_STACK_START - 1] / 16))];
                }
                mem[regI] = reg[mem[mem[pc - 2] - 0xF0]];
                regI++;
            }
            break;
        case 0x65:
            /* Inverse of 55 */
            if((CHIP8_STACK_START + (2 * sp + 2)) >= CHIP8_ROM_START){
                /* If there is space in the stack, we can temporarily borrow it for a counter */
                for(mem[CHIP8_STACK_START + (2 * sp + 2)] += (mem[pc - 2] - 0xF0); mem[CHIP8_STACK_START + (2 * sp + 2)] > 0 ; mem[CHIP8_STACK_START + (2 * sp + 2)]--, regI++){
                    reg[(mem[pc - 2] - 0xF0) - mem[CHIP8_STACK_START + (2 * sp + 2)] = 0] = mem[regI];
                }
                reg[mem[mem[pc - 2] - 0xF0]] = mem[regI];
                regI++;
            }else if(mem[CHIP8_STACK_START - 1] - ((mem[CHIP8_STACK_START - 1] / 16) * 16) == 0){
                /* Otherwise, we can borrow space in the last nibble before the stack if it is a 0*/
                for(mem[CHIP8_STACK_START - 1] += (mem[pc - 2] - 0xF0); (mem[CHIP8_STACK_START - 1] - (16 * (mem[CHIP8_STACK_START - 1] / 16))) > 0; mem[CHIP8_STACK_START]--, regI++){
                    reg[mem[CHIP8_STACK_START - 1] - (16 * (mem[CHIP8_STACK_START - 1] / 16))] = mem[regI];
                }
                reg[mem[mem[pc - 2] - 0xF0]] = mem[regI];
                regI++;
            }
            break;
        default:
            Decode_Unimplemented();
            break;
    }
}

inline static void
Decode_Unimplemented(void){
    fprintf(stderr, "Opcode %x%x not implemented!\n", mem[pc - 2], mem[pc - 1]);
}


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
