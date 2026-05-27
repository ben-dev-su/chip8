#include "chip8.h"
#include <stdio.h>

void log_chip8_state(const chip8_t *chip8) {
  printf("PC=0x%04x\t"
         "OP=0x%04x\tI=0x%04x\tSP=0x%04x\t"
         "v[0:0x%02x\t1:0x%02x\t2:0x%02x\t3:0x%02x\t4:0x%02x\t5:0x%02x\t"
         "6:0x%02x\t7:0x%02x\t8:0x%02x\t9:0x%02x\tA:0x%02x\t"
         "B:0x%02x\tC:0x%02x\tD:0x%02x\tE:0x%02x\tF:0x%02x\n",
         chip8->pc, chip8->opcode, chip8->I, chip8->sp, chip8->v[0],
         chip8->v[1], chip8->v[2], chip8->v[3], chip8->v[4], chip8->v[5],
         chip8->v[6], chip8->v[7], chip8->v[8], chip8->v[9], chip8->v[0xA],
         chip8->v[0xB], chip8->v[0xC], chip8->v[0xD], chip8->v[0xE],
         chip8->v[0xF]);
}
