#include "chip8.h"

chip8* chip;

int main(int argc, char* args[]) {
    //setup graphics
    initialize_chip8("./rom/octojam.ch8");

    for (size_t i = 0; i < 2000; i++) {
        emulate_cycle();
        fflush(stdout);
        usleep(500);
    }

    printf("We done yeah?\n");

    SDL_Event event;
    while (1) {
        usleep(5000);
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
            break;
        }
    }

    exit_rountine(0);
    return 0;
}