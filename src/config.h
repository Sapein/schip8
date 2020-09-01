#ifndef SCHIP8_CONFIG
#define SCHIP8_CONFIG

#define CHIP8_RAM_SIZE 4095
#define CHIP8_FONT_BYTES 80
#define CHIP8_ROM_START 512 /* This is where we place the ROM initally in memory, it should be 512 or 1536.) */
#define CHIP8_CYCLE_SPEED 700 /* This is the number of instructions per second */

#define CHIP8_DISPLAY_HEIGHT 32
#define CHIP8_DISPLAY_WIDTH 64

#define CHIP8_STACK_START 86
#define CHIP8_STACK_SIZE 32


#ifdef chip8
const uint8_t font[CHIP8_FONT_BYTES] = {0xF0, 0x90, 0x90, 0x90, /* Bytes for 0 */
                                        0xF0, 0x20, 0x60, 0x20, /* Bytes for 1 */
                                        0x20, 0x70, 0xF0, 0x10,
                                        0xF0, 0x80, 0xF0, 0xF0,
                                        0x10, 0xF0, 0x10, 0xF0,
                                        0x90, 0x90, 0xF0, 0x10,
                                        0x10, 0xF0, 0x80, 0xF0,
                                        0x10, 0xF0, 0xF0, 0x80,
                                        0xF0, 0x90, 0xF0, 0xF0,
                                        0x10, 0x20, 0x40, 0x40,
                                        0xF0, 0x90, 0xF0, 0x90,
                                        0xF0, 0xF0, 0x90, 0xF0,
                                        0x10, 0xF0, 0xF0, 0x90,
                                        0xF0, 0x90, 0x90, 0xE0,
                                        0x90, 0xE0, 0x90, 0xE0, /* Font for A */
                                        0xF0, 0x80, 0x80, 0x80,
                                        0xF0, 0xE0, 0x90, 0x90,
                                        0x90, 0xE0, 0xF0, 0x80,
                                        0xF0, 0x80, 0xF0, 0xF0,
                                        0x80, 0xF0, 0x80, 0x80}; /* Bytes for F */


#endif

#endif
