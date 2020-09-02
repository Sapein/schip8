#ifndef SUCKLESS_C8IO
#define SUCKLESS_C8I0

_Bool IO_Init(void);
_Bool IO_Shutdown(void);

_Bool IO_UpdateScreen(_Bool display[CHIP8_DISPLAY_HEIGHT][CHIP8_DISPLAY_WIDTH]);
uint8_t IO_GetKeyPress(void);

#endif
