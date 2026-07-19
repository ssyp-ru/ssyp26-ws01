#include "array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_array(array_t *a){    //?
	(*a).data = (token_t *)malloc(4*sizeof(token_t));
	(*a).capacity = 4;
	(*a).size = 0;
}

void destroy_array(array_t *a){     //OK
	free((*a).data);
}

void push_array(array_t *a, token_t value){     //?     
	if((*a).size >= (*a).capacity){
		(*a).capacity *= 2;
		token_t *b;
		b = (token_t *)malloc((*a).capacity*sizeof(token_t));
		for(int i = 0; i < (*a).size; i++){
			b[i] = (*a).data[i];
		}
//		(*a).data = (int *)malloc((*a).capacity*sizeof(int));
//		for(int i = 0; i < (*a).size; i++){
//			(*a).data[i] = (b).data[i];
//		}
		free((*a).data);
		a->data = b;
	}
	(*a).data[(*a).size] = value;
	(*a).size++;
}

//void print_array(array_t *a){
//	printf("array: ");
//	for(int i = 0; i < (*a).size; i++){
//		printf("%d ", (*a).data[i]);
//	}
//	printf("\n");
//}

token_t pop_array(array_t *a){      //OK
	(*a).size--;
	return (*a).data[(*a).size];
}

