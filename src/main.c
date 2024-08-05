#include "chip8.h"

chip8* chip;

int main(int argc, char* args[]) {
    //setup graphics
    initialize_chip8("./rom/octojam.ch8");

    emulate_cycle();
    fflush(stdout);
    emulate_cycle();
    fflush(stdout);
    emulate_cycle();
    fflush(stdout);
    emulate_cycle();
    fflush(stdout);
    emulate_cycle();
    fflush(stdout);
    emulate_cycle();
    fflush(stdout);

    SDL_Event event;
    while (true) {
        usleep(50000);
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
            break;
        }
    }

    exit_rountine(0);
    return 0;
}