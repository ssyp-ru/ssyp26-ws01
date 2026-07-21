#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"
#include "c_utils/utils.h"

static token_t* get_tok(tokens_t* tokens, int* pos) {
    if (*pos >= tokens->length) {
        // hack, but simpler than checking for NULL everywhere
        static token_t eof = {
            .type = TOKEN_ERROR,
            .start = "[ERROR]",
            .length = 7,
            .line = -1,
        };
        return &eof;
    };

    return &tokens->ptr[*pos];
}

static void err(tokens_t* tokens, int* pos, const char* text) {
    // err is called after pos has been incremented
    // so we have to decrement to get the actual
    // token where the error occured
    (*pos)--;

    token_t* tok = get_tok(tokens, pos);

    char* tokstr = (char*)malloc((tok->length + 1) * sizeof(char));
    memcpy(tokstr, tok->start, tok->length);
    tokstr[tok->length] = 0;

    log_error("Parser error at line %d (token \"%s\"): %s\n", tok->line, tokstr, text);

    free(tokstr);
    exit(1);
}

static stmt_t* add_stmt(stmt_list_t* ast) {
    if (!ast->items) {
        ast->count = 0;
        ast->capacity = 16;
        ast->items = (stmt_t**)malloc(ast->capacity * sizeof(stmt_t*));
    }

    if (ast->count == ast->capacity) {
        ast->capacity *= 2;
        ast->items = (stmt_t**)realloc(ast->items, ast->capacity * sizeof(stmt_t*));
    }

    stmt_t* stmt = (stmt_t*)malloc(sizeof(stmt_t));
    memset(stmt, 0, sizeof(stmt_t));
    ast->items[ast->count++] = stmt;
    return stmt;
}

static expr_t* make_logical_expr(expr_t* left, token_t op, expr_t* right) {
    log_debug("create logical expr");
    log_token(" exrp token:", &op);
    expr_t* out = (expr_t*)malloc(sizeof(expr_t));
    out->type = EXPR_LOGICAL;
    out->line = op.line;
    out->value.logical.op = op;
    out->value.logical.left = left;
    out->value.logical.right = right;
    return out;
}

static expr_t* make_binary_expr(expr_t* left, token_t op, expr_t* right) {
    log_debug("create binary expr");
    log_token("  bin exrp token:", &op);
    expr_t* out = (expr_t*)malloc(sizeof(expr_t));
    out->type = EXPR_BINARY;
    out->line = op.line;
    out->value.binary.op = op;
    out->value.binary.left = left;
    out->value.binary.right = right;
    return out;
}

expr_t* make_assign_expr(token_t op, expr_t* variable, expr_t* value) {
    expr_t* out = (expr_t*)malloc(sizeof(expr_t));
    out->type = EXPR_ASSIGN;
    out->line = op.line;
    out->value.assign.name = variable->value.variable.name;
    out->value.assign.value = value;
    return out;
}

expr_t* make_key_set(token_t* tok, expr_t* left, expr_t* value) {
    expr_t* new = (expr_t*)malloc(sizeof(expr_t));
    new->type = EXPR_KEY_SET;
    new->value.key_set.object = left->value.key.object;
    new->value.key_set.key = left->value.key.key;
    new->value.key_set.value = value;
    return (new);
}

static expr_t* make_unary_expr(token_t op, expr_t* right) {
    expr_t* out = (expr_t*)malloc(sizeof(expr_t));
    out->type = EXPR_UNARY;
    out->line = op.line;
    out->value.unary.op = op;
    out->value.unary.right = right;
    return out;
}

static expr_t* make_call_expr(expr_t* callee, token_t paren, expr_list_t arguments) {
    expr_t* out = (expr_t*)malloc(sizeof(expr_t));
    out->type = EXPR_CALL;
    out->line = paren.line;
    out->value.call.callee = callee;
    out->value.call.paren = paren;
    out->value.call.arguments = arguments;
    return out;
}

