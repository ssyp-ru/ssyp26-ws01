main: main.o lexer.h lexer.o token.h token.o parser.o
	gcc main.o lexer.o token.o parser.o -o main

parser.o: parser.c parser.h lexer.h value.h
	gcc -c parser.c

lexer.o: lexer.c lexer.h token.h
	gcc -c lexer.c

token.o: token.c token.h lexer.h
	gcc -c token.c

main.o: main.c token.h lexer.h
	gcc -c main.c

clean:
	rm *.o
	rm main
