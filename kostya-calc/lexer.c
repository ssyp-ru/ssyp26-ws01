#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void add_token(Tokens* tokens, int i, int len, TokenKind kind) {
	if (!tokens->ptr) {
		tokens->length = 0;
		tokens->capacity = 16;
		tokens->ptr = malloc(tokens->capacity * sizeof(Token));
	}

	if (tokens->length == tokens->capacity) {
		tokens->capacity *= 2;
		tokens->ptr = realloc(tokens->ptr, tokens->capacity * sizeof(Token));
	}

	Token* tok = &tokens->ptr[tokens->length++];
	tok->start = i;
	tok->len = len;
	tok->kind = kind;
}

typedef struct {
	const char* pattern;
	TokenKind kind;
	int len;
} Pattern;

// longer patterns should go first
#define P(pat, kin) { .pattern = (pat), .kind = (kin), .len = sizeof(pat) - 1 }
static Pattern patterns[] = {
	P("assert", ASSERT),
	P("print", PRINT),
	P("false", FALSE),
	P("true", TRUE),
	P("&&", AND),
	P("||", OR),
	P("==", EQU),
	P("!=", NEQ),
	P("<=", LESS_EQ),
	P(">=", GREATER_EQ),
	P("<", LESS),
	P(">", GREATER),
	P("+", ADD),
	P("-", MINUS),
	P("*", MUL),
	P("/", DIV),
	P("!", BANG),
	P("(", PAR_OPEN),
	P(")", PAR_CLOSE),
	P(";", SEMICOLON),
};
#undef P

bool tokenize(Tokens* tokens, const char* code) {
	int i = 0;
	while (code[i]) {
		char c = code[i];

		// special cases
		if (c >= '0' && c <= '9') { // number
			int len = 0;
			bool seen_point = false;
			while (code[i + len] >= '0' && code[i + len] <= '9' || (!seen_point && code[i + len] == '.')) {
				if (code[i + len] == '.') seen_point = true;
				len++;
			}

			add_token(tokens, i, len, NUMBER);
			i += len;

			continue;
		} else if (c == '/' && code[i + 1] == '/') { // line comment
			int len = 1;
			while (code[i + len] != '\n') len++;

			add_token(tokens, i, len, LINE_COMMENT);
			i += len;

			continue;
		} else if (c == ' ' || c == '\t' || c == '\n') { // whitespace - skipping
			i++;
			continue;
		} 

		for (int pi = 0; pi < sizeof(patterns) / sizeof(Pattern); pi++) {
			Pattern* pat = &patterns[pi];

			if (!memcmp(pat->pattern, &code[i], pat->len)) {
				add_token(tokens, i, pat->len, pat->kind);
				i += pat->len;
				goto loop_end;
			}
		}

		printf("Invalid char at position %d: '%c'\n", i, c);
		return false;

loop_end: // im sorry
	}

	add_token(tokens, 0, 0, ENDOFFILE);
	tokens->code = code;

	return true;
}

