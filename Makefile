CC=gcc
CFLAGS=\
	-Wall \
	-Wextra
TARGET=a.out

FILES=main.c

all: $(TARGET)

.PHONY: $(TARGET) clean

bootstrap:
	python def.py -f def.json -o def.h
	$(CC) -o a.out def.c $(CFLAGS)

generator:
	python def.py -f collections.json -o collections.h

$(TARGET): generator
	$(CC) -o $@ $(FILES) $(CFLAGS)

clean:
	rm -f a.out a.exe gen.* collections.h

docs:
	python docs.py
