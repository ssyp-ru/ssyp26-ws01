#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "lexer.h"
#include "parser.h"
#include "value.h"
#include "c_utils/utils.h"

static char* read_full(FILE* fp) {
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* output = (char*) malloc(size + 1);
    fread(output, 1, size, fp);
    output[size] = 0;

    return output;
}

[[noreturn]] static void rterr(const char* text) {
    log_error("Runtime error: %s\n", text);
    exit(1);
}

static value_t eval(expr_t* expr) {
    if (expr->type == EXPR_LITERAL) {
        value_t val;

        switch (expr->value.literal.type) {
        case LITERAL_NUMBER:
            token_t* tok = &expr->value.literal.lexeme;
            char* end = (char*) tok->start + tok->length;

            val.type = VAL_NUMBER;
            val.val.number = strtod(tok->start, &end);
            break;
        case LITERAL_BOOL:
            val.type = VAL_BOOL;
            val.val.boolean = expr->value.literal.lexeme.type == TOKEN_TRUE;
        default:
            rterr("TODO: literals");
        }

        return val;
    }

    if (expr->type == EXPR_UNARY) {
        value_t inner = eval(expr->value.unary.right);

        if (expr->value.unary.op.type == TOKEN_MINUS) {
            if (inner.type != VAL_NUMBER) rterr("Expected number after unary minus");

            inner.val.number = -inner.val.number;
        } else if (expr->value.unary.op.type == TOKEN_BANG) {
            if (inner.type != VAL_BOOL) rterr("Expected bool after unary bang");

            inner.val.boolean = !inner.val.boolean;
        } else assert(false);

        return inner;
    }

    rterr("TODO: other expr types");

    /*switch (expr->binary.op) {
    // TODO
    default:
        assert(false);
    }*/
}

int main(int argc, char** argv) {
    log_set_level(LOG_LEVEL_DEBUG);

    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE* fp = fopen(argv[1], "r");
    char* code = read_full(fp);
    fclose(fp);

    tokens_t tokens = { 0 };
    if (!tokenize(&tokens, code)) return 1;

    stmt_list_t ast = { 0 };
    parse_program(&ast, &tokens);
    free(code);

    value_t val = eval(ast.items[0]->as.assert_stmt.expression);
    printf("%lf", val.val.number);

    return 0;
}
