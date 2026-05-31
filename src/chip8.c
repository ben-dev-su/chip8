#include "chip8.h"
#include "logging.h"
#include "stack.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FONTSET_START_INDEX (0x50)
#define PROGRAM_START_INDEX (0x200)

typedef struct decoded_instruction {
  uint16_t type;
  uint16_t nnn;
  uint8_t nn;
  uint8_t n;
  uint8_t reg_x;
  uint8_t reg_y;
} decoded_instruction_t;

/*
 * FONT data that will be stored in memory at position 0x50 to 0x9F
 */
static const uint8_t CHIP8_FONTSET[] = {
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

static void fetch(chip8_t *chip8);
static void decode(const chip8_t *chip8, decoded_instruction_t *instr);
static bool execute(chip8_t *chip8, decoded_instruction_t *instr);

/*
 * @brief Returns the file size for a given file.
 */
static bool get_file_size(FILE *fp, long *file_size) {
  if (fseek(fp, 0, SEEK_END) != 0) {
    perror("fseek");
    return false;
  }
  *file_size = ftell(fp);

  if (*file_size == -1L) {
    perror("ftell");
    return false;
  }

  rewind(fp);
  return true;
}

//  CHIP8 keypad   |   WASD
//_________________|____________
//    1	2	3	C      |  1	2	3	4
//    4	5	6	D      |  Q	W	E	R
//    7	8	9	E      |  A	S	D	F
//    A	0	B	F      |  Z	X	C	V

void chip8_initialise(chip8_t *chip8) {
  memset(chip8, 0, sizeof(*chip8));
  memcpy(&chip8->memory[FONTSET_START_INDEX], CHIP8_FONTSET,
         sizeof(CHIP8_FONTSET));

  chip8->pc = PROGRAM_START_INDEX;
}

bool chip8_load_game(chip8_t *chip8, const char *file_name) {
  FILE *fp = fopen(file_name, "rb");
  if (fp == NULL) {
    perror("fopen");
    return false;
  }
  long file_size;
  if (!get_file_size(fp, &file_size)) {
    fclose(fp);
    return false;
  }
  if ((size_t)file_size > sizeof(chip8->memory) - PROGRAM_START_INDEX) {
    fclose(fp);
    fprintf(stderr, "Read file exceeds available memory: %ld\n", file_size);
    return false;
  }
  size_t read = fread(chip8->memory + PROGRAM_START_INDEX, sizeof(uint8_t),
                      file_size, fp);
  if (read != (size_t)file_size) {
    perror("fread");
    fclose(fp);
    return false;
  }
  fclose(fp);
  return true;
}

bool chip8_emulate_cycle(chip8_t *chip8) {
  if (chip8 == NULL) {
    return false;
  }

  decoded_instruction_t instr = {0};
  fetch(chip8);
  decode(chip8, &instr);
  if (!execute(chip8, &instr)) {
    return false;
  }

#ifdef DEBUG
  log_chip8_state(chip8);
#endif
  return true;
}

/*
 * @brief Does fetch opcodes from memory, and increments the program counter.
 *
 */
static void fetch(chip8_t *chip8) {
  uint8_t first_byte = chip8->memory[chip8->pc];
  uint8_t second_byte = chip8->memory[chip8->pc + 1];
  chip8->opcode = (first_byte << 8) | second_byte;
  chip8->pc += 2;
}

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
Fx29: set I to the 5 line high hex sprite for the lowest nibble in vX
 */
static void decode(const chip8_t *chip8, decoded_instruction_t *instr) {
  instr->type = chip8->opcode & 0xF000;
  instr->n = chip8->opcode & 0x000F;
  instr->nn = chip8->opcode & 0x00FF;
  instr->nnn = chip8->opcode & 0x0FFF;
  instr->reg_x = (chip8->opcode >> 8) & 0x000F;
  instr->reg_y = (chip8->opcode >> 4) & 0x000F;
}

static bool execute(chip8_t *chip8, decoded_instruction_t *instr) {
  switch (instr->type) {
  case 0x0000: {
    uint16_t subop = instr->nn;
    switch (subop) {

      // 00E0: clear the screen
    case 0x00E0:
      memset(chip8->display, 0, sizeof(chip8->display));
      break;

      // 00EE: return from subroutine to address pulled from stack
    case 0x00EE:
      if (!pop(&chip8->stack, &chip8->pc)) {
        return false;
      };
      break;

    default:
      fprintf(stderr, "Unknown opcode: %u\n", subop);
    }
  } break;

    // 1nnn: jump to address NNN
  case 0x1000:
    chip8->pc = instr->nnn;
    break;

    // 2nnn: push return address onto stack and call subroutine at address NNN
  case 0x2000:
    if (!push(&chip8->stack, chip8->pc)) {
      return false;
    }
    chip8->pc = instr->nnn;
    break;

    // 3xnn: skip next opcode if vX == NN
  case 0x3000: {
    uint8_t vx = chip8->v[instr->reg_x];
    if (vx == instr->nn) {
      chip8->pc += 2;
    }
    break;
  }

    // 4xnn: skip next opcode if vX != NN
  case 0x4000: {
    uint8_t vx = chip8->v[instr->reg_x];
    if (vx != instr->nn) {
      chip8->pc += 2;
    }
    break;
  }

    // 5xy0: skip next opcode if vX == vY
  case 0x5000: {
    uint8_t vx = chip8->v[instr->reg_x];
    uint8_t vy = chip8->v[instr->reg_y];
    if (vx == vy) {
      chip8->pc += 2;
    }
    break;
  }

    // 6xnn: set vX to NN
  case 0x6000:
    chip8->v[instr->reg_x] = instr->nn;
    break;

    // 7xnn: add NN to vX
  case 0x7000:
    chip8->v[instr->reg_x] += instr->nn;
    break;

    // Annn: set I to NNN
  case 0xA000:
    chip8->I = instr->nnn;
    break;
  case 0x8000: {
    uint16_t subop = instr->n;
    uint8_t vx = chip8->v[instr->reg_x];
    uint8_t vy = chip8->v[instr->reg_y];
    switch (subop) {

      // 8xy0: set vX to the value of vY
    case 0x0:
      chip8->v[instr->reg_x] = vy;
      break;

      // 8xy1: set vX to the result of bitwise vX OR vY [Quirk 5]
    case 0x1:
      chip8->v[instr->reg_x] = vx | vy;
      break;

      // 8xy2: set vX to the result of bitwise vX AND vY [Quirk 5]
    case 0x2:
      chip8->v[instr->reg_x] = vx & vy;
      break;

      // 8xy3: set vX to the result of bitwise vX XOR vY [Quirk 5]
    case 0x3:
      chip8->v[instr->reg_x] = vx ^ vy;
      break;

      // 8xy4: add vY to vX, vF is set to 1 if an overflow happened, to 0 if
      // not, even if X=F!
    case 0x4:
      chip8->v[0xF] = vx + vy > 255;
      chip8->v[instr->reg_x] += vy;
      break;

      // 8xy5: subtract vY from vX, vF is set to 0 if an underflow happened, to
      // 1 if not, even if X=F!
    case 0x5:
      chip8->v[0xF] = vx >= vy;
      chip8->v[instr->reg_x] -= vy;
      break;

      // 8xy6: set vX to vY and shift vX one bit to the right, set vF to the bit
      // shifted out, even if X=F! [Quirk 6]
    case 0x6:
      chip8->v[instr->reg_x] = vy;
      chip8->v[0xF] = chip8->v[instr->reg_x] & 0x01;
      chip8->v[instr->reg_x] = chip8->v[instr->reg_x] >> 1;
      break;

      // 8xy7: set vX to the result of subtracting vX from vY, vF is set to 0 if
      // an underflow happened, to 1 if not, even if X=F!
    case 0x7:
      chip8->v[0xF] = vy >= vx;
      chip8->v[instr->reg_x] = vy - vx;
      break;

      // 8xyE: set vX to vY and shift vX one bit to the left, set vF to the bit
      // shifted out, even if X=F! [Quirk 6]
    case 0xE:
      chip8->v[instr->reg_x] = vy;
      chip8->v[0xF] = (chip8->v[instr->reg_x] & 0x80) == 0x80;
      chip8->v[instr->reg_x] = chip8->v[instr->reg_x] << 1;
      break;
    default:
      fprintf(stderr, "Unknown opcode: %u\n", subop);
    }
    break;
  }

    // 9xy0: skip next opcode if vX != vY
  case 0x9000: {
    uint8_t vx = chip8->v[instr->reg_x];
    uint8_t vy = chip8->v[instr->reg_y];
    if (vx != vy) {
      chip8->pc += 2;
    }
    break;
  }

    // Dxyn: draw 8xN pixel sprite at position vX, vY with data starting at the
    // address in I, I is not changed [Quirk 7] [Quirk 8] [Quirk 9] [Quirk 10]
  case 0xD000: {
    uint8_t xpos = chip8->v[instr->reg_x];
    uint8_t ypos = chip8->v[instr->reg_y];
    uint8_t height = instr->n;
    chip8->v[0xF] = 0;
    for (size_t row = 0; row < height; row++) {
      uint8_t sprite = chip8->memory[chip8->I + row];
      for (size_t col = 0; col < 8; col++) {
        if (sprite & (0x80 >> col)) {
          // start index: width * y + x
          // index pos: row * width + col
          // = width * y + x + row * width + col
          uint16_t pixel_idx =
              (CHIP8_DISPLAY_WIDTH * (row + ypos)) + col + xpos;
          chip8->v[0xF] |= chip8->display[pixel_idx] == 1;
          chip8->display[pixel_idx] ^= 1;
        }
      }
    }
    break;
  }
  case 0xF000: {
    uint16_t subop = instr->nn;
    switch (subop) {

      // Fx33: write the value of vX as BCD value at the addresses I, I+1 and
      // I+2
    case 0x33: {
      uint8_t vx = chip8->v[instr->reg_x];
      chip8->memory[chip8->I] = vx / 100;
      chip8->memory[chip8->I + 1] = (vx % 100) / 10;
      chip8->memory[chip8->I + 2] = vx % 10;
      break;
    }

      // Fx55: write the content of v0 to vX at the memory pointed to by I, I is
      // incremented by X+1 [Quirk 12]
    case 0x55:
      memcpy(&chip8->memory[chip8->I], chip8->v, instr->reg_x + 1);
      chip8->I += instr->reg_x + 1;
      break;

      // Fx65: read the bytes from memory pointed to by I into the registers v0
      // to vX, I is incremented by X+1 [Quirk 12]
    case 0x65:
      memcpy(chip8->v, &chip8->memory[chip8->I], instr->reg_x + 1);
      chip8->I += instr->reg_x + 1;
      break;

      // Fx1E: add vX to I
    case 0x1E:
      chip8->I += chip8->v[instr->reg_x];
      break;
    default:
      fprintf(stderr, "Unknown opcode: %u\n", subop);
    }
  } break;
  default:
    fprintf(stderr, "Unknown opcode: %u\n", instr->type);
  }
  return true;
}
