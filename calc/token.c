#include "stdio.h"
#include "lexer.h"

#define COLOR_RESET   "\x1b[0m"
#define COLOR_BLACK   "\x1b[30m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_WHITE   "\x1b[37m"

#define COLOR_BRIGHT_BLACK   "\x1b[90m"
#define COLOR_BRIGHT_RED     "\x1b[91m"
#define COLOR_BRIGHT_GREEN   "\x1b[92m"
#define COLOR_BRIGHT_YELLOW  "\x1b[93m"
#define COLOR_BRIGHT_BLUE    "\x1b[94m"
#define COLOR_BRIGHT_MAGENTA "\x1b[95m"
#define COLOR_BRIGHT_CYAN    "\x1b[96m"
#define COLOR_BRIGHT_WHITE   "\x1b[97m"

token_type getTokenType(char sym) {
	switch(sym) {
		case '+':
			return TOKEN_PLUS;
		case '-':
			return TOKEN_MINUS;
		case '*':
			return TOKEN_STAR;
		case '/':
			return TOKEN_SLASH;
		case '(':
			return TOKEN_LEFT_PAREN;
		case ')':
			return TOKEN_RIGHT_PAREN;
		default:
			if (sym >= '0' && sym <= '9') {
				return TOKEN_NUMBER;
			}
			return TOKEN_ERROR;
	}
}

void tokenize(char *text, token_array_t *a) {
	token_t curToken;
    for(int i = 0; text[i] != '\0'; i++) { 
        curToken.t = getTokenType(text[i]);
        curToken.start = i;
        curToken.finish = i;

        token_t *lastToken = &a->data[a->size - 1];
        if (i == 0 || lastToken->t != curToken.t) {
            push_back(a, curToken);
        } else {
            lastToken->finish++;
        }
    }
}
	

void output(char *text, token_array_t *t) {
	for(int i =0; i < t->size; i++) {
		token_t *curToken = &t->data[i];
		for(int tInd = curToken->start; tInd < curToken->finish + 1; tInd++) {
			switch(curToken->t) {
				case TOKEN_NUMBER:
					printf("%s", COLOR_CYAN);
					break;
				case TOKEN_PLUS:
					printf("%s", COLOR_RED);
					break;
				case TOKEN_MINUS:
					printf("%s", COLOR_GREEN);
					break;
				case TOKEN_STAR:
					printf("%s", COLOR_BLUE);
					break;
				case TOKEN_SLASH:
					printf("%s", COLOR_YELLOW);
					break;
				case TOKEN_LEFT_PAREN:
					printf("%s", COLOR_MAGENTA);
					break;
				case TOKEN_RIGHT_PAREN:
					printf("%s", COLOR_MAGENTA);
					break;
				case TOKEN_ERROR:
					printf("%s", COLOR_RESET);
					break;
                default:
                    break;
			}
			printf("%c", text[tInd]);
			printf("%s", COLOR_RESET);
		}
	}
}

void token_output(token_array_t *t) {
	for(int i = 0; i < t->size; i++) {
		token_t *curT = &t->data[i];
		printf("\n token index %d\n token type %d\n start %d\n finish %d\n\n", i, curT->t, curT->start, curT->finish);
	}	
}
