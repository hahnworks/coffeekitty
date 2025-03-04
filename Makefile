CC = gcc
CFLAGS = -Iinclude $(shell pkg-config --cflags libxml-2.0) -Wall -Wextra
LIBS = $(shell pkg-config --libs libxml-2.0) -lm
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TARGET = bin/coffeekitty

all: $(TARGET)

clean:
	rm -f src/*.o $(TARGET)

install: $(TARGET)
	mkdir -p /usr/local/bin
	cp $(TARGET) /usr/local/bin

$(TARGET): $(OBJ)
	mkdir -p bin
	$(CC) $(OBJ) $(LIBS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
