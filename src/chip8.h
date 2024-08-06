#ifndef __CHIP8_H__
#define __CHIP8_H__

#include <SDL2/SDL.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

//SDL error codes
#define SDL_INIT_SUCCESS 0;
#define ERROR_SDL_INIT -1;
#define ERROR_SDL_CREATE_WINDOWS -2;

//Constants
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SCALE_FACTOR 16
#define APP_NAME "My cool ass Chip 8 emulator"

//RGB values for B(black) and W(white)
//Color palette Paperback-2 Palette
//Black
#define RB 0x38
#define GB 0x2b
#define BB 0x26
//White
#define RW 0xb8
#define GW 0xc2
#define BW 0xb9

// -----------SDL GRAPHICS----------------
typedef struct sdl_graphics_t {
    SDL_Renderer* renderer;
    SDL_Window* window;
    SDL_Surface* screenSurface;
    size_t width;
    size_t height;
    Uint32 black;
    Uint32 white;
} sdl_graphics;

// -----------MACHINE INFO----------------
typedef struct chip8_t {
    unsigned char memory [4096];
    unsigned char reg [16];
    unsigned short i;
    unsigned short pc;
    unsigned char screen [SCREEN_WIDTH * SCREEN_HEIGHT];
    unsigned short stack [16];
    unsigned short sp;
    unsigned char input[16];
    unsigned char delay_timer;
    unsigned char sound_timer;
    sdl_graphics* graphics;
} chip8;

extern unsigned char chip8_fontset[80];

typedef unsigned short opcode;

extern chip8* chip;


//SDL functions

int SDL_init_context(sdl_graphics* graphics, char* title, size_t width, size_t height);
void SDL_update_frame(sdl_graphics* graphics);
void SDL_close(sdl_graphics* graphics);
void clear_display(sdl_graphics* graphics);
void drawScreen(char x, char y, char n);
void SDL_update_surface(sdl_graphics* graphics);
void printDisplay(void);

//Emulator functions
void initialize_chip8(char* path_to_rom);
void exec_instr(opcode instr);
void emulate_cycle(void);
void exit_rountine(int exit_code);
void handle_input(SDL_Event* event);

#endif