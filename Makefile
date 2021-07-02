CC=gcc
CFLAGS=\
	-Wall \
	-Wextra
TARGET=a.out

FILES=main.c

all: $(TARGET)

.PHONY: clean

generator:
	python def.py -f def.json -o gen.h

$(TARGET): generator
	$(CC) -o $@ $(FILES) $(CFLAGS)

clean:
	rm -f a.out a.exe gen.*
