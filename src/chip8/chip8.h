#ifndef SUCKLESS_CHIP8
#define SUCKLESS_CHIP8

void
Chip8_Init(void);

_Bool
Chip8_Shutdown(void);

_Bool
Chip8_Cycle(void);

void
Chip8_LoadROM(uint8_t rom[], size_t rom_size);

extern _Bool display[CHIP8_DISPLAY_HEIGHT][CHIP8_DISPLAY_WIDTH];
extern _Bool disp_chng;
extern uint8_t key;
extern _Bool key_pressed;
#endif