static expr_t* make_literal_expr(token_t lexeme, literal_type_t type) {
    expr_t* out = (expr_t*)malloc(sizeof(expr_t));
    out->line = lexeme.line;
    out->type = EXPR_LITERAL;
    out->value.literal.lexeme = lexeme;
    out->value.literal.type = type;
    return out;
}

static expr_t* make_variable_expr(token_t name) {
    expr_t* out = (expr_t*)malloc(sizeof(expr_t));
    out->line = name.line;
    out->type = EXPR_VARIABLE;
    out->value.variable.name = name;
    return out;
}

void add_expr(expr_list_t* expressions, expr_t* expression) {
    if (expressions->count == expressions->capacity) {
        expressions->capacity = expressions->capacity ? expressions->capacity * 2 : 8;
        expressions->items = (expr_t**)realloc(expressions->items, expressions->capacity * sizeof(expr_t*));
    }

    expressions->items[expressions->count++] = expression;
}

void add_parameter(function_decl_t* declaration, token_t parameter) {
    if (declaration->parameter_count == declaration->parameter_capacity) {
        declaration->parameter_capacity = declaration->parameter_capacity ? declaration->parameter_capacity * 2 : 8;
        declaration->parameters =
            (token_t*)realloc(declaration->parameters, declaration->parameter_capacity * sizeof(token_t));
    }

    declaration->parameters[declaration->parameter_count++] = parameter;
}

static void free_expr(expr_t* expr);

static void free_expr_list(expr_list_t* list) {
    for (int i = 0; i < list->count; i++) {
        free_expr(list->items[i]);
    }

    free(list->items);
}

void free_expr(expr_t* expr) {
    switch (expr->type) {
    case EXPR_GROUPING:
        free_expr(expr->value.grouping.expression);
        break;
    case EXPR_UNARY:
        free_expr(expr->value.unary.right);
        break;
    case EXPR_BINARY:
        free_expr(expr->value.binary.left);
        free_expr(expr->value.binary.right);
        break;
    case EXPR_LOGICAL:
        free_expr(expr->value.logical.left);
        free_expr(expr->value.logical.right);
        break;
    case EXPR_ASSIGN:
        free_expr(expr->value.assign.value);
        break;
    case EXPR_CALL:
        free_expr(expr->value.call.callee);
        free_expr_list(&expr->value.call.arguments);
        break;
    case EXPR_KEY:
        free_expr(expr->value.key.object);
        free_expr(expr->value.key.key);
        break;
    case EXPR_KEY_SET:
        free_expr(expr->value.key_set.object);
        free_expr(expr->value.key_set.key);
        free_expr(expr->value.key_set.value);
        break;
    default:
        break;
    }

    free(expr);
}

static void free_stmt(stmt_t* stmt) {
    switch (stmt->type) {
    case STMT_ASSERT:
        free_expr(stmt->as.assert_stmt.expression);
        break;
    case STMT_BLOCK:
        free_ast(&stmt->as.block.declarations);
        break;
    case STMT_EXPRESSION:
        free_expr(stmt->as.expression.expression);
        break;
    case STMT_FOR:
        free_stmt(stmt->as.for_stmt.initializer);
        free_expr(stmt->as.for_stmt.condition);
        free_expr(stmt->as.for_stmt.increment);
        free_stmt(stmt->as.for_stmt.body);
        break;
    case STMT_FUNCTION:
        free(stmt->as.function.declaration.parameters);
        free_ast(&stmt->as.function.declaration.body);
        break;
    case STMT_IF:
        free_expr(stmt->as.if_stmt.condition);
        free_stmt(stmt->as.if_stmt.then_branch);
        if (stmt->as.if_stmt.else_branch)
            free_stmt(stmt->as.if_stmt.else_branch);
        break;
    case STMT_LET:
        free_expr(stmt->as.let.initializer);
        break;
    case STMT_PRINT:
        free_expr(stmt->as.print.expression);
        break;
    case STMT_RETURN:
        free_expr(stmt->as.return_stmt.value);
        break;
    case STMT_WHILE:
        free_expr(stmt->as.while_stmt.condition);
        free_stmt(stmt->as.while_stmt.body);
        break;
    }

    free(stmt);
}

