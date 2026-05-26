#pragma once

#define STACK_MAX_LEVEL 16

typedef struct Stack {
  unsigned char top;
  unsigned short arr[STACK_MAX_LEVEL];
} stack_t;

/**
 * @brief Pops the top element from the stack.
 *
 * Removes the element on top and saves value to the result param.
 * Will either return 1 for success or 0 for failure.
 *
 *
 * @param stack Pointer to the stack.
 * @param The value from the top of the stack.
 * @return returns 1 for success or 0 for failure.
 */
int pop(stack_t *stack, unsigned short *result);

/**
 * @brief Push a given element on top of the stack.
 *
 * The given value is being pushed to the top of the stack.
 *
 * @param stack Pointer to the stack.
 * @param value The value to push.
 * @return returns 1 for success or 0 for failure.
 */
int push(stack_t *stack, unsigned short value);
unsigned short peek(stack_t *s);
