#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "c_utils/utils.h"

static char* read_full(FILE* fp) {
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* output = (char*)malloc(size + 1);
    fread(output, 1, size, fp);
    output[size] = 0;

    return output;
}

int main(int argc, char** argv) {
    log_set_level(LOG_LEVEL_DEBUG);

    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE* fp = fopen(argv[1], "r");
    const char* code = read_full(fp);
    fclose(fp);

    tokens_t tokens = {0};
    if (!tokenize(&tokens, code))
        return 1;

    log_tokens(&tokens);

    stmt_list_t ast = {0};
    parse_program(&ast, &tokens);

    for (int i = 0; i < ast.count; i++) {
        interpret(code, ast.items[i]);
    }

    return 0;
}
