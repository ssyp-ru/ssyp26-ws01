CC := gcc
CFLAGS := -ggdb -O0
LFLAGS := -ggdb
HEADERS := common.h lexer.h value.h parser.h codegen.h
OBJECTS := main.o lexer.o parser.o

all: nani

clean:
	rm -f calc *.o
	rm nani

nani: main.o lexer.o parser.o
	$(CC) $(LFLAGS) -o nani $(OBJECTS)

main.o: main.c $(HEADERS)
	$(CC) $(CFLAGS) -c main.c

lexer.o: lexer.c $(HEADERS)
	$(CC) $(CFLAGS) -c lexer.c

parser.o: parser.c $(HEADERS)
	$(CC) $(CFLAGS) -c parser.c
