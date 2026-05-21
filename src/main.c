#define _POSIX_C_SOURCE                                                        \
  199309L // https://stackoverflow.com/questions/50167494/how-to-know-to-which-value-i-should-define-posix-c-source
#include "chip8.h"
#include <stdio.h>
#include <time.h>

void render(chip8_t *chip8) {
  printf("\x1b[H");
  for (size_t i = 0; i < sizeof(chip8->gfx); i++) {
    if (i % 64 == 0) {
      printf("\n");
    }
    if (chip8->gfx[i] == 1) {
      printf("@");
    } else {
      printf(".");
    }
  }
  fflush(stdin);
}
int main() {
  chip8_t chip;
  chip8_initialise(&chip);
  chip8_loadGame(&chip, "res/roms/IBM Logo.ch8");

  printf("\033[?25l");
  struct timespec ts = {.tv_sec = 0, .tv_nsec = 1666666667};
  while (1) {
    chip8_emulateCycle(&chip);
    render(&chip);
    nanosleep(&ts, NULL);
  }
  return 0;
}
