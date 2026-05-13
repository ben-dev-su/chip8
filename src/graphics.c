#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <stdbool.h>
#include <stdio.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_TITLE "Chip8"

typedef struct Sdl {
  SDL_Window *window;
  SDL_Renderer *renderer;
} sdl_t;

void tick(bool *running) {
  SDL_Event event;
  // event handling
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      *running = true;
    } else if (event.type == SDL_EVENT_KEY_DOWN) {
      SDL_Log("key was pressed: %d", event.key.key);
    }
  }
}

void update() {};
void input();
void render();

bool init_sdl(sdl_t *sdl) {

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL initialisation failed: %s\n",
                 SDL_GetError());

    return false;
  }

  if (!SDL_CreateWindowAndRenderer(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT,
                                   SDL_WINDOW_BORDERLESS, &sdl->window,
                                   &sdl->renderer)) {

    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create: %s\n",
                 SDL_GetError());
    return false;
  }

  return true;
}

void graphics() {
  sdl_t sdl = {.window = NULL, .renderer = NULL};
  bool res_init = init_sdl(&sdl);

  if (!res_init) {
    SDL_DestroyWindow(sdl.window);
    SDL_DestroyRenderer(sdl.renderer);
    SDL_Quit();
    return;
  }

  bool running = false;
  while (!running) {
    tick(&running);
    // game logic
    SDL_SetRenderDrawColor(sdl.renderer, 0xFF, 0, 0, 0xFF);
    SDL_RenderClear(sdl.renderer);
    SDL_RenderPresent(sdl.renderer);
  }
  SDL_DestroyWindow(sdl.window);
  SDL_DestroyRenderer(sdl.renderer);
  SDL_Quit();
}

int main() {
  graphics();
  return 0;
}
