#include <stdlib.h>

#include "lexer.h"

void init_slice(token_array_t *a) {
	int baseCap = 4;
	a->size = 0;
	a->capacity = baseCap;
	a->data = (token_t*)malloc(baseCap * sizeof(token_t));
}

void destroy_slice(token_array_t *a) {
	a->size = 0;
	a->capacity = 0;
	free(a->data);
}

void push_back(token_array_t *a, token_t value) {
	if (a->size > a->capacity - 1) {
		token_t *old_slice = a->data;
		a->data = (token_t*)malloc(a->capacity * sizeof(token_t) * 2);
		a->capacity *= 2;
		
		for(int i = 0; i < a->size; i++) {
			a->data[i] = old_slice[i];	
		}
		free(old_slice);
	}
	a->data[a->size] = value;
	a->size++;

}

token_t pop_back(token_array_t *a) {
	a->size--;
	return a->data[a->size];
}
