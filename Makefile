CC=gcc
CFLAGS= \
	-Wall \
	-Wextra

TEST_C = main.c
TEST_PRE_T = test

DEF_C = def.c streplace.c
DEF_PRE_T = def

ifeq ($(OS),Windows_NT)
	TEST_T = $(TEST_PRE_T).exe
	DEF_T = $(DEF_PRE_T).exe
else
	TEST_T = $(TEST_PRE_T).out
	DEF_T = $(DEF_PRE_T)
endif

.PHONY: clean

# 'make release' to create 'def' for generating definitions

all: debug

release: CFLAGS += -O2
release: build

debug: CFLAGS += -ggdb -O0
debug: build

build:
	$(CC) -o $(DEF_T) $(DEF_C) $(CFLAGS)
	touch build

clean:
	rm -f collections.h $(TEST_T) $(DEF_T) DOCS.md build

# Test stuff

bootstrap: debug
	./$(DEF_T) -f collections.json -o collections.h

defh: release
	./$(DEF_T) -f def.json -o def.h

test: $(TEST_T)

$(TEST_T): bootstrap
	$(CC) -o $@ $(TEST_C) $(CFLAGS) -O2

# Misc

docs:
	python docs.py
