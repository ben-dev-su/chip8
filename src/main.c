#include "chip8.h"
#include <stdio.h>

int main() {
  chip8_t chip;
  chip8_initialise(&chip);
  chip8_loadGame(&chip, "res/roms/IBM Logo.ch8");

  printf("chip8 pc: %d\n", chip.pc);
  printf("chip8 pc: %02x\n", chip.memory[512]);
  printf("chip8 pc: %02x\n", chip.memory[513]);
  printf("chip8 pc: %02x\n", chip.memory[554]);
  return 0;
}
