#pragma once

#include "stack.h"
#include <stdbool.h>
#include <stdint.h>

#define CHIP8_MEMORY_SIZE (4096)
#define CHIP8_NUM_REGISTERS (16)
#define CHIP8_DISPLAY_WIDTH (64)
#define CHIP8_DISPLAY_HEIGHT (32)
#define CHIP8_NUM_KEYS (16)

/**
 *  @brief The Chip8 emulator struct holding all data
 *
 */
typedef struct chip8 {
  stack_t stack;                     ///< our stack
  uint16_t opcode;                   ///< current opcode
  uint16_t I;                        ///< index register
  uint16_t pc;                       ///< program counter
  uint8_t delay_timer;               ///< delay timer
  uint8_t sound_timer;               ///< sound timer
  uint8_t memory[CHIP8_MEMORY_SIZE]; ///< memory
  uint8_t v[CHIP8_NUM_REGISTERS];    ///< CPU registers V0 to VF
  uint8_t display[CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT]; ///< screen
  uint8_t keys[CHIP8_NUM_KEYS];                                ///< keypad
} chip8_t;

/*
 * @brief Initialise the Chip8.
 */
void chip8_initialise(chip8_t *chip8);
/*
 * @brief Emulates a cycle.
 */
bool chip8_emulate_cycle(chip8_t *chip8);

/*
 * @brief Loads a Chip8 program into the Chip8's memory.
 */
bool chip8_load_game(chip8_t *chip8, const char *file_name);
