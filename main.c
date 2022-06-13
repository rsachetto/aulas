#include <stdio.h>
#include <stdlib.h>

#include "lista.h"
#include <math.h>
#include <pthread.h>

void print_array(float *v, int v_size) {

    for(int i = 0; i < v_size; i++)
        printf("%lf\n", v[i]);
}

int main() {
    
    float *v = NULL;

    append(v, 1.0f);
    append(v, 2.0f);
    append(v, 3.0f);
    append(v, 4.0f);

    print_array(v, arrlength(v));

    arrfree(v);
}
