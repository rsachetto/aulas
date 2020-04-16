#include <stdio.h>
#include "list.h"

int float_compare (const void *pa, const void *pb) {

	float a = *((float*)pa);
	float b = *((float*)pb);

	if(a < b) return -1;
	if(a > b) return 1;

	return 0;
}

void print_float(void *item) {
	float a = *((float*)item);
	printf("%lf  \n", a);
}


int main() {

	array_list *p;
	p = new_array_list(sizeof(float));
	array_list_set_capacity(p, 10);
	
	float a = 30;

	printf("%zu, %zu, %p\n", p->capacity, p->size, p->data);

	al_append(p, (void*)&a);
	a = 10;
	al_append(p, (void*)&a);
	a = 20;
	al_append(p, (void*)&a);

	float *data = as_float(p);

	qsort(p->data, p->size, p->data_size, float_compare);

	array_list_map(p, print_float);

	return 1;
}

