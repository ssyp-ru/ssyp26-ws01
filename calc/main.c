#include "array.h"
#include "token.h"
#include "parser.h"
#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

void test(){    //OK
//    char *s = "(3 +";
    char *s = "(3 +2)* 5-25= 0;0< 89;      {0   };5/600; ABCDE; 25 105    4";
    printf("%s\n", s);
    array_t a = tokenizer(s);
    print_array(&a, s);    
    destroy_array(&a);
}

void test2(){
    char *s = "3+2*5";
    printf("%s\n", s);
    array_t a = tokenizer(s);
    print_array(&a, s);
    token_list_t token_list = {
        a.data,
        a.size,
        a.capacity
    };
//    printf("OK_1\n");
    lexer_t list;
//    printf("OK_2\n");
    list.token_list = token_list;
//    printf("OK_3\n");
    list.str = s;
//    printf("OK_4\n");
    list.current = 0;
//    printf("OK_5\n");
    expr_t *ast = p_expr(&list);
//    printf("OK_6\n");
    print_ast(ast);
    printf("\n");
    printf("%d\n", interpretator(ast));
    destroy_array(&a);
//    printf("OK_7\n");
}

int main(){
    test();
    test2();
}
