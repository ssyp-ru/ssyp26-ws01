#include "array.h"
#include "token.h"
#include <stdio.h>
//#include <stdlib.h>
#include <string.h>

array_t tokenizer(char s[]){    //?
    int l = 0;
    int l2 = strlen(s);
//    enum token_type t1 = TOKEN_OTHER;
    enum token_type t2;
    array_t a;
    init_array(&a);
    for(int i = 0; i < l2; i += 0){
        char c = s[i];
        l = 1;
        if(c >= '0' && c <= '9'){
            while(i+l < l2 && s[i+l] >= '0' && s[i+l] <= '9'){
                l++;
            }
            t2 = TOKEN_NUMBER;
            //c = s[i];
        } 
        else if(c == ' '){
//            printf("WHITESPACE\n");
            t2 = TOKEN_W;
        }
        else if(c == '+'){
            t2 = TOKEN_PLUS;
        }
        else if(c == '-'){
            t2 = TOKEN_MINUS;
        }
        else if(c == '*'){
            t2 = TOKEN_MULTIPLY;
        }
        else if(c == '/'){
            t2 = TOKEN_DIVIDE;
        } 
        else if(c == '('){
            t2 = TOKEN_PARENTHESISL;
        }
        else if(c == ')'){
            t2 = TOKEN_PARENTHESISR;
//            printf("!!!\n");
        }
        else if(c == '<'){
            t2 = TOKEN_LESS;
        }
        else if(c == '>'){
            t2 = TOKEN_MORE;
        }
        else if(c == '='){
            t2 = TOKEN_EQUAL;
        }
        else{
            t2 = TOKEN_OTHER;
        }

        token_t t = {
            t2,
            i,
            i+l-1
        };
//            printf("T(n): %d; st: %d; fi: %d\n", TOKEN_NUMBER, i-l, i-1);
        push_array(&a, t);

        i += l;
    }
    return a;
}

void print_array(array_t *a, char *s){      //?
    int l = (*a).size;
    for(int i = 0; i < l; i++){
        token_t t = (*a).data[i];
        int lt = t.finish - t.start;
        switch(t.t){
            case TOKEN_NUMBER:
                printf("\x1b[32m");
                break;
            case TOKEN_PLUS:
                printf("\x1b[33m");
                break;
            case TOKEN_MINUS:
                printf("\x1b[33m");
                break;
            case TOKEN_MULTIPLY:
                printf("\x1b[31m");
                break;
            case TOKEN_DIVIDE:
                printf("\x1b[31m");
                break;
            case TOKEN_PARENTHESISL:
                printf("\x1b[34m");
                break;
            case TOKEN_PARENTHESISR:
                printf("\x1b[34m");
                break;
            case TOKEN_LESS:
                printf("\x1b[35m");
                break;
            case TOKEN_MORE:
                printf("\x1b[35m");
                break;
            case TOKEN_EQUAL:
                printf("\x1b[36m");
                break;
            case TOKEN_OTHER:
                printf("\x1b[37m");
                break;
        }

//        printf("N: %d\n", i);
        for(int j = 0; j < lt+1; j++){
            printf("%c", s[t.start+j]);
        }

        printf("\x1b[0m");
//        printf("%d \n", t.t);
    }
    printf("\n");
}
