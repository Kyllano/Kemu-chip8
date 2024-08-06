#include "chip8.h"

int SDL_init_context(sdl_graphics* graphics, char* title, size_t width, size_t height) {
    graphics->width = width;
    graphics->height = height;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit_rountine(EXIT_FAILURE);
    }

    if (SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_SHOWN, &graphics->window, &graphics->renderer) < 0) {
        fprintf(stderr, "Error on context initiation! SDL_Error: %s\n", SDL_GetError());
        exit_rountine(EXIT_FAILURE);
    } else if (graphics->window == NULL || graphics->renderer == NULL) {
        fprintf(stderr, "Window or renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        exit_rountine(EXIT_FAILURE);
    }

    SDL_SetWindowTitle(graphics->window, title);
    graphics->screenSurface = SDL_GetWindowSurface(graphics->window);

    graphics->black = SDL_MapRGB(graphics->screenSurface->format, RB, GB, BB);
    graphics->white = SDL_MapRGB(graphics->screenSurface->format, RW, GW, BW);

    return SDL_INIT_SUCCESS;
}

void SDL_update_frame(sdl_graphics* graphics) {
    SDL_Texture* current_texture = SDL_CreateTextureFromSurface(graphics->renderer, graphics->screenSurface);
    if (current_texture == NULL) {
        fprintf(stderr, "Could create Texture! SDL Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    if (SDL_RenderCopy(graphics->renderer, current_texture, NULL, NULL) < 0) {
        fprintf(stderr, "Could not render copy! SDL Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_RenderPresent(graphics->renderer);
    SDL_DestroyTexture(current_texture);
}

void clear_display(sdl_graphics* graphics) {
    for (size_t i = 0; i < SCREEN_HEIGHT * SCREEN_WIDTH; i++) {
        chip->screen[i] = 0x0;
    }

    SDL_SetRenderDrawColor(graphics->renderer, RB, GB, BB, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(graphics->renderer);
    SDL_RenderPresent(graphics->renderer);
}

void SDL_close(sdl_graphics* graphics) {
    //Destroy window
    SDL_FreeSurface(graphics->screenSurface);
    SDL_DestroyRenderer(graphics->renderer);
    SDL_DestroyWindow(graphics->window);
    graphics->window = NULL;
    graphics->renderer = NULL;
    graphics->screenSurface = NULL;

    //Quit SDL subsystems
    SDL_Quit();

    free(graphics);
}

//This first does the draw screen on the chip 8 struct
//need to then draw on our actual screen surface
//Gotta check if it s right tho
void drawScreen(char x, char y, char n) {
    chip->reg[0xf] = 0x0;
    x %= SCREEN_WIDTH;
    y %= SCREEN_HEIGHT;
    for (char i = 0; i < n; i++) {
        //Do one line (one byte of data)
        if (y + i > SCREEN_HEIGHT) {
            break;
        }
        for (char j = 0; j < 8; j++) {
            if (x + j >= SCREEN_WIDTH) {
                //clipping the rest of the line
                break;
            }
            size_t pixel_addr = ((x + j)) + ((y + i)) * SCREEN_WIDTH;
            char old_pixel = chip->screen[pixel_addr];
            chip->screen[pixel_addr] ^= (chip->memory[chip->i + i] >> (((8 - 1) - j)));
            chip->screen[pixel_addr] &= 0x1; //keep only the bit that says if it is on or off
            char new_pixel = chip->screen[pixel_addr];
            //update VF if needed
            chip->reg[0xf] |= (old_pixel = 0x1 && new_pixel == 0x0);
        }
    }
}

/* void drawScreen(char x, char y, char n) {
    printf("printing\n");
    chip->reg[0xf] = 0x0;
    x %= SCREEN_WIDTH;
    y %= SCREEN_HEIGHT;

    for (char i = 0; i < n; i++) {
        if (y + i >= SCREEN_HEIGHT) {
            printf("breaking\n");
            //clipping the rest of the sprite
            break;
        }
        char data = chip->memory[chip->i + i];
        if (data == 0x0) {
            continue;
        }
        printf("data is 0x%02x\n", data);
        for (char j = 0; j < 8; j++) {
            if (x + j >= SCREEN_WIDTH) {
                printf("breaking bad\n");
                //clipping the rest of the line
                continue;
            }
            if (((data >> ((8 - 1) - j)) & 0x1) == 0x0) {
                continue;
            }
            printf("pixel is 0x%01x\n", ((data >> ((8 - 1) - j)) & 0x1));
            size_t pixel_address = (x + j) + ((y + i) * SCREEN_WIDTH);
            chip->screen[pixel_address] ^= 0x1;
            //chip->reg[0xf] |= !(chip->screen[pixel_address]);
            if (chip->screen[pixel_address] == 0x0) {
                chip->reg[0xf] = 0x1;
            }
        }
    }
} */

void printDisplay(void) {
    for (size_t i = 0; i < SCREEN_HEIGHT; i++) {
        for (size_t j = 0; j < SCREEN_WIDTH; j++) {
            char to_print = (chip->screen[i * SCREEN_WIDTH + j] == 0x0) ? ' ' : '*';
            printf("%c", to_print);
        }
        printf("\n");
    }
}

void SDL_update_surface(sdl_graphics* graphics) {
    for (size_t x = 0; x < SCREEN_WIDTH; x++) {
        for (size_t y = 0; y < SCREEN_HEIGHT; y++) {
            SDL_Rect pixel;
            pixel.x = x * SCALE_FACTOR;
            pixel.y = y * SCALE_FACTOR;
            pixel.h = SCALE_FACTOR;
            pixel.w = SCALE_FACTOR;

            if (chip->screen[x + y * SCREEN_WIDTH] == 0x0) {
                SDL_FillRect(graphics->screenSurface, &pixel, graphics->black);
            } else {
                SDL_FillRect(graphics->screenSurface, &pixel, graphics->white);
            }
        }
    }
}
