#include <pthread.h>
#include "../filters/filter.h"
#include <stdatomic.h>

typedef struct
{
    int w, h;
    unsigned char *input;
    unsigned char *output;
    filter filter;
    atomic_int *shared_counter;
} thread_args;

typedef enum
{
    MODE_ROW,
    MODE_COLUMN,
    MODE_PIXEL
} ConvolutionMode;

void pthread_convolution(unsigned char *pixel_array, int w, int h, filter filter, int num_threads, ConvolutionMode mode);
