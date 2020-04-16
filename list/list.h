#ifndef LIST_H
#define LIST_H

#include <stdint.h>
#include <stdlib.h>

typedef uint64_t u64;
typedef uint32_t u32;

#define al_append(l, data) array_list_append((l), data, (l)->data_size)
#define as_float(l) ((float*)l->data)

typedef void map_function(void *);

typedef struct {
	u64 size;
    u64 capacity;
	u64 data_size;
	void *data;
} array_list;

array_list * new_array_list(u64 data_size);
void array_list_append(array_list *l, void *data, u64 data_size);
void array_list_set_capacity(array_list *l, u64 capacity);
void array_list_map(array_list *l, map_function *f);
#endif
