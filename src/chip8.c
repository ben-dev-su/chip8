#include "chip8.h"
#include "logging.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

void fetch(chip8_t *chip8);
void decode(chip8_t *chip8);
void execute();
long get_file_size(FILE *fp);
unsigned short getN(unsigned short opcode);
unsigned short getNN(unsigned short opcode);
unsigned short getNNN(unsigned short opcode);
unsigned short getIndexX(unsigned short opcode);
unsigned short getIndexY(unsigned short opcode);

/*
 * FONT data that will be stored in memory at position 0x50 to 0x9F
 */
static unsigned short PROGRAM_START_INDEX = 0x200;
static unsigned char CHIP8_FONTSET_STARTINDEX = 0x50;

/* commented out to silence the compiler. not sure if i even need it */
/* static unsigned char FONT_END_INDEX = 0x9F; */

/*
 * FONT data that will be stored in memory at position 0x50 to 0x9F
 */
static const unsigned char CHIP8_FONTSET[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

//  CHIP8 keypad   |   WASD
//_________________|____________
//    1	2	3	C      |  1	2	3	4
//    4	5	6	D      |  Q	W	E	R
//    7	8	9	E      |  A	S	D	F
//    A	0	B	F      |  Z	X	C	V

void chip8_initialise(chip8_t *chip8) {
  memset(chip8, 0, sizeof(*chip8));
  memcpy(&chip8->memory[CHIP8_FONTSET_STARTINDEX], CHIP8_FONTSET,
         sizeof(CHIP8_FONTSET));

  chip8->pc = PROGRAM_START_INDEX;
}

void chip8_loadGame(chip8_t *chip8, char *fileName) {
  printf("Filename: %s\n", fileName);
  FILE *fp = fopen(fileName, "rb");
  if (fp == NULL) {
    perror("fopen failed");
    return;
  }
  long file_size = get_file_size(fp);
  fread(chip8->memory + PROGRAM_START_INDEX, sizeof(unsigned char), file_size,
        fp);

  fclose(fp);
}

void chip8_emulateCycle(chip8_t *chip8) {
  fetch(chip8);
  decode(chip8);
  /* execute(); */
}

/*
 * @Brief Does fetch opcodes from memory, and increments the program counter.
 *
 */
void fetch(chip8_t *chip8) {
  if (chip8 == NULL) {
    printf("chip8 can't be NULL (forgot to initialise?): %p", chip8);
    free(chip8);
    exit(1);
  }

  unsigned short opcode;
  unsigned char first_byte = chip8->memory[chip8->pc];
  unsigned char second_byte = chip8->memory[chip8->pc + 1];
  opcode = first_byte << 8 | second_byte;
  chip8->opcode = opcode;
}

/* implemented opcodes
00E0: clear the screen
00EE: return from subroutine to address pulled from stack
1nnn: jump to address NNN
2nnn: push return address onto stack and call subroutine at address NNN
3xnn: skip next opcode if vX == NN
4xnn: skip next opcode if vX != NN
5xy0: skip next opcode if vX == vY
6xnn: set vX to NN
7xnn: add NN to vX
8xy0: set vX to the value of vY
8xy1: set vX to the result of bitwise vX OR vY [Quirk 5]
8xy2: set vX to the result of bitwise vX AND vY [Quirk 5]
8xy3: set vX to the result of bitwise vX XOR vY [Quirk 5]
8xy4: add vY to vX, vF is set to 1 if an overflow happened, to 0 if not, even if
X=F!

8xy5: subtract vY from vX, vF is set to 0 if an underflow happened, to 1 if not,
even if X=F!

8xy6: set vX to vY and shift vX one bit to the right, set vF to the bit shifted
out, even if X=F! [Quirk 6]

8xy7: set vX to the result of subtracting vX from vY, vF is set to 0 if an
underflow happened, to 1 if not, even if X=F!

8xyE: set vX to vY and shift vX one bit to the left, set vF to the bit shifted
out, even if X=F! [Quirk 6]

9xy0: skip next opcode if vX != vY
Annn: set I to NNN

Dxyn: draw 8xN pixel sprite at position vX, vY with data starting at the address
in I, I is not changed [Quirk 7] [Quirk 8] [Quirk 9] [Quirk 10]
 */

/* TODO: opcodes to implement

0nnn: jump to native assembler subroutine at 0xNNN
Bnnn: jump to address NNN + v0 Cxnn: set vx to a random value masked (bitwise
AND) with NN

Ex9E: skip next opcode if key in the lower 4 bits of vX is pressed
ExA1: skip next opcode if key in the lower 4 bits of vX is not pressed
Fx07: set vX to the value of the delay timer
Fx0A: wait for a key pressed and released and set vX to it
Fx15: set delay timer to vX
Fx18: set sound timer to vX, sound is played until timer reaches zero
Fx1E: add vX to I
Fx29: set I to the 5 line high hex sprite for the lowest nibble in vX
Fx33: write the value of vX as BCD value at the addresses I, I+1 and I+2
Fx55: write the content of v0 to vX at the memory pointed to by I, I is
incremented by X+1 [Quirk 12]

Fx65: read the bytes from memory pointed to by I into the registers v0 to vX, I
is incremented by X+1 [Quirk 12]
 */
void decode(chip8_t *chip8) {
  unsigned short op = chip8->opcode & 0xF000;
  // The 4 bit value
  // FIXME: unused
  unsigned short N;
  (void)N;
  // The 8 bit value
  // FIXME: unused
  unsigned short NN;
  (void)NN;
  // The 12 bit value
  // FIXME: unused
  unsigned short NNN;
  (void)NNN;
  // Index of a v register
  unsigned short index_x;
  // Index of a v register
  // FIXME: unused
  unsigned short index_y;
  (void)index_y;

  switch (chip8->opcode & 0xF000) {
  case 0x0000:
    op = chip8->opcode & 0x00FF;
    switch (op) {
    case 0x00E0:
      memset(chip8->gfx, 0, sizeof(chip8->gfx));
      chip8->pc += 2;
      break;
    case 0x00EE: {
      unsigned short address;
      pop(&chip8->stack, &address);
      chip8->pc = address;
      chip8->sp = chip8->stack.top;
      chip8->pc += 2;
      break;
    }
    }
    break;
  case 0x1000:
    chip8->pc = chip8->opcode & 0x0FFF;
    break;
  case 0x2000: {
    unsigned short NNN = getNNN(chip8->opcode);
    push(&chip8->stack, chip8->pc);
    chip8->sp = chip8->stack.top;
    chip8->pc = NNN;
    break;
  }
  case 0x3000: {
    unsigned short vx = chip8->v[getIndexX(chip8->opcode)];
    unsigned short NN = getNN(chip8->opcode);
    if (vx == NN) {
      chip8->pc += 2;
    }
    chip8->pc += 2;
    break;
  }
  case 0x4000: {
    unsigned short vx = chip8->v[getIndexX(chip8->opcode)];
    unsigned short NN = getNN(chip8->opcode);
    if (vx != NN) {
      chip8->pc += 2;
    }
    chip8->pc += 2;
    break;
  }
  case 0x5000: {
    unsigned short vx = chip8->v[getIndexX(chip8->opcode)];
    unsigned short vy = chip8->v[getIndexY(chip8->opcode)];
    if (vx == vy) {
      chip8->pc += 2;
    }
    chip8->pc += 2;
    break;
  }
  case 0x6000:
    index_x = getIndexX(chip8->opcode);
    NN = getNN(chip8->opcode);
    chip8->v[index_x] = NN;
    chip8->pc += 2;
    break;
  case 0x7000:
    index_x = getIndexX(chip8->opcode);
    NN = getNN(chip8->opcode);
    chip8->v[index_x] += NN;
    chip8->pc += 2;
    break;
  case 0xA000:
    chip8->I = chip8->opcode & 0x0FFF;
    chip8->pc += 2;
    break;
  case 0x8000: {
    op = getN(chip8->opcode);
    unsigned short index_x = getIndexX(chip8->opcode);
    unsigned short index_y = getIndexY(chip8->opcode);
    unsigned short vx = chip8->v[index_x];
    unsigned short vy = chip8->v[index_y];
    switch (op) {
    case 0x0:
      chip8->v[index_x] = vy;
      break;
    case 0x1:
      chip8->v[index_x] = vx | vy;
      break;
    case 0x2:
      chip8->v[index_x] = vx & vy;
      break;
    case 0x3:
      chip8->v[index_x] = vx ^ vy;
      break;
    case 0x4:
      if (vx + vy > 255) {
        chip8->v[0xf] = 1;
      } else {

        chip8->v[0xf] = 0;
      }
      chip8->v[index_x] += vy;
      break;
    case 0x5:
      if (vx >= vy) {
        chip8->v[0xF] = 1;
      } else {
        chip8->v[0xF] = 0;
      }
      chip8->v[index_x] -= vy;
      break;
    case 0x6:
      chip8->v[index_x] = vy;
      if ((chip8->v[index_x] & 0b00000001) == 1) {
        chip8->v[0xF] = 1;
      } else {
        chip8->v[0xF] = 0;
      }
      chip8->v[index_x] = chip8->v[index_x] >> 1;
      break;
    case 0x7:
      if (vy >= vx) {
        chip8->v[0xF] = 1;
      } else {
        chip8->v[0xF] = 0;
      }
      chip8->v[index_x] = vy - vx;
      break;
    case 0xE:
      chip8->v[index_x] = vy;
      if ((chip8->v[index_x] & 0b10000000) == 0b10000000) {
        chip8->v[0xF] = 1;
      } else {
        chip8->v[0xF] = 0;
      }
      chip8->v[index_x] = chip8->v[index_x] << 1;
      break;
    }
    chip8->pc += 2;
    break;
  }
  case 0x9000: {
    unsigned short vx = chip8->v[getIndexX(chip8->opcode)];
    unsigned short vy = chip8->v[getIndexY(chip8->opcode)];
    if (vx != vy) {
      chip8->pc += 2;
    }
    chip8->pc += 2;
    break;
  }
  case 0xD000: {
    unsigned char vx = getIndexX(chip8->opcode);
    unsigned char vy = getIndexY(chip8->opcode);
    unsigned char xpos = chip8->v[vx];
    unsigned char ypos = chip8->v[vy];
    unsigned char height = getN(chip8->opcode);
    unsigned char sprite;
    chip8->v[0xf] = 0;
    for (size_t row = 0; row < height; row++) {
      sprite = chip8->memory[chip8->I + row];
      for (size_t col = 0; col < 8; col++) {
        if (sprite & (0b10000000 >> col)) {
          // start index: width * y + x
          // index pos: row * width + col
          // = width * y + x + row * width + col
          if (chip8->gfx[(64 * (row + ypos)) + col + xpos] == 1) {
            chip8->v[0xf] = 1;
          } else {
            chip8->v[0xf] = 0;
          }
          chip8->gfx[(64 * (row + ypos)) + col + xpos] ^= 1;
        }
      }
    }
    chip8->pc += 2;
    break;
  }
  default:
    printf("Unknown opcode: 0x%x\n", chip8->opcode);
  }
#ifdef DEBUG
  log_chip8_state(chip8);
#endif
}

void testSprite() {}
/*
 * @Brief Returns the file size for a given file.
 */
long get_file_size(FILE *fp) {

  if (fp == NULL) {
    printf("file pointer can't NULL: %p", fp);
    exit(1);
  }

  long size;
  fseek(fp, 0, SEEK_END);
  size = ftell(fp);

  if (size == -1) {
    perror("something went wrong ");
    exit(1);
  }

  rewind(fp);

  return size;
}

unsigned short getN(unsigned short opcode) { return opcode & 0x000F; };
unsigned short getNN(unsigned short opcode) { return opcode & 0x00FF; }
unsigned short getNNN(unsigned short opcode) { return opcode & 0xFFF; }
unsigned short getIndexX(unsigned short opcode) { return opcode >> 8 & 0x000F; }
unsigned short getIndexY(unsigned short opcode) { return opcode >> 4 & 0x000F; }
