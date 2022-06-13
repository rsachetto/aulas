#ifndef LISTA_H
#define LISTA_H

#include <stdint.h>

typedef uint64_t u64;
typedef uint32_t u32;

struct header {    
    u64 size;
    u64 capacity;
};

void * maybe_grow(void *list, u64 item_size);

//INTERNALS MACROS
#define __original_address__(__l) ( (char*)(__l) - sizeof(struct header) )
#define __len__(__l) ( ((struct header *)(__original_address__(__l)))->size )
#define __cap__(__l) ( ((struct header* )(__original_address__(__l)))->capacity )
#define __internal_len__(__l) ( ((struct header *)(__l))->size )
#define __internal_cap__(__l) ( ((struct header *)(__l))->capacity )


//API
#define append(__l, __item) ( (__l) = maybe_grow( (__l), sizeof(*(__l)) ), __l[__len__(__l)++] = (__item) )
#define arrlength(__l) ( (__l) ? __len__(__l) : 0 )
#define arrcapacity(__l) ( (__l) ? __cap__(__l) : 0 )
#define arrfree(__l) free(__original_address__(__l))

#endif
