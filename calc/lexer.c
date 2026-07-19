#include "array.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test(){    //OK
//    char *s = "(3 +";
    char *s = "(3 +2)* 5-25= 0;0< 89;      {0   };5/600; ABCDE; 25 105    4";
    printf("%s\n", s);
    array_t a = tokenizer(s);
    print_array(&a, s);    
    destroy_array(&a);
}

