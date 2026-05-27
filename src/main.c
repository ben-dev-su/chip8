#define _POSIX_C_SOURCE                                                        \
  199309L // https://stackoverflow.com/questions/50167494/how-to-know-to-which-value-i-should-define-posix-c-source
#include "backend.h"
#include "chip8.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

int main() {
  chip8_t chip;
  sdl_t sdl;
  chip8_initialise(&chip);
  chip8_loadGame(&chip, "res/roms/chip8-test-suite/1-chip8-logo.ch8");
  if (!setup_backend(&sdl)) {
    clean_up_backend(&sdl);
    exit(1);
  }

  struct timespec ts = {.tv_sec = 0, .tv_nsec = 166666667};
  while (1) {
    chip8_emulateCycle(&chip);
    if (!process_events()) {
      break;
    }
    render(&chip, &sdl);
    nanosleep(&ts, NULL);
  }

  clean_up_backend(&sdl);
  return 0;
}
