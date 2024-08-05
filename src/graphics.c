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
}

void clear_display(sdl_graphics* graphics) {
    SDL_SetRenderDrawColor(graphics->renderer, 0,0,0,0);
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

