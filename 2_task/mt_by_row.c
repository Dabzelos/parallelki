#include "../2_task/mt.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

void *thread_convolve_by_row(void *arg)
{
    thread_args *args = (thread_args *)arg;
    int w = args->w, h = args->h;
    struct filter filter = args->filter;

    // TODO вынести это в какой то общий .h, тк каждый раз пользуюсь пикселем
    struct pixel
    {
        unsigned char r, g, b;
    };

    struct pixel *input = (struct pixel *)args->input;
    struct pixel *output = (struct pixel *)args->output;

    int y;
    while ((y = atomic_fetch_add(args->shared_counter, 1)) < h)
    {
        for (int x = 0; x < w; x++)
        {
            double red = 0, green = 0, blue = 0;

            for (int fy = 0; fy < filter.size; fy++)
            {
                for (int fx = 0; fx < filter.size; fx++)
                {
                    int ix = (x - filter.size / 2 + fx + w) % w;
                    int iy = (y - filter.size / 2 + fy + h) % h;

                    struct pixel p = input[iy * w + ix];
                    red += p.r * filter.matrix[fy][fx];
                    green += p.g * filter.matrix[fy][fx];
                    blue += p.b * filter.matrix[fy][fx];
                }
            }

            struct pixel *dest = &output[y * w + x];
            dest->r = fmin(fmax((int)(filter.doubleCoeff * red + filter.bias), 0), 255);
            dest->g = fmin(fmax((int)(filter.doubleCoeff * green + filter.bias), 0), 255);
            dest->b = fmin(fmax((int)(filter.doubleCoeff * blue + filter.bias), 0), 255);
        }
    }

    return NULL;
}

void *thread_convolve_by_collumn(void *arg)
{
    thread_args *args = (thread_args *)arg;
    int w = args->w, h = args->h;
    struct filter filter = args->filter;

    struct pixel
    {
        unsigned char r, g, b;
    };

    struct pixel *input = (struct pixel *)args->input;
    struct pixel *output = (struct pixel *)args->output;

    int x;
    while ((x = atomic_fetch_add(args->shared_counter, 1)) < w)
    {
        for (int y = 0; y < h; y++)
        {
            double red = 0, green = 0, blue = 0;

            for (int fy = 0; fy < filter.size; fy++)
            {
                for (int fx = 0; fx < filter.size; fx++)
                {
                    int ix = (x - filter.size / 2 + fx + w) % w;
                    int iy = (y - filter.size / 2 + fy + h) % w;

                    struct pixel p = input[iy * w + ix];
                    red += p.r * filter.matrix[fy][fx];
                    green += p.g * filter.matrix[fy][fx];
                    blue += p.b * filter.matrix[fy][fx];
                }
            }

            struct pixel *dest = &output[y * w + x];
            dest->r = fmin(fmax((int)(filter.doubleCoeff * red + filter.bias), 0), 255);
            dest->g = fmin(fmax((int)(filter.doubleCoeff * green + filter.bias), 0), 255);
            dest->b = fmin(fmax((int)(filter.doubleCoeff * blue + filter.bias), 0), 255);
        }
    }

    return NULL;
}

void *thread_convolve_by_pixel(void *arg)
{
    thread_args *args = (thread_args *)arg;
    int w = args->w, h = args->h;
    struct filter filter = args->filter;

    struct pixel
    {
        unsigned char r, g, b;
    };

    struct pixel *input = (struct pixel *)args->input;
    struct pixel *output = (struct pixel *)args->output;

    int index;
    while ((index = atomic_fetch_add(args->shared_counter, 1)) < w * h)
    {
        int x = index % w;
        int y = index / w;

        double red = 0, green = 0, blue = 0;

        for (int fy = 0; fy < filter.size; fy++)
        {
            for (int fx = 0; fx < filter.size; fx++)
            {
                int ix = (x - filter.size / 2 + fx + w) % w;
                int iy = (y - filter.size / 2 + fy + h) % h;

                struct pixel p = input[iy * w + ix];
                red += p.r * filter.matrix[fy][fx];
                green += p.g * filter.matrix[fy][fx];
                blue += p.b * filter.matrix[fy][fx];
            }
        }

        struct pixel *dest = &output[y * w + x];
        dest->r = fmin(fmax((int)(filter.doubleCoeff * red + filter.bias), 0), 255);
        dest->g = fmin(fmax((int)(filter.doubleCoeff * green + filter.bias), 0), 255);
        dest->b = fmin(fmax((int)(filter.doubleCoeff * blue + filter.bias), 0), 255);
    }

    return NULL;
}

void pthread_convolution(unsigned char *pixel_array, int w, int h,
                         struct filter filter, int num_threads, ConvolutionMode mode)
{
    pthread_t threads[num_threads];
    thread_args args[num_threads];

    atomic_int shared_counter = 0;
    unsigned char *result = malloc(w * h * 3);

    for (int i = 0; i < num_threads; i++)
    {
        args[i].input = pixel_array;
        args[i].output = result;
        args[i].w = w;
        args[i].h = h;
        args[i].filter = filter;
        args[i].shared_counter = &shared_counter;

        void *(*thread_func)(void *) = NULL;
        switch (mode)
        {
        case MODE_ROW:
            thread_func = thread_convolve_by_row;
            break;
        case MODE_COLUMN:
            thread_func = thread_convolve_by_collumn;
            break;
        case MODE_PIXEL:
            thread_func = thread_convolve_by_pixel;
            break;
        }

        pthread_create(&threads[i], NULL, thread_func, &args[i]);
    }

    for (int i = 0; i < num_threads; i++)
        pthread_join(threads[i], NULL);

    memcpy(pixel_array, result, w * h * 3);
    free(result);
}
