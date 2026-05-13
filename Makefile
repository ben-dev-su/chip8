CC = clang
CFLAGS = --std=c23 -Wall -Wextra -Werror
CHIP8 = chip8

build/main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c -o build/main.o

build/stack.o: src/stack.c
	$(CC) $(CFLAGS) -c src/stack.c -o build/stack.o

build/chip8.o: src/chip8.c
	$(CC) $(CFLAGS) -c src/chip8.c -o build/chip8.o

chip8: build/main.o build/stack.o build/chip8.o
	$(CC) build/main.o build/stack.o build/chip8.o -o bin/$(CHIP8)

clean:
	rm -f build/*.o bin/$(CHIP8)
