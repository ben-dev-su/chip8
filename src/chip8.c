#include "chip8.h"
#include "logging.h"
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

/* TODO: opcodes to implement
 * 0xCXNN generate a random  number
 * 0xEX9E skip if key
 * 0xEXA1 skip if key
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
    case 0x00EE:
      chip8->pc += 2;
      break;
    }
    break;
  case 0x1000:
    chip8->pc = chip8->opcode & 0x0FFF;
    break;
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
unsigned short getNNN(unsigned short opcode);
unsigned short getIndexX(unsigned short opcode) { return opcode >> 8 & 0x000F; }
unsigned short getIndexY(unsigned short opcode) { return opcode >> 4 & 0x000F; }
