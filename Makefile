CC=gcc
CFLAGS=\
	-Wall \
	-Wextra
TARGET=a.out

FILES=main.c

all: $(TARGET)

.PHONY: clean

generator:
	python def.py -f collections.json -o collections.h

$(TARGET): generator
	$(CC) -o $@ $(FILES) $(CFLAGS)

clean:
	rm -f a.out a.exe gen.* collections.h

docs:
	python docs.py
