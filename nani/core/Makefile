CC := gcc
VENDOR_DIR := ../../vendor
CFLAGS := -ggdb -O0 -I$(VENDOR_DIR)
LFLAGS := -ggdb
HEADERS := common.h lexer.h value.h parser.h codegen.h $(VENDOR_DIR)/c_utils/utils.h
OBJECTS := main.o lexer.o parser.o utils.o

all: nani

clean:
	rm -f calc *.o
	rm nani

nani: $(OBJECTS)
	$(CC) $(LFLAGS) -o nani $(OBJECTS)

main.o: main.c $(HEADERS)
	$(CC) $(CFLAGS) -c main.c

lexer.o: lexer.c $(HEADERS)
	$(CC) $(CFLAGS) -c lexer.c

parser.o: parser.c $(HEADERS)
	$(CC) $(CFLAGS) -c parser.c

utils.o: $(VENDOR_DIR)/c_utils/utils.c $(VENDOR_DIR)/c_utils/utils.h
	$(CC) $(CFLAGS) -c $(VENDOR_DIR)/c_utils/utils.c
