#include "chip8.h"
#include <stdio.h>

void log_chip8_state(const chip8_t *chip8) {
  fprintf(stderr,
          "+---------------------------------------------------------+\n"
          "| PC 0x%04X   I 0x%04X   OP 0x%04X   DT 0x%02X   ST 0x%02X    |\n"
          "+---------------------------------------------------------+\n",
          chip8->pc, chip8->I, chip8->opcode, chip8->delay_timer,
          chip8->sound_timer);

  for (size_t i = 0; i < 16; i++) {
    fprintf(stderr, "| V%zX 0x%02X  ", i, chip8->v[i]);
    if (i % 4 == 3) {
      fprintf(stderr, "|\n");
    }
  }

  fprintf(stderr,
          "+---------------------------------------------------------+\n");

  fprintf(stderr, "| Stack (depth %2u): ", chip8->stack.top);
  for (size_t i = 0; i < chip8->stack.top; i++) {
    fprintf(stderr, "0x%03X ", chip8->stack.arr[i]);
  }
  fprintf(stderr, "\n");

  fprintf(stderr,
          "+---------------------------------------------------------+\n\n");
}
