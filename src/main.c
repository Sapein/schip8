#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "config.h"
#include "chip8/chip8.h"

_Bool
argparse(int argc, char *argv[]);

_Bool
romload(char *path);

int
main(int argc, char *argv[]){
    int success = 0;
    Chip8_Init();
    if(argc > 0){
        if(argparse(argc, argv)){success = 1; goto end;}
        if(romload(argv[argc - 1])){success = 1; goto end;}
        while(Chip8_Cycle()){
            if(disp_chng){
                for(int row = 0; row < CHIP8_DISPLAY_HEIGHT; row++){
                    for(int col = 0; col < CHIP8_DISPLAY_WIDTH; col++){
                        if(display[row][col]){
                            printf(" ");
                        }else{
                            printf("*");
                        }
                    }
                    printf("\n");
                }
            }
        }
    }else{
        fprintf(stderr, "You must provide a ROM file\n!");
    }

end:
    Chip8_Shutdown();
    return success; }

_Bool
argparse(int argc, char *argv[]){
    for(char *arg = argv[0]; arg == argv[argc - 1]; arg = argv[1]){
        if(strncmp("--help", arg, strlen("--help")) == 0 || strncmp("-h", arg, strlen("-h"))){
            printf("Usage: schip8 [--help|-h] (rom)\n");
            printf("----------------------------------\n");
            printf("Options:\n");
            printf("\t--help\\-h: Displays this help\n");
            return true;
        }else if(strncmp("--", arg, strlen("--")) == 0){
            return false;
        }
    }
    return false;
}

_Bool
romload(char *path){
    FILE *romfile = NULL;
    unsigned char rombuffer[CHIP8_RAM_SIZE] = {0};
    long int romsize = 0;
    _Bool exit = false;

    if((romfile = fopen(path, "r")) != NULL){
        if(fseek(romfile, 0, SEEK_END) == 0){
            if((romsize = ftell(romfile)) != -1L){
                rewind(romfile);
                if(fread(rombuffer, 1, romsize, romfile) == romsize){
                    Chip8_LoadROM(rombuffer, (size_t) romsize);
                }else{
                    perror("schip8 - fread:");
                    exit = true;
                }
            }else if(romsize == -1L){
                perror("schip8 - ftell:");
                exit = true;
            }
        }else{
            perror("schip8 - fseek:");
            exit = true;
        }
    }
    fclose(romfile);
    romfile = NULL;
    return exit;
}
