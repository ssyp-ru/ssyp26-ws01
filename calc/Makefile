main: main.o lexer.h lexer.o parser.o
	gcc main.o lexer.o parser.o -o main

parser.o: parser.c parser.h lexer.h value.h
	gcc -c parser.c

lexer.o: lexer.c lexer.h 
	gcc -c lexer.c

main.o: main.c lexer.h
	gcc -c main.c

clean:
	rm *.o
	rm main
