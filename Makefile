CC = cc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Iinclude
TARGET = maze

SOURCES = $(wildcard src/*.c)
OBJECTS = $(SOURCES:src/%.c=build/%.o)

.PHONY: all clean run test

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

TEST_TARGET = build/tests
TEST_OBJECTS = $(filter-out build/main.o,$(OBJECTS))

$(TEST_TARGET): tests/test_maze.c tests/minunit.h $(TEST_OBJECTS)
	$(CC) $(CFLAGS) -Itests tests/test_maze.c $(TEST_OBJECTS) -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -rf build $(TARGET)
