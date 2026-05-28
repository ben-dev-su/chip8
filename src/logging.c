#include "chip8.h"
#include <stdio.h>

void log_chip8_state(const chip8_t *chip8) {
  printf("PC=0x%04x "
         "OP=0x%04x I=0x%04x SP=0x%04x\t"
         "v[0:0x%02x 1:0x%02x 2:0x%02x 3:0x%02x 4:0x%02x 5:0x%02x "
         "6:0x%02x 7:0x%02x 8:0x%02x 9:0x%02x A:0x%02x "
         "B:0x%02x C:0x%02x D:0x%02x E:0x%02x F:0x%02x\n",
         chip8->pc, chip8->opcode, chip8->I, chip8->sp, chip8->v[0],
         chip8->v[1], chip8->v[2], chip8->v[3], chip8->v[4], chip8->v[5],
         chip8->v[6], chip8->v[7], chip8->v[8], chip8->v[9], chip8->v[0xA],
         chip8->v[0xB], chip8->v[0xC], chip8->v[0xD], chip8->v[0xE],
         chip8->v[0xF]);
}