void free_ast(stmt_list_t* ast) {
    for (int i = 0; i < ast->count; i++) {
        stmt_t* stmt = ast->items[i];
        free_stmt(stmt);
    }

    free(ast->items);
}

static void parse_stmt(stmt_t* stmt, tokens_t* tokens, int* pos);
static void parse_function_declaration(stmt_t* stmt, tokens_t* tokens, int* pos);
static void parse_if_stmt(stmt_t* stmt, tokens_t* tokens, int* pos);
static void parse_while_stmt(stmt_t* stmt, tokens_t* tokens, int* pos);
expr_t* parse_assignment(tokens_t* tokens, int* pos);
static expr_t* parse_or(tokens_t* tokens, int* pos);
static expr_t* parse_and(tokens_t* tokens, int* pos);
static expr_t* parse_equality(tokens_t* tokens, int* pos);
static expr_t* parse_comparison(tokens_t* tokens, int* pos);
static expr_t* parse_term(tokens_t* tokens, int* pos);
static expr_t* parse_factor(tokens_t* tokens, int* pos);
static expr_t* parse_unary(tokens_t* tokens, int* pos);
static expr_t* parse_call(tokens_t* tokens, int* pos);
static expr_t* parse_primary(tokens_t* tokens, int* pos);

void parse_program(stmt_list_t* ast, tokens_t* tokens) {
    int pos = 0;
    log_debug("parse_program: pos=%d", pos);
    log_token("  cur token", get_tok(tokens, &pos));

    while (pos < tokens->length) {
        stmt_t* stmt = add_stmt(ast);
        parse_stmt(stmt, tokens, &pos);
    }
}

void parse_stmt(stmt_t* stmt, tokens_t* tokens, int* pos) {
    log_debug("parse_stmt: pos=%d", *pos);
    log_token("  cur token", get_tok(tokens, pos));
    token_t* tok = get_tok(tokens, pos);
    (*pos)++;

    stmt->line = tok->line;

    if (tok->type == TOKEN_FN) {
        parse_function_declaration(stmt, tokens, pos);
        return;
    } else if (tok->type == TOKEN_LET) {
        stmt->type = STMT_LET;

        tok = get_tok(tokens, pos);
        (*pos)++;
        if (tok->type != TOKEN_IDENTIFIER)
            err(tokens, pos, "Expected variable name");
        stmt->as.let.name = *tok;
        stmt->as.let.initializer = NULL;

        tok = get_tok(tokens, pos);
        if (tok->type == TOKEN_EQ) {
            (*pos)++;
            stmt->as.let.initializer = parse_assignment(tokens, pos);
        }
    } else if (tok->type == TOKEN_ASSERT) {
        stmt->type = STMT_ASSERT;
        stmt->as.assert_stmt.expression = parse_assignment(tokens, pos);
    } else if (tok->type == TOKEN_PRINT) {
        stmt->type = STMT_PRINT;
        stmt->as.print.expression = parse_assignment(tokens, pos);
    } else if (tok->type == TOKEN_RETURN) {
        stmt->type = STMT_RETURN;
        stmt->as.return_stmt.keyword = *tok;
        if (get_tok(tokens, pos)->type == TOKEN_SEMICOLON)
            stmt->as.return_stmt.value = NULL;
        else
            stmt->as.return_stmt.value = parse_assignment(tokens, pos);
    } else if (tok->type == TOKEN_IF) {
        parse_if_stmt(stmt, tokens, pos);
        return;
    } else if (tok->type == TOKEN_WHILE) {
        parse_while_stmt(stmt, tokens, pos);
        return;
    } else if (tok->type == TOKEN_LEFT_BRACE) {
        stmt->type = STMT_BLOCK;
        stmt_list_t* decls = &stmt->as.block.declarations;

        while (get_tok(tokens, pos)->type != TOKEN_RIGHT_BRACE) {
            if (*pos >= tokens->length) {
                (*pos)++;
                err(tokens, pos, "Expected right brace at block end");
            }

            stmt_t* body_stmt = add_stmt(decls);
            parse_stmt(body_stmt, tokens, pos);
        }
        (*pos)++;

        return; // no semicolon
    } else {
        stmt->type = STMT_EXPRESSION;
        (*pos)--;
        stmt->as.expression.expression = parse_assignment(tokens, pos);
    }

    tok = get_tok(tokens, pos);
    (*pos)++;
    if (tok->type != TOKEN_SEMICOLON)
        err(tokens, pos, "Expected semicolon");
}

