#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fetch();
void decode();
void execute();
long get_file_size(FILE *fp);

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

/* void chip8_emulateCycle(chip8_t *chip8) { */
/*   fetch(); */
/*   decode(); */
/*   execute(); */
/* } */

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
