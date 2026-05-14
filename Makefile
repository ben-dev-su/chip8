CC = clang
CFLAGS = --std=c23 -Wall -Wextra -Werror
CHIP8 = chip8

SRCS = src/main.c src/stack.c src/chip8.c
OBJS = $(SRCS:src/%.c=build/%.o)

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

$(CHIP8): $(OBJS) | bin
	$(CC) $(OBJS) -o bin/$(CHIP8)

build bin:
	mkdir -p $@

clean:
	rm -rfv build bin

.PHONY: clean
