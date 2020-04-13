#include <stdlib.h>
#include "lista.h"

void * maybe_grow(void *list, u64 data_size_in_bytes) {

    void *p;
    u64 m = 2;

    if(list == NULL) {
        p = malloc(data_size_in_bytes * m + sizeof(struct header));
        if(!p) return NULL;
        __internal_len__(p) = 0;
    }
    else {

        u64 list_size = __internal_len__(p);
        u64 cap = __internal_cap__(p);
    

        if ((__internal_len__(p) + 1) > cap) {
            m = cap * 2;
            p = realloc(__original_address__(list), data_size_in_bytes * m + sizeof(struct header));
            if(!p) return NULL;
        }


    }
    
    __internal_cap__(p) = m;

    return p + sizeof(struct header);
}

