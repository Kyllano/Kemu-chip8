#include "chip8.h"

unsigned char chip8_fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void initialize_chip8(char* path_to_rom) {
    chip = (chip8*)malloc(sizeof(chip8));
    printf("chip malloced\n");
    srand(time(NULL));

    chip->pc = 0x200;
    chip->i = 0;
    chip->sp = 0;
    chip->delay_timer = 0;
    chip->sound_timer = 0;

    memset(chip->stack, 0, 16 * sizeof(short));   //clear stack
    memset(chip->memory, 0, 4096 * sizeof(char)); // clear memory
    for (size_t i = 0; i < 16; i++) {
        chip->reg[i] = 0; //Clear registers
    }

    //initialize graphics
    chip->graphics = (sdl_graphics*)malloc(sizeof(sdl_graphics));
    SDL_init_context(chip->graphics, APP_NAME, SCREEN_WIDTH * SCALE_FACTOR, SCREEN_HEIGHT * SCALE_FACTOR);

    //load program into memory (scoping variables coz Im a good boy)
    {
        FILE* file = fopen(path_to_rom, "rb");
        unsigned char byte;
        size_t count = 0x200;
        while (fread(&byte, sizeof(char), 1, file) == 1) {
            //printf("read 0x%2x, byte no 0x%3lx\n", byte, count);
            chip->memory[count] = byte;
            count++;
        }

        fclose(file);
    }

    //load fontset
    for (size_t i = 0; i < 80; i++) {
        chip->memory[i] = chip8_fontset[i];
    }
}

void emulate_cycle(void) {
    opcode inst = chip->memory[chip->pc] << 8 | chip->memory[chip->pc + 1];

    exec_instr(inst);

    if (chip->delay_timer > 0) {
        chip->delay_timer--;
    }

    if (chip->sound_timer > 0) {
        if (chip->sound_timer == 1) {
            printf("MAKE SOME NOISE\a");
        }
        chip->sound_timer--;
    }
}

void exit_rountine(int exit_code) {
    SDL_close(chip->graphics);
    free(chip);
    exit(exit_code);
}

