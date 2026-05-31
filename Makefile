CC = clang
CFLAGS = -std=c23 -Wall -Wextra -Werror -Wshadow -MMD -MP
LDLIBS = -lSDL3

CHIP8 = bin/chip8
SRCS = src/main.c src/stack.c src/chip8.c src/backend.c src/logging.c
OBJS = $(SRCS:src/%.c=build/%.o)

.PHONY: all debug release clean
all: debug

debug: CFLAGS += -g -DDEBUG
debug: $(CHIP8)

release: CFLAGS += -O2 -DNDEBUG
release: $(CHIP8)

$(CHIP8): $(OBJS) | bin
	$(CC) $(OBJS) -o $@ $(LDLIBS)

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build bin:
	mkdir -p $@

clean:
	rm -rfv build bin

-include $(OBJS:.o=.d)
