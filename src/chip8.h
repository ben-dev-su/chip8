#include "stack.h"

/**
 *  @brief The Chip8 emulator struct holding all data
 *
 */
typedef struct Chip8 {
  stack_t stack;              ///< our stack
  unsigned short opcode;      ///< current opcode
  unsigned short I;           ///< index register
  unsigned short pc;          ///< program counter
  unsigned short sp;          ///< stack pointer
  unsigned char delay_timer;  ///< delay timer
  unsigned char sound_timer;  ///< sound timer
  unsigned char memory[4096]; ///< memory
  unsigned char v[16];        ///< CPU registers V0 to VF
  unsigned char gfx[64 * 32]; ///< screen
  unsigned char keys[16];     ///< keypad
} chip8_t;

/*
 * @Brief Initialise the Chip8.
 */
void chip8_initialise(chip8_t *chip8);
/*
 * @Brief Emulates a cycle.
 */
void chip8_emulateCycle(chip8_t *chip8);

/*
 * @Brief Loads a Chip8 program into the Chip8's memory.
 */
void chip8_loadGame(chip8_t *chip8, char *fileName);
