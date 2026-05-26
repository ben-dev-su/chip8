#include "backend.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_WIDTH 64
#define WINDOW_HEIGHT 32
#define WINDOW_SCALE 10
#define WINDOW_TITLE "Chip8"

static bool initialise_sdl(sdl_t *sdl) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL initialisation failed: %s\n",
                 SDL_GetError());

    return false;
  }

  int width = WINDOW_WIDTH * WINDOW_SCALE;
  int height = WINDOW_HEIGHT * WINDOW_SCALE;
  if (!SDL_CreateWindowAndRenderer(WINDOW_TITLE, width, height, 0, &sdl->window,
                                   &sdl->renderer)) {

    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create: %s\n",
                 SDL_GetError());
    return false;
  }
  return true;
}

bool setup_backend(sdl_t *sdl) { return initialise_sdl(sdl); }

bool process_events() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      return false;
    } else if (event.type == SDL_EVENT_KEY_DOWN) {
      SDL_Log("key was pressed: %d", event.key.key);
    }
  }
  return true;
}

void render(chip8_t *chip8, sdl_t *sdl) {
  SDL_FRect rect = {.x = 0, .y = 0, .h = WINDOW_SCALE, .w = WINDOW_SCALE};
  SDL_SetRenderDrawColor(sdl->renderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(sdl->renderer);

  SDL_SetRenderDrawColor(sdl->renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
  for (size_t y = 0; y < WINDOW_HEIGHT; y++) {
    for (size_t x = 0; x < WINDOW_WIDTH; x++) {
      int i = WINDOW_WIDTH * y + x;
      if (chip8->gfx[i]) {
        rect.x = x * WINDOW_SCALE;
        rect.y = y * WINDOW_SCALE;
        SDL_RenderFillRect(sdl->renderer, &rect);
      }
    }
  }
  SDL_RenderPresent(sdl->renderer);
}

void clean_up_backend(sdl_t *sdl) {
  SDL_DestroyWindow(sdl->window);
  SDL_DestroyRenderer(sdl->renderer);
  SDL_Quit();
}
