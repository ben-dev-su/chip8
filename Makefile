CC = clang
CFLAGS = --std=c23 -g -Wall -Wextra -Werror
CHIP8 = chip8
LDLIBS = -lSDL3

SRCS = src/main.c src/stack.c src/chip8.c src/backend.c
OBJS = $(SRCS:src/%.c=build/%.o)

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

$(CHIP8): $(OBJS) | bin
	$(CC) $(OBJS) -o bin/$(CHIP8) $(LDLIBS)

build bin:
	mkdir -p $@

clean:
	rm -rfv build bin

.PHONY: clean
