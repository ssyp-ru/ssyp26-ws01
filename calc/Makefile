main: main.o array.h array.o token.h token.o parser.h parser.o interpretator.o
	gcc main.o parser_1.o array.o token.o interpretator.o -o main -g

array.o: array.c array.h token.h
	gcc -c array.c -g

token.o: token.c token.h array.h
	gcc -c token.c -g

parser.o: parser_1.c parser.h array.h
	gcc -c parser_1.c -g

main.o: parser.h array.h token.h main.c
	gcc -c main.c -g

interpretator.o: interpretator.c array.h value.h token.h parser.h
	gcc -c interpretator.c -g

clean:
	rm *.o
	rm main

