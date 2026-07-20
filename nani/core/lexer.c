#include "lexer.h"
#include "c_utils/utils.h"

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

static void add_token(tokens_t* tokens, const char* start, int len, token_kind_t type, int line) {
    if (!tokens->ptr) {
        tokens->length = 0;
        tokens->capacity = 16;
        tokens->ptr = (token_t*)malloc(tokens->capacity * sizeof(token_t));
    }

    if (tokens->length == tokens->capacity) {
        tokens->capacity *= 2;
        tokens->ptr = (token_t*)realloc(tokens->ptr, tokens->capacity * sizeof(token_t));
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
// clang-format off
#define P(pat, kin) {.pattern = (pat), .type = (kin), .len = sizeof(pat) - 1}
static pattern_t patterns[] = {
    P("return", TOKEN_RETURN),
    P("assert", TOKEN_ASSERT),
    P("print", TOKEN_PRINT),
    P("while", TOKEN_WHILE),
    P("false", TOKEN_FALSE),
    P("true", TOKEN_TRUE),
    P("else", TOKEN_ELSE),
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
    P("[", TOKEN_LEFT_BRACKET),
    P("]", TOKEN_RIGHT_BRACKET),
    P(",", TOKEN_COMMA),
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
// clang-format on

const char* token_kind_name(token_kind_t type) {
    switch (type) {
    case TOKEN_LEFT_PAREN:
        return "LEFT_PAREN";
    case TOKEN_RIGHT_PAREN:
        return "RIGHT_PAREN";
    case TOKEN_LEFT_BRACE:
        return "LEFT_BRACE";
    case TOKEN_RIGHT_BRACE:
        return "RIGHT_BRACE";
    case TOKEN_LEFT_BRACKET:
        return "TOKEN_LEFT_BRACKET";
    case TOKEN_RIGHT_BRACKET:
        return "TOKEN_RIGHT_BRACKET";
    case TOKEN_COMMA:
        return "COMMA";
    case TOKEN_SEMICOLON:
        return "SEMICOLON";
    case TOKEN_MINUS:
        return "MINUS";
    case TOKEN_PLUS:
        return "PLUS";
    case TOKEN_SLASH:
        return "SLASH";
    case TOKEN_STAR:
        return "STAR";
    case TOKEN_BANG:
        return "BANG";
    case TOKEN_BANG_EQ:
        return "BANG_EQ";
    case TOKEN_EQ:
        return "EQ";
    case TOKEN_EQ_EQ:
        return "EQ_EQ";
    case TOKEN_GREATER:
        return "GREATER";
    case TOKEN_GREATER_EQ:
        return "GREATER_EQ";
    case TOKEN_LESS:
        return "LESS";
    case TOKEN_LESS_EQ:
        return "LESS_EQ";
    case TOKEN_IDENTIFIER:
        return "IDENTIFIER";
    case TOKEN_STRING:
        return "STRING";
    case TOKEN_NUMBER:
        return "NUMBER";
    case TOKEN_AND:
        return "AND";
    case TOKEN_ASSERT:
        return "ASSERT";
    case TOKEN_ELSE:
        return "ELSE";
    case TOKEN_FALSE:
        return "FALSE";
    case TOKEN_FN:
        return "FN";
    case TOKEN_FOR:
        return "FOR";
    case TOKEN_IF:
        return "IF";
    case TOKEN_LET:
        return "LET";
    case TOKEN_NIL:
        return "NIL";
    case TOKEN_OR:
        return "OR";
    case TOKEN_PRINT:
        return "PRINT";
    case TOKEN_RETURN:
        return "RETURN";
    case TOKEN_TRUE:
        return "TRUE";
    case TOKEN_WHILE:
        return "WHILE";
    case TOKEN_ERROR:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}

void log_token(const char* prefix, const token_t* token) {
    log_debug("%s: type=%s, lexeme=\"%.*s\", line=%d", prefix, token_kind_name(token->type), token->length,
              token->start, token->line);
}

void log_tokens(const tokens_t* tokens) {
    log_debug("tokens: count=%d", tokens->length);
    for (int i = 0; i < tokens->length; i++) {
        log_token("token", &tokens->ptr[i]);
    }
}

bool tokenize(tokens_t* tokens, const char* code) {
    int i = 0;
    int line = 1;

    while (code[i]) {
        bool parsed = false;
        for (int pi = 0; pi < sizeof(patterns) / sizeof(pattern_t); pi++) {
            pattern_t* pat = &patterns[pi];

            if (!memcmp(pat->pattern, &code[i], pat->len)) {
                add_token(tokens, &code[i], pat->len, pat->type, line);
                i += pat->len;
                parsed = true;
                break;
            }
        }

        // special cases
        char c = code[i];
        if (is_digit(c)) { // number
            int len = 0;
            bool seen_point = false;
            while (is_digit(code[i + len]) || (!seen_point && code[i + len] == '.')) {
                if (code[i + len] == '.')
                    seen_point = true;
                len++;
            }

            add_token(tokens, &code[i], len, TOKEN_NUMBER, line);
            i += len;

            continue;
        } else if (c == '"') { // string
            // TODO: escape sequences

            int len = 1; // left quote
            while (code[i + len] != '"')
                len++;
            len++; // right quote

            add_token(tokens, &code[i], len, TOKEN_STRING, line);
            i += len;

            continue;

        } else if (is_alpha(c) || c == '_') { // identifier
            int len = 0;
            while (is_alnum(code[i + len]) || code[i + len] == '_')
                len++;

            add_token(tokens, &code[i], len, TOKEN_IDENTIFIER, line);
            i += len;

            continue;
        } else if (c == '/' && code[i + 1] == '/') { // line comment
            int len = 1;
            while (code[i + len] != '\n')
                len++;

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

        if (parsed)
            continue;

        log_error("Invalid char on line %d: '%c'\n", line, c);
        return false;
    }

    return true;
}
