#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include "config.h"
#include "io/io.h"
#include "chip8/chip8.h"

_Bool
argparse(int argc, char *argv[]);

_Bool
romload(char *path);

int
main(int argc, char *argv[]){
    int success = 0;
    int instruct = 0;
    time_t start = 0;
    time_t end = 0;
    if(IO_Init() != true){
        goto end;
    }
    Chip8_Init();
    if(argc > 0){
        if(argparse(argc, argv)){success = 1; goto end;}
        if(romload(argv[argc - 1])){success = 1; goto end;}
        for(instruct = 0, start = time(NULL); Chip8_Cycle() == true; instruct++){

            if(disp_chng){
                IO_UpdateScreen(display);
            }

            key = IO_GetKeyPress();
            if(key <= 0xF){
                key_pressed = true;
            }

            end = time(NULL);
            if(instruct >= CHIP8_CYCLE_SPEED){
                if((end - start) > 0){
                    sleep(1);
                }
                instruct = 0;
            }

            start = time(NULL);
        }
    }else{
        fprintf(stderr, "You must provide a ROM file\n!");
    }

end:
    Chip8_Shutdown();
    IO_Shutdown();
    return success;
}

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
