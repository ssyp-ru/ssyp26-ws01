CC := gcc
CFLAGS := -ggdb -O0
LFLAGS := -ggdb

all: calc

clean:
	rm -f calc *.o

calc: main.o lexer.o parser.o
	$(CC) $(LFLAGS) -o $@ $^

main.o: main.c lexer.h parser.h value.h
	$(CC) $(CFLAGS) -c -o $@ $<

lexer.o: lexer.c lexer.h
	$(CC) $(CFLAGS) -c -o $@ $<

parser.o: parser.c parser.h lexer.h value.h
	$(CC) $(CFLAGS) -c -o $@ $<

