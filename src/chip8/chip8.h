#ifndef SUCKLESS_CHIP8
#define SUCKLESS_CHIP8

void
Chip8_Init(void);

_Bool
Chip8_Shutdown(void);

_Bool
Chip8_LoadROM(uint8_t *rom, size_t rom_size);
#endif