static void parse_function_declaration(stmt_t* stmt, tokens_t* tokens, int* pos) {
    stmt->type = STMT_FUNCTION;
    function_decl_t* declaration = &stmt->as.function.declaration;
    declaration->parameters = NULL;
    declaration->parameter_count = 0;
    declaration->parameter_capacity = 0;
    declaration->body.items = NULL;
    declaration->body.count = 0;
    declaration->body.capacity = 0;

    token_t* tok = get_tok(tokens, pos);
    (*pos)++;
    if (tok->type != TOKEN_IDENTIFIER)
        err(tokens, pos, "Expected function name");
    declaration->name = *tok;

    tok = get_tok(tokens, pos);
    (*pos)++;
    if (tok->type != TOKEN_LEFT_PAREN)
        err(tokens, pos, "Expected left parentheses");

    if (get_tok(tokens, pos)->type != TOKEN_RIGHT_PAREN) {
        while (1) {
            tok = get_tok(tokens, pos);
            (*pos)++;
            if (tok->type != TOKEN_IDENTIFIER)
                err(tokens, pos, "Expected parameter name");
            add_parameter(declaration, *tok);

            tok = get_tok(tokens, pos);
            (*pos)++;
            if (tok->type == TOKEN_RIGHT_PAREN)
                break;
            if (tok->type != TOKEN_COMMA)
                err(tokens, pos, "Expected comma or right parentheses");
        }
    } else {
        (*pos)++;
    }

    tok = get_tok(tokens, pos);
    (*pos)++;
    if (tok->type != TOKEN_LEFT_BRACE)
        err(tokens, pos, "Expected left brace");

    while (get_tok(tokens, pos)->type != TOKEN_RIGHT_BRACE) {
        if (*pos >= tokens->length) {
            (*pos)++;
            err(tokens, pos, "Expected right brace");
        }

        stmt_t* body_stmt = add_stmt(&declaration->body);
        parse_stmt(body_stmt, tokens, pos);
    }
    (*pos)++;
}

void parse_if_stmt(stmt_t* stmt, tokens_t* tokens, int* pos) {
    stmt->type = STMT_IF;
    stmt->as.if_stmt.else_branch = NULL;

    token_t* tok = get_tok(tokens, pos);
    (*pos)++;
    if (tok->type != TOKEN_LEFT_PAREN)
        err(tokens, pos, "Expected left parentheses");

    expr_t* cond = parse_or(tokens, pos);
    stmt->as.if_stmt.condition = cond;

    tok = get_tok(tokens, pos);
    (*pos)++;
    if (tok->type != TOKEN_RIGHT_PAREN)
        err(tokens, pos, "Expected right parentheses");

    stmt_t* then = (stmt_t*)malloc(sizeof(stmt_t));
    memset(then, 0, sizeof(stmt_t));
    stmt->as.if_stmt.then_branch = then;
    parse_stmt(then, tokens, pos);

    if (get_tok(tokens, pos)->type != TOKEN_ELSE)
        return;
    (*pos)++;

    stmt_t* elsee = (stmt_t*)malloc(sizeof(stmt_t));
    memset(elsee, 0, sizeof(stmt_t));
    stmt->as.if_stmt.else_branch = elsee;
    parse_stmt(elsee, tokens, pos);
}

