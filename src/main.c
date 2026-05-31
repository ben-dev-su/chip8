#define _POSIX_C_SOURCE                                                        \
  199309L // https://stackoverflow.com/questions/50167494/how-to-know-to-which-value-i-should-define-posix-c-source
#include "backend.h"
#include "chip8.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
  chip8_t chip;
  sdl_t sdl;
  chip8_initialise(&chip);
  char *file_name = "res/roms/chip8-test-suite/3-corax+.ch8";
  if (!chip8_load_game(&chip, file_name)) {
    printf("Unable to load game: %s", file_name);
    exit(1);
  }
  if (!setup_backend(&sdl)) {
    clean_up_backend(&sdl);
    exit(1);
  }

  struct timespec ts = {.tv_sec = 0, .tv_nsec = 166666667};
  while (1) {
    if (!chip8_emulate_cycle(&chip)) {
      printf("chip8 can't be NULL (forgot to initialise?): %p", &chip);
      break;
    }
    if (!process_events()) {
      break;
    }
    render(&chip, &sdl);
    nanosleep(&ts, NULL);
  }

  clean_up_backend(&sdl);
  return 0;
}
