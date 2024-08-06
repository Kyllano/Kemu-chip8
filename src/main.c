#include "chip8.h"

chip8* chip;

int main(int argc, char* args[]) {
    //setup graphics
    initialize_chip8("./rom/quirk.ch8");
    SDL_Event event;

    while (1) {
        handle_input(&event);

        for (size_t i = 0; i < 11; i++) {
            emulate_cycle();
        }

        fflush(stdout);
        usleep(1000);
    }

    printf("We done yeah?\n");

    exit_rountine(0);
    return 0;
}