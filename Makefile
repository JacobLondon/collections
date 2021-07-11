CC=gcc
CFLAGS=\
	-Wall \
	-Wextra
TARGET=a.out

FILES=main.c

all: $(TARGET)

.PHONY: $(TARGET) clean

bootstrap:
	$(CC) -o def def.c $(CFLAGS) -O2
bootstrap-debug:
	$(CC) -o def def.c $(CFLAGS) -ggdb

generator: bootstrap
	./def -f collections.json -o collections.h

$(TARGET): generator
	$(CC) -o $@ $(FILES) $(CFLAGS) -O2
debug: generator
	$(CC) -o $@ $(FILES) $(CFLAGS) -ggdb

clean:
	rm -f a.out a.exe gen.* collections.h def

docs:
	python docs.py
