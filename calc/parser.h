#ifndef CALC_PARSER_H
#define CALC_PARSER_H

#include "ast.h"
#include "common.h"
#include "lexer.h"

typedef struct {
    const token_list_t *tokens;
    int current;
    bool had_error;
} parser_t;

void parser_init(parser_t *parser, const token_list_t *tokens);
stmt_list_t parser_parse_program(parser_t *parser);

#endif
