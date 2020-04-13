#include <stdio.h>
#include <stdlib.h>

#include "lista.h"
#include <math.h>
#include <pthread.h>

void print_array(float *v, int v_size) {

    for(int i = 0; i < v_size; i++)
        printf("%lf\n", v[i]);
}

typedef char* string;

struct celula {
    int a;
    float b;
};

void add_in_list(float **l, float f) {
    append(*l, f);
}

void *fp(void *a) {
    printf("UUHUH\n");
    return NULL;
}

int main() {
    
    pthread_t t;


    float *v = NULL;

    pthread_create(&t, NULL, fp, NULL);
    add_in_list(&v, powf(3.0, 2.0));

    pthread_join(t, NULL);

    print_array(v, arrlength(v));



}
