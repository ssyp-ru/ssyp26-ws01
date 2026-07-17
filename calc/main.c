#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "token.h"
#include "parser.h"
// token


int main() {

	FILE * f = fopen("highlight.txt", "r");

	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, 0, SEEK_SET);
    
	char *s = malloc(size +1);
	fread(s, 1, size, f);
	s[size] = '\0';

	token_array_t t;
	init_slice(&t);
	tokenize(s, &t);
	//token_output(&t);
	output(s, &t);

    lexer_t lexer;
    lexer.tokens = t;
    lexer.cur = 0;
    lexer.orig = s;

    expr_t *expr = p_expr(&lexer);
    print_ast(expr);

	destroy_slice(&t);
	fclose(f);
	free(s);
	
	return 0;
}
