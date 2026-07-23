CC := gcc
AR := ar
VENDOR_DIR := ../../vendor
RAYLIB_DIR := $(VENDOR_DIR)/raylib/src
RAYLIB_LIB := $(RAYLIB_DIR)/libraylib.a
UNAME_S := $(shell uname -s)
CFLAGS := -ggdb -O0 -I$(VENDOR_DIR) -Wall -Wpedantic -Werror -Wno-strict-prototypes
LFLAGS := -ggdb

ifeq ($(UNAME_S),Darwin)
RAYLIB_LFLAGS := -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework CoreVideo
endif
ifeq ($(UNAME_S),Linux)
RAYLIB_LFLAGS := -lGL -lc -lm -lpthread -ldl -lrt -lX11
endif
ifeq ($(WINDOWS),1)
RAYLIB_LFLAGS := -lm -mwindows -m64 -luser32 -lwinmm -lopengl32
CC := x86_64-w64-mingw32-gcc
AR := x86_64-w64-mingw32-ar
endif

ifeq ($(SANITIZE),1)
CFLAGS += -fsanitize=address -fno-omit-frame-pointer
LFLAGS += -fsanitize=address -fno-omit-frame-pointer
endif
HEADERS := common.h lexer.h value.h hash.h parser.h interpreter.h native.h codegen.h $(VENDOR_DIR)/c_utils/utils.h
OBJECTS := main.o lexer.o parser.o value.o hash.o interpreter.o native.o utils.o common.o

.PHONY: all build clean raylib

all: build

ifeq ($(WINDOWS),1)
build: nani.exe
else
build: nani
endif

clean:
	rm -f nani.exe nani *.o
	$(MAKE) -C $(RAYLIB_DIR) clean

ifeq ($(WINDOWS),1)
nani.exe: $(OBJECTS) raylib
	$(CC) $(LFLAGS) -o nani.exe $(OBJECTS) $(RAYLIB_LIB) $(RAYLIB_LFLAGS)

raylib:
	$(MAKE) -C $(RAYLIB_DIR) RAYLIB_BUILD_MODE=DEBUG TARGET_PLATFORM=PLATFORM_DESKTOP_WIN32 CC=$(CC) AR=$(AR)
else
nani: $(OBJECTS) raylib
	$(CC) $(LFLAGS) -o nani $(OBJECTS) $(RAYLIB_LIB) $(RAYLIB_LFLAGS)

raylib:
	$(MAKE) -C $(RAYLIB_DIR) RAYLIB_BUILD_MODE=DEBUG
endif

main.o: main.c $(HEADERS)
	$(CC) $(CFLAGS) -c main.c

lexer.o: lexer.c $(HEADERS)
	$(CC) $(CFLAGS) -c lexer.c

parser.o: parser.c $(HEADERS)
	$(CC) $(CFLAGS) -c parser.c

value.o: value.c $(HEADERS)
	$(CC) $(CFLAGS) -c value.c

hash.o: hash.c $(HEADERS)
	$(CC) $(CFLAGS) -c hash.c

interpreter.o: interpreter.c $(HEADERS)
	$(CC) $(CFLAGS) -c interpreter.c

native.o: native.c $(HEADERS) $(RAYLIB_DIR)/raylib.h
	$(CC) $(CFLAGS) -c native.c

common.o: common.c $(HEADERS)
	$(CC) $(CFLAGS) -c common.c

utils.o: $(VENDOR_DIR)/c_utils/utils.c $(VENDOR_DIR)/c_utils/utils.h
	$(CC) $(CFLAGS) -c $(VENDOR_DIR)/c_utils/utils.c