void parse_while_stmt(stmt_t* stmt, tokens_t* tokens, int* pos) {
    stmt->type = STMT_WHILE;

    token_t* tok = get_tok(tokens, pos);
    (*pos)++;
    if (tok->type != TOKEN_LEFT_PAREN)
        err(tokens, pos, "Expected left parentheses");

    expr_t* cond = parse_or(tokens, pos);
    stmt->as.while_stmt.condition = cond;

    tok = get_tok(tokens, pos);
    (*pos)++;
    if (tok->type != TOKEN_RIGHT_PAREN)
        err(tokens, pos, "Expected right parentheses");

    stmt_t* body = (stmt_t*)malloc(sizeof(stmt_t));
    memset(body, 0, sizeof(stmt_t));
    stmt->as.while_stmt.body = body;
    parse_stmt(body, tokens, pos);
}

expr_t* parse_assignment(tokens_t* tokens, int* pos) {
    expr_t* left = parse_or(tokens, pos);

    token_t* tok = get_tok(tokens, pos);
    if (tok->type != TOKEN_EQ)
        return left;
    (*pos)++;

    expr_t* value = parse_assignment(tokens, pos);

    /*
     TODO(OBJ): Тут нужно посмотреть, если у нас EXPR_KEY то мы меняем на EXPR_KEY_SET
     (также как с make_assign_expr)
     */
    if (left->type == EXPR_KEY) {
        expr_t* out = make_key_set(tok, left, value);
        free(left);
        return out;
    }
    if (left->type != EXPR_VARIABLE)
        err(tokens, pos, "Invalid assignment target");

    expr_t* out = make_assign_expr(*tok, left, value);
    free(left);
    return out;
}

expr_t* parse_or(tokens_t* tokens, int* pos) {
    log_debug("parse_or: pos=%d", *pos);
    log_token("  cur token", get_tok(tokens, pos));
    expr_t* left = parse_and(tokens, pos);

    token_t* tok = get_tok(tokens, pos);
    if (tok->type != TOKEN_OR)
        return left;
    (*pos)++;

    expr_t* right = parse_and(tokens, pos);
    return make_logical_expr(left, *tok, right);
}

expr_t* parse_and(tokens_t* tokens, int* pos) {
    log_debug("parse_and: pos=%d", *pos);
    log_token("  cur token", get_tok(tokens, pos));
    expr_t* left = parse_equality(tokens, pos);

    token_t* tok = get_tok(tokens, pos);
    if (tok->type != TOKEN_AND)
        return left;
    (*pos)++;

    expr_t* right = parse_equality(tokens, pos);
    return make_logical_expr(left, *tok, right);
}

expr_t* parse_equality(tokens_t* tokens, int* pos) {
    log_debug("parse_equality: pos=%d", *pos);
    log_token("  cur token", get_tok(tokens, pos));
    expr_t* left = parse_comparison(tokens, pos);

    token_t* tok = get_tok(tokens, pos);
    if (tok->type != TOKEN_EQ_EQ && tok->type != TOKEN_BANG_EQ)
        return left;
    (*pos)++;

    expr_t* right = parse_comparison(tokens, pos);
    return make_logical_expr(left, *tok, right);
}

expr_t* parse_comparison(tokens_t* tokens, int* pos) {
    log_debug("parse_comparison: pos=%d", *pos);
    log_token("  cur token", get_tok(tokens, pos));
    expr_t* left = parse_term(tokens, pos);

    token_t* tok = get_tok(tokens, pos);
    switch (tok->type) {
    case TOKEN_LESS:
    case TOKEN_LESS_EQ:
    case TOKEN_GREATER:
    case TOKEN_GREATER_EQ:
        break;
    default:
        return left;
    }
    (*pos)++;

    expr_t* right = parse_term(tokens, pos);
    return make_logical_expr(left, *tok, right);
}

expr_t* parse_term(tokens_t* tokens, int* pos) {
    log_debug("parse_term: pos=%d", *pos);
    log_token("  cur token", get_tok(tokens, pos));
    expr_t* left = parse_factor(tokens, pos);

    token_t* tok = get_tok(tokens, pos);
    if (tok->type != TOKEN_PLUS && tok->type != TOKEN_MINUS)
        return left;
    (*pos)++;

    expr_t* right = parse_factor(tokens, pos);
    return make_binary_expr(left, *tok, right);
}

