#ifndef CALC_PARSER_H
#define CALC_PARSER_H

#include "ast.h"
#include "common.h"
#include "lexer.h"

typedef struct {
    const TokenList *tokens;
    int current;
    bool had_error;
} Parser;

void parser_init(Parser *parser, const TokenList *tokens);
StmtList parser_parse_program(Parser *parser);

#endif
