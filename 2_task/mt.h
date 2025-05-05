#include <pthread.h>
#include "../filters/filter.h"
#include <stdatomic.h>

typedef struct
{
    int w, h;
    unsigned char *input;
    unsigned char *output;
    struct filter filter;
    atomic_int *shared_counter; // <--- указатель на атомарный счётчик
} thread_args;

void pthread_convolution(unsigned char *pixel_array, int w, int h, struct filter filter, int num_threads);