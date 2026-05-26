#pragma once

#include "chip8.h"
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>

typedef struct Sdl {
  SDL_Window *window;
  SDL_Renderer *renderer;
} sdl_t;

bool setup_backend(sdl_t *sdl);
void clean_up_backend(sdl_t *sdl);
void render(chip8_t *chip8, sdl_t *sdl);
bool process_events();
