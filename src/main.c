#include "chip8.h"
#include <stdio.h>

int main() {
  chip8_t chip;
  chip8_initialise(&chip);
  chip8_loadGame(&chip, "res/roms/IBM Logo.ch8");

  chip8_emulateCycle(&chip);
  chip8_emulateCycle(&chip);
  chip8_emulateCycle(&chip);
  chip8_emulateCycle(&chip);
  chip8_emulateCycle(&chip);
  chip8_emulateCycle(&chip);
  chip8_emulateCycle(&chip);
  chip8_emulateCycle(&chip);

  printf("\n");
  return 0;
}