expr_t* parse_factor(tokens_t* tokens, int* pos) {
    log_debug("parse_factor: pos=%d", *pos);
    log_token("  cur token", get_tok(tokens, pos));
    expr_t* left = parse_unary(tokens, pos);

    token_t* tok = get_tok(tokens, pos);
    if (tok->type != TOKEN_STAR && tok->type != TOKEN_SLASH)
        return left;
    (*pos)++;

    expr_t* right = parse_unary(tokens, pos);
    return make_binary_expr(left, *tok, right);
}

expr_t* parse_unary(tokens_t* tokens, int* pos) {
    log_debug("parse_unary: pos=%d", *pos);
    log_token("  cur token", get_tok(tokens, pos));
    token_t* tok = get_tok(tokens, pos);
    if (tok->type != TOKEN_BANG && tok->type != TOKEN_MINUS)
        return parse_call(tokens, pos);
    (*pos)++;
    return make_unary_expr(*tok, parse_unary(tokens, pos));
}

static expr_t* parse_call(tokens_t* tokens, int* pos) {
    expr_t* callee = parse_primary(tokens, pos);

    /*
     TODO(OBJ): добавить проверку на TOKEN_LEFT_BRACKET/TOKEN_RIGHT_BRACKET, внутри
     `[expression]` нужно ещё раз спарсить (там может быть сложное выражение).
     Должно получится expr с типом EXPR_KEY
     */

    if (get_tok(tokens, pos)->type == TOKEN_LEFT_BRACKET) { // ?
        (*pos)++;
        expr_t* key = parse_assignment(tokens, pos);
        expr_t* expr = (expr_t*)malloc(sizeof(expr_t));
        expr->type = EXPR_KEY;
        expr->value.key.key = key;
        expr->value.key.object = callee;
        if (get_tok(tokens, pos)->type == TOKEN_RIGHT_BRACKET) {
            (*pos)++;
            return expr;
        }
        // error?
    }

    while (get_tok(tokens, pos)->type == TOKEN_LEFT_PAREN) {
        token_t paren = *get_tok(tokens, pos);
        (*pos)++;

        expr_list_t arguments = {0};
        if (get_tok(tokens, pos)->type != TOKEN_RIGHT_PAREN) {
            while (1) {
                add_expr(&arguments, parse_assignment(tokens, pos));

                token_t* separator = get_tok(tokens, pos);
                (*pos)++;
                if (separator->type == TOKEN_RIGHT_PAREN)
                    break;
                if (separator->type != TOKEN_COMMA)
                    err(tokens, pos, "Expected comma or right parentheses");
            }
        } else {
            (*pos)++;
        }

        callee = make_call_expr(callee, paren, arguments);
    }

    return callee;
}

expr_t* parse_primary(tokens_t* tokens, int* pos) {
    log_debug("parse_primary: pos=%d", *pos);
    log_token("  cur token", get_tok(tokens, pos));
    token_t* tok = get_tok(tokens, pos);
    (*pos)++;

    if (tok->type == TOKEN_LEFT_PAREN) {
        expr_t* expr = parse_assignment(tokens, pos);

        token_t* tok2 = get_tok(tokens, pos);
        (*pos)++;
        if (tok2->type != TOKEN_RIGHT_PAREN)
            err(tokens, pos, "Expected right parentheses");

        return expr;
    }

    if (tok->type == TOKEN_NUMBER)
        return make_literal_expr(*tok, LITERAL_NUMBER);
    if (tok->type == TOKEN_STRING)
        return make_literal_expr(*tok, LITERAL_STRING);
    if (tok->type == TOKEN_TRUE || tok->type == TOKEN_FALSE)
        return make_literal_expr(*tok, LITERAL_BOOL);
    if (tok->type == TOKEN_IDENTIFIER)
        return make_variable_expr(*tok);

    err(tokens, pos, "Expected primary (expr in parentheses, number, true, false or identifier)");
    return NULL;
}
