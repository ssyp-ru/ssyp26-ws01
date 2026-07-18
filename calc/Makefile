main: main.o lexer.h lexer.o parser.o interpreter.o
	gcc main.o lexer.o parser.o interpreter.o -o main

parser.o: parser.c parser.h lexer.h value.h
	gcc -c parser.c

lexer.o: lexer.c lexer.h 
	gcc -c lexer.c

main.o: main.c lexer.h
	gcc -c main.c

interpreter.o: interpreter.c interpreter.h
	gcc -c interpreter.c


clean:
	rm *.o
	rm main
