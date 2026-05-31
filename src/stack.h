#pragma once

#include <stdbool.h>
#include <stdint.h>

#define STACK_MAX_LEVEL 16

typedef struct stack {
  uint16_t arr[STACK_MAX_LEVEL];
  uint8_t top;
} stack_t;

/**
 * @brief Pops the top element from the stack.
 *
 * Removes the element on top and saves value to the result param.
 * Will either return true for success or false for failure.
 *
 *
 * @param stack Pointer to the stack.
 * @param result The value from the top of the stack.
 * @return returns true for success or false for failure.
 */
bool pop(stack_t *stack, uint16_t *result);

/**
 * @brief Push a given element on top of the stack.
 *
 * The given value is being pushed to the top of the stack.
 *
 * @param stack Pointer to the stack.
 * @param value The value to push.
 * @return returns true for success or false for failure.
 */
bool push(stack_t *stack, uint16_t value);