void exec_instr(opcode instr) {
    printf("instruction is 0x%04x\n", instr);
    switch (instr >> 12) {
        case 0x0:
            switch (instr) {
                //clear screen
                case 0x00e0:
                    clear_display(chip->graphics);
                    chip->pc += 2;
                    break;
                //exit subroutine
                case 0x00ee:
                    chip->sp--;
                    chip->pc = chip->stack[chip->sp];
                    break;
                default:
                    fprintf(stderr, "Unknown instruction 0x%04x\n", instr);
                    exit_rountine(EXIT_FAILURE);
                    break;
            }
            break;
        case 0x1:
            //Jump to address
            chip->pc = instr & 0x0fff;
            break;
        case 0x2:
            //Call subroutine
            chip->pc += 2;
            chip->stack[chip->sp] = chip->pc;
            chip->sp++;
            chip->pc = (instr & 0xfff);
            break;
        case 0x3: {
            //Skip the following instruction if the value of register VX equals NN
            size_t ireg = (instr & 0x0f00) >> 8;
            size_t val = (instr & 0x00ff);
            if (chip->reg[ireg] == val) {
                chip->pc += 2;
            }
            chip->pc += 2;
        } break;
        case 0x4: {
            //Skip the following instruction if the value of register VX is not equal to NN
            size_t ireg = ((instr & 0x0f00) >> 8);
            size_t val = (instr & 0x00ff);
            if (chip->reg[ireg] != val) {
                chip->pc += 2;
            }
            chip->pc += 2;
        } break;
        case 0x5: {
            //Skip the following instruction if the value of register VX is equal to the value of register VY
            size_t iregx = (instr & 0x0f00) >> 8;
            size_t iregy = (instr & 0x00f0) >> 4;
            if (chip->reg[iregx] == chip->reg[iregy]) {
                chip->pc += 2;
            }
            chip->pc += 2;
        } break;
        case 0x6: {
            //Set VX to NN
            size_t ireg = (instr & 0x0f00) >> 8;
            chip->reg[ireg] = (instr & 0x00ff);
            chip->pc += 2;
        } break;
        case 0x7: {
            //Add NN to VX
            size_t ireg = (instr & 0x0f00) >> 8;
            size_t val = (instr & 0x00ff);
            chip->reg[ireg] += val;
            chip->pc += 2;
        } break;
        case 0x8: {
            size_t iregx = (instr & 0x0f00) >> 8;
            size_t iregy = (instr & 0x00f0) >> 4;
            chip->pc += 2; // Putting it before because I don't wanna copy that 4 times
            switch (instr & 0xf) {
                case 0x0:
                    chip->reg[iregx] = chip->reg[iregy];
                    break;
                case 0x1:
                    chip->reg[iregx] = chip->reg[iregy] | chip->reg[iregx];
                    break;
                case 0x2:
                    chip->reg[iregx] = chip->reg[iregy] & chip->reg[iregx];
                    break;
                case 0x3:
                    chip->reg[iregx] = chip->reg[iregy] ^ chip->reg[iregx];
                    break;
                case 0x4: {
                    char carry = ((chip->reg[iregx] + chip->reg[iregy] > 0xff) ? 1 : 0);
                    chip->reg[iregx] += chip->reg[iregy];
                    chip->reg[0xf] = carry;
                } break;
                case 0x5: {
                    char carry = ((chip->reg[iregx] > chip->reg[iregy]) ? 1 : 0);
                    chip->reg[iregx] -= chip->reg[iregy];
                    chip->reg[0xf] = carry;
                } break;
                case 0x6:
                    chip->reg[0xf] = (chip->reg[iregy] & 0x1);
                    chip->reg[iregx] = chip->reg[iregy] >> 1;
                    break;
                case 0x7:
                    char carry = ((chip->reg[iregy] > chip->reg[iregx]) ? 1 : 0);
                    chip->reg[iregx] = chip->reg[iregy] - chip->reg[iregx];
                    chip->reg[0xf] = carry;
                    break;
                case 0xe:
                    chip->reg[0xf] = (chip->reg[iregy] & 0x80);
                    chip->reg[iregx] = chip->reg[iregy] << 1;
                    break;
                default:
                    fprintf(stderr, "Unknown instruction 0x%04x\n", instr);
                    exit_rountine(EXIT_FAILURE);
                    break;
            }
        } break;
        case 0x9: {
            //Skip the following instruction if register VX is not equal to register VY
            size_t iregx = ((instr & 0x0f00) >> 8);
            size_t iregy = ((instr & 0x00f0) >> 4);
            if (chip->reg[iregx] != chip->reg[iregy]) {
                chip->pc += 2;
            }
            chip->pc += 2;
        } break;
        case 0xa: {
            chip->i = (instr & 0x0fff);
            chip->pc += 2;
        } break;
        case 0xb:
            chip->pc = (instr & 0xfff) + chip->reg[0];
            break;
        case 0xc:
            chip->reg[(instr & 0x0f00) >> 8] = (rand() % 0xf00) & (instr & 0xff);
            chip->pc += 2;
            break;
        case 0xd: {
            size_t iregx = ((instr & 0x0f00) >> 8);
            size_t iregy = ((instr & 0x00f0) >> 4);
            char n = (instr & 0x000f);

            drawScreen(chip->reg[iregx], chip->reg[iregy], n);
            //printDisplay();

            if (chip->reg[0xf] != 0x0) {
                SDL_update_surface(chip->graphics);
                SDL_update_frame(chip->graphics);
            }

            chip->pc += 2;

        } break;
        case 0xe:
            switch (instr & 0xff) {
                case 0x9e:
                    if (chip->input[chip->reg[(instr & 0x0f00) >> 8]]) {
                        chip->pc += 2;
                    }
                    chip->pc += 2;
                    break;

                default:
                    fprintf(stderr, "Unknown instruction 0x%04x\n", instr);
                    exit_rountine(EXIT_FAILURE);
                    break;
            }
            break;
        case 0xf: {
            switch (instr & 0x00ff) {
                case 0x07:
                    chip->reg[(instr & 0x0f00) >> 8] = chip->delay_timer;
                    chip->pc += 2;
                    break;
                case 0x1e:
                    chip->i += chip->reg[(instr & 0x0f00) >> 8];
                    chip->pc += 2;
                    break;
                case 0x15:
                    chip->delay_timer = chip->reg[(instr & 0x0f00) >> 8];
                    chip->pc += 2;
                    break;
                case 0x33: {
                    size_t iregx = ((instr & 0x0f00) >> 8);
                    size_t hund = chip->reg[iregx] / 100;
                    size_t tens = (chip->reg[iregx] - (hund * 100)) / 10;
                    size_t unit = chip->reg[iregx] - (hund * 100) - (tens * 10);

                    chip->memory[chip->i] = hund;
                    chip->memory[chip->i + 1] = tens;
                    chip->memory[chip->i + 2] = unit;
                    chip->pc += 2;
                } break;
                case 0x55:
                    for (size_t i = 0; i <= ((instr & 0x0f00) >> 8); i++) {
                        chip->memory[chip->i + i] = chip->reg[i];
                    }
                    chip->pc += 2;
                    break;
                case 0x65:
                    for (size_t i = 0; i <= ((instr & 0x0f00) >> 8); i++) {
                        chip->reg[i] = chip->memory[chip->i + i];
                    }
                    chip->pc += 2;
                    break;
                default:
                    fprintf(stderr, "Unknown instruction 0x%04x\n", instr);
                    exit_rountine(EXIT_FAILURE);
                    break;
            }

        } break;
        default:
            fprintf(stderr, "Unknown instruction 0x%04x\n", instr);
            exit_rountine(EXIT_FAILURE);
            break;
    }
}