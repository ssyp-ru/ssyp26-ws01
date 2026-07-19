CC := gcc
VENDOR_DIR := ../../vendor
RAYLIB_DIR := $(VENDOR_DIR)/raylib/src
RAYLIB_LIB := $(RAYLIB_DIR)/libraylib.a
CFLAGS := -ggdb -O0 -I$(VENDOR_DIR) -Wall -Wpedantic -Werror
LFLAGS := -ggdb

ifeq ($(SANITIZE),1)
CFLAGS += -fsanitize=address -fno-omit-frame-pointer
LFLAGS += -fsanitize=address -fno-omit-frame-pointer
endif
HEADERS := common.h lexer.h value.h parser.h interpreter.h codegen.h $(VENDOR_DIR)/c_utils/utils.h
OBJECTS := main.o lexer.o parser.o value.o interpreter.o utils.o common.o

.PHONY: all build clean raylib

all: build

build: nani

clean:
	rm -f calc *.o
	rm nani
	$(MAKE) -C $(RAYLIB_DIR) clean

nani: $(OBJECTS) raylib
	$(CC) $(LFLAGS) -o nani $(OBJECTS) $(RAYLIB_LIB)

raylib:
	$(MAKE) -C $(RAYLIB_DIR) RAYLIB_BUILD_MODE=DEBUG

main.o: main.c $(HEADERS)
	$(CC) $(CFLAGS) -c main.c

lexer.o: lexer.c $(HEADERS)
	$(CC) $(CFLAGS) -c lexer.c

parser.o: parser.c $(HEADERS)
	$(CC) $(CFLAGS) -c parser.c

value.o: value.c $(HEADERS)
	$(CC) $(CFLAGS) -c value.c

interpreter.o: interpreter.c $(HEADERS)
	$(CC) $(CFLAGS) -c interpreter.c

common.o: common.c $(HEADERS)
	$(CC) $(CFLAGS) -c common.c

utils.o: $(VENDOR_DIR)/c_utils/utils.c $(VENDOR_DIR)/c_utils/utils.h
	$(CC) $(CFLAGS) -c $(VENDOR_DIR)/c_utils/utils.c
