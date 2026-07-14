#pragma once

#include <stdbool.h>

typedef enum {
	NUMBER,
	SEMICOLON,
	LINE_COMMENT,

	ADD,
	MUL,
	DIV,
	MINUS,

	PAR_OPEN,
	PAR_CLOSE,

	BANG,
	TRUE,
	FALSE,
	AND,
	OR,

	EQU,
	NEQ,
	LESS,
	LESS_EQ,
	GREATER,
	GREATER_EQ,

	PRINT,
	ASSERT,

	ENDOFFILE,
} TokenKind;

typedef struct {
	int start;
	int len;
	TokenKind kind;
} Token;

typedef struct {
	Token* ptr;
	int length;
	int capacity;
	const char* code;
} Tokens;

bool tokenize(Tokens* tokens, const char* code);

