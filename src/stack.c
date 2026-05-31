#include "stack.h"
#include <stdbool.h>
#include <stdio.h>

bool pop(stack_t *stack, uint16_t *result) {
  if (stack == NULL || result == NULL) {
    fprintf(stderr, "'stack' or 'result' can't be NULL\n");
    return false;
  }

  if (stack->top == 0) {
    fprintf(stderr, "Stack underflow\n");
    return false;
  }

  *result = stack->arr[--stack->top];
  return true;
}

bool push(stack_t *stack, uint16_t value) {
  if (stack == NULL) {
    fprintf(stderr, "Stack can't be NULL\n");
    return false;
  }

  if (stack->top >= STACK_MAX_LEVEL) {
    fprintf(stderr, "Stack overflow\n");
    return false;
  }

  stack->arr[stack->top++] = value;

  return true;
}
