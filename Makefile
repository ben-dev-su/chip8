CC = clang
CFLAGS = --std=c23 -Wall -Wextra -Werror
CHIP8 = chip8

build/main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c -o build/main.o

build/stack.o: src/stack.c
	$(CC) $(CFLAGS) -c src/stack.c -o build/stack.o

chip8: build/main.o build/stack.o
	$(CC) build/main.o build/stack.o -o bin/$(CHIP8)

clean:
	rm -f build/main.o bin/$(CHIP8)
