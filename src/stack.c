#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

int pop(stack_t *stack, unsigned short *result) {
  if (stack == NULL || result == NULL) {
    fprintf(stderr, "'stack' or 'result' can't be NULL\n");
    exit(EXIT_FAILURE);
  }

  if (stack->top == 0)
    return 0;

  *result = stack->arr[--stack->top];
  return 1;
}

int push(stack_t *stack, unsigned short value) {
  if (stack == NULL) {
    fprintf(stderr, "Stack can't be NULL\n");
    exit(EXIT_FAILURE);
  }

  if (stack->top > STACK_MAX_LEVEL) {
    return 0;
  }

  stack->arr[stack->top++] = value;

  return 1;
}
