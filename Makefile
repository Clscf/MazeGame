CC = cc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Iinclude
TARGET = maze

SOURCES = $(wildcard src/*.c)
OBJECTS = $(SOURCES:src/%.c=build/%.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build $(TARGET)
