#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool is_digit(char c) {
	return c >= '0' && c <= '9';
}

static bool is_alpha(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static bool is_alnum(char c) {
	return is_digit(c) || is_alpha(c);
}

static void add_token(tokens_t* tokens, const char* start, int len, int line, token_kind_t type) {
	if (!tokens->ptr) {
		tokens->length = 0;
		tokens->capacity = 16;
		tokens->ptr = malloc(tokens->capacity * sizeof(token_t));
	}

	if (tokens->length == tokens->capacity) {
		tokens->capacity *= 2;
		tokens->ptr = realloc(tokens->ptr, tokens->capacity * sizeof(token_t));
	}

	token_t* tok = &tokens->ptr[tokens->length++];
	tok->start = start;
	tok->length = len;
	tok->type = type;
    tok->line = line;
}

typedef struct {
	const char* pattern;
	token_kind_t type;
	int len;
} pattern_t;

// longer patterns should go first
#define P(pat, kin) { .pattern = (pat), .type = (kin), .len = sizeof(pat) - 1 }
static pattern_t patterns[] = {
    P("return", TOKEN_RETURN),
    P("struct", TOKEN_STRUCT),
    P("assert", TOKEN_ASSERT),
    P("print", TOKEN_PRINT),
    P("while", TOKEN_WHILE),
    P("false", TOKEN_FALSE),
    P("true", TOKEN_TRUE),
    P("else", TOKEN_ELSE),
    P("this", TOKEN_THIS),
    P("and", TOKEN_AND),
    P("for", TOKEN_FOR),
    P("let", TOKEN_LET),
    P("nil", TOKEN_NIL),
    P("fn", TOKEN_FN),
    P("if", TOKEN_IF),
    P("or", TOKEN_OR),
    P("==", TOKEN_EQ_EQ),
    P("!=", TOKEN_BANG_EQ),
    P("<=", TOKEN_LESS_EQ),
    P(">=", TOKEN_GREATER_EQ),
    P("(", TOKEN_LEFT_PAREN),
    P(")", TOKEN_RIGHT_PAREN),
    P("{", TOKEN_LEFT_BRACE),
    P("}", TOKEN_RIGHT_BRACE),
    P(",", TOKEN_COMMA),
    P(".", TOKEN_DOT),
    P(";", TOKEN_SEMICOLON),
    P("+", TOKEN_PLUS),
    P("-", TOKEN_MINUS),
    P("*", TOKEN_STAR),
    P("/", TOKEN_SLASH),
    P("!", TOKEN_BANG),
    P("=", TOKEN_EQ),
    P("<", TOKEN_LESS),
    P(">", TOKEN_GREATER),
};
#undef P

bool tokenize(tokens_t* tokens, const char* code) {
	int i = 0;
    int line = 1;

	while (code[i]) {
		char c = code[i];

		// special cases
		if (is_digit(c)) { // number
			int len = 0;
			bool seen_point = false;
			while (is_digit(code[i + len]) || (!seen_point && code[i + len] == '.')) {
				if (code[i + len] == '.') seen_point = true;
				len++;
			}

			add_token(tokens, &code[i], len, TOKEN_NUMBER, line);
			i += len;

			continue;
		} else if (c == '"') { // string
            // TODO: escape sequences

            int len = 1; // left quote
			while (code[i + len] != '"') len++;
            len++; // right quote

			add_token(tokens, &code[i], len, TOKEN_STRING, line);
			i += len;

			continue;

        } else if (is_alpha(c) || c == '_') { // identifier
			int len = 0;
			while (is_alnum(code[i + len]) || code[i + len] == '_') len++;

			add_token(tokens, &code[i], len, TOKEN_IDENTIFIER, line);
			i += len;

			continue;
		} else if (c == '/' && code[i + 1] == '/') { // line comment
			int len = 1;
			while (code[i + len] != '\n') len++;

            // TODO: add a comment token for syntax highlighting purposes
			// add_token(tokens, &code[i], len, TOKEN_COMMENT, line);
			i += len;

			continue;
		} else if (c == ' ' || c == '\t') { // whitespace - skipping
			i++;
			continue;
		} else if (c == '\n') { // newline
            line++;
            i++;
            continue;
        }

		for (int pi = 0; pi < sizeof(patterns) / sizeof(pattern_t); pi++) {
			pattern_t* pat = &patterns[pi];

			if (!memcmp(pat->pattern, &code[i], pat->len)) {
				add_token(tokens, &code[i], pat->len, pat->type, line);
				i += pat->len;
				goto loop_end;
			}
		}

		printf("Invalid char on line %d: '%c'\n", line, c);
		return false;

loop_end: // im sorry
	}

	return true;
}
