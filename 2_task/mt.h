#include <pthread.h>
#include "../filters/filter.h"
#include <stdatomic.h>

#define THREAD_COUNT 4
#define BLOCK_SIZE 8

typedef struct
{
    int w, h;
    unsigned char *input;
    unsigned char *output;
    filter filter;
    atomic_int *shared_counter;
    int block_size;
} thread_args;

typedef enum
{
    MODE_ROW,
    MODE_COLUMN,
    MODE_PIXEL,
    MODE_BLOCK
} ConvolutionMode;

void mt_convolution(unsigned char *pixel_array, int w, int h, filter filter, int num_threads, ConvolutionMode mode, int block_size);
