#include <stdlib.h>
#include "lista.h"

void * maybe_grow(void *list, u64 data_size_in_bytes) {

    u64 m = 2;

    if(list == NULL) {
        list = malloc(data_size_in_bytes * m + sizeof(struct header));
        if(!list) return NULL;
        __internal_cap__(list) = m;
        __internal_len__(list) = 0;
    }
    else {

        u64 list_size = __len__(list);
        m = __cap__(list);

        if ((list_size + 1) > m) {
            m = m * 2;
            list = realloc(__original_address__(list), data_size_in_bytes * m + sizeof(struct header));
            if(!list) return NULL;
            __internal_cap__(list) = m;
        }
        else {
            return list;
        }
    }

    return (char*) list + sizeof(struct header);
}

