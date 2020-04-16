#include <assert.h>
#include <string.h>
#include "list.h"

#define INITIAL_CAP 2
#define CAP_MULTIPLIER 2

array_list * new_array_list(u64 data_size) {
	array_list *list = (array_list *) calloc(1, sizeof(array_list));
	list->data_size = data_size;
	return list;
}

void array_list_append(array_list *l, void *data, u64 data_size) {

	assert(l);
	assert(data);
	assert(data_size);

	int c = INITIAL_CAP;

	if(l->data == NULL) {
		l->data = malloc(c * data_size);	
	}
	else {
		c = l->capacity;

		if(l->size + 1 > c) {
			l->data = realloc(l->data, c*CAP_MULTIPLIER*data_size);
		}
	}

	memcpy( ((char*)l->data) + (data_size * l->size), data, data_size);
	
	l->size++;
	l->capacity = c;
}

void array_list_set_capacity(array_list *l, u64 capacity) {

	l->data = realloc(l->data, capacity*l->data_size);
	l->capacity = capacity;

	if(capacity < l->size) {
		l->size = capacity;
	}

}

void array_list_map(array_list *l, map_function *f) {

	for(u64 i = 0; i < l->size; i++) {
		f( (char*)(l->data) + (i*l->data_size) );
	}

}

