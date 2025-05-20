#include "../2_task/mt.h"

#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *thread_convolve_by_row(void *arg) {
    thread_args *args = (thread_args *)arg;
    int w = args->w, h = args->h;
    filter filter = args->filter;

    pixel *input = (pixel *)args->input;
    pixel *output = (pixel *)args->output;

    int y;
    while ((y = atomic_fetch_add(args->shared_counter, 1)) < h) {
        for (int x = 0; x < w; x++) {
            double red = 0.0, green = 0.0, blue = 0.0;

            for (int fy = 0; fy < filter.size; fy++) {
                for (int fx = 0; fx < filter.size; fx++) {
                    int ix = (x - filter.size / 2 + fx + w) % w;
                    int iy = (y - filter.size / 2 + fy + h) % h;

                    pixel p = input[iy * w + ix];
                    red += p.r * filter.matrix[fy][fx];
                    green += p.g * filter.matrix[fy][fx];
                    blue += p.b * filter.matrix[fy][fx];
                }
            }

            pixel *dest = &output[y * w + x];
            dest->r = fmin(fmax((int)round(filter.doubleCoeff * red + filter.bias), 0), 255);
            dest->g = fmin(fmax((int)round(filter.doubleCoeff * green + filter.bias), 0), 255);
            dest->b = fmin(fmax((int)round(filter.doubleCoeff * blue + filter.bias), 0), 255);
        }
    }

    return NULL;
}

void *thread_convolve_by_collumn(void *arg) {
    thread_args *args = (thread_args *)arg;
    int w = args->w, h = args->h;
    filter filter = args->filter;

    pixel *input = (pixel *)args->input;
    pixel *output = (pixel *)args->output;

    int x;
    while ((x = atomic_fetch_add(args->shared_counter, 1)) < w) {
        for (int y = 0; y < h; y++) {
            double red = 0, green = 0, blue = 0;

            for (int fy = 0; fy < filter.size; fy++) {
                for (int fx = 0; fx < filter.size; fx++) {
                    int ix = (x - filter.size / 2 + fx + w) % w;
                    int iy = (y - filter.size / 2 + fy + h) % h;

                    pixel p = input[iy * w + ix];
                    red += p.r * filter.matrix[fy][fx];
                    green += p.g * filter.matrix[fy][fx];
                    blue += p.b * filter.matrix[fy][fx];
                }
            }

            pixel *dest = &output[y * w + x];
            dest->r = fmin(fmax((int)round(filter.doubleCoeff * red + filter.bias), 0), 255);
            dest->g = fmin(fmax((int)round(filter.doubleCoeff * green + filter.bias), 0), 255);
            dest->b = fmin(fmax((int)round(filter.doubleCoeff * blue + filter.bias), 0), 255);
        }
    }

    return NULL;
}

void *thread_convolve_by_pixel(void *arg) {
    thread_args *args = (thread_args *)arg;
    int w = args->w, h = args->h;
    filter filter = args->filter;

    pixel *input = (pixel *)args->input;
    pixel *output = (pixel *)args->output;

    int index;
    while ((index = atomic_fetch_add(args->shared_counter, 1)) < w * h) {
        int x = index % w;
        int y = index / w;

        double red = 0, green = 0, blue = 0;

        for (int fy = 0; fy < filter.size; fy++) {
            for (int fx = 0; fx < filter.size; fx++) {
                int ix = (x - filter.size / 2 + fx + w) % w;
                int iy = (y - filter.size / 2 + fy + h) % h;

                pixel p = input[iy * w + ix];
                red += p.r * filter.matrix[fy][fx];
                green += p.g * filter.matrix[fy][fx];
                blue += p.b * filter.matrix[fy][fx];
            }
        }

        pixel *dest = &output[y * w + x];
        dest->r = fmin(fmax((int)round(filter.doubleCoeff * red + filter.bias), 0), 255);
        dest->g = fmin(fmax((int)round(filter.doubleCoeff * green + filter.bias), 0), 255);
        dest->b = fmin(fmax((int)round(filter.doubleCoeff * blue + filter.bias), 0), 255);
    }

    return NULL;
}

// номер блока * размер блока + индекс < номер блока* размер блока < size или чето такое надо
// подумать
void *thread_convolve_by_block(void *arg) {
    thread_args *args = (thread_args *)arg;
    int w = args->w, h = args->h;
    filter filter = args->filter;
    int block_size = args->block_size;

    pixel *input = (pixel *)args->input;
    pixel *output = (pixel *)args->output;

    int block_index;
    while ((block_index = atomic_fetch_add(args->shared_counter, 1)) <
           ((w + block_size - 1) / block_size) * ((h + block_size - 1) / block_size)) {
        int blocks_per_row = (w + block_size - 1) / block_size;
        int block_x = block_index % blocks_per_row;
        int block_y = block_index / blocks_per_row;

        int start_x = block_x * block_size;
        int end_x = (block_x + 1) * block_size;
        if (end_x > w) end_x = w;

        int start_y = block_y * block_size;
        int end_y = (block_y + 1) * block_size;
        if (end_y > h) end_y = h;

        for (int y = start_y; y < end_y; y++) {
            for (int x = start_x; x < end_x; x++) {
                double red = 0.0, green = 0.0, blue = 0.0;

                for (int fy = 0; fy < filter.size; fy++) {
                    for (int fx = 0; fx < filter.size; fx++) {
                        int ix = (x - filter.size / 2 + fx + w) % w;
                        int iy = (y - filter.size / 2 + fy + h) % h;

                        pixel p = input[iy * w + ix];
                        red += p.r * filter.matrix[fy][fx];
                        green += p.g * filter.matrix[fy][fx];
                        blue += p.b * filter.matrix[fy][fx];
                    }
                }

                pixel *dest = &output[y * w + x];
                dest->r = fmin(fmax((int)round(filter.doubleCoeff * red + filter.bias), 0), 255);
                dest->g = fmin(fmax((int)round(filter.doubleCoeff * green + filter.bias), 0), 255);
                dest->b = fmin(fmax((int)round(filter.doubleCoeff * blue + filter.bias), 0), 255);
            }
        }
    }

    return NULL;
}

void mt_convolution(unsigned char *pixel_array, int w, int h, filter filter, int num_threads,
                    ConvolutionMode mode, int block_size) {
    pthread_t threads[num_threads];
    thread_args args[num_threads];

    atomic_int shared_counter = 0;
    unsigned char *result = malloc(w * h * 3);

    for (int i = 0; i < num_threads; i++) {
        args[i].input = pixel_array;
        args[i].output = result;
        args[i].w = w;
        args[i].h = h;
        args[i].filter = filter;
        args[i].shared_counter = &shared_counter;

        void *(*thread_func)(void *) = NULL;
        switch (mode) {
            case MODE_ROW:
                thread_func = thread_convolve_by_row;
                break;
            case MODE_COLUMN:
                thread_func = thread_convolve_by_collumn;
                break;
            case MODE_PIXEL:
                thread_func = thread_convolve_by_pixel;
                break;
            case MODE_BLOCK:

                args[i].block_size = (block_size <= 1) ? BLOCK_SIZE : block_size;
                thread_func = thread_convolve_by_block;
                break;
        }

        pthread_create(&threads[i], NULL, thread_func, &args[i]);
    }

    double start = get_time_in_seconds();

    for (int i = 0; i < num_threads; i++) pthread_join(threads[i], NULL);

    double finish = get_time_in_seconds();

    printf("convo time %lf", (finish - start));

    memcpy(pixel_array, result, w * h * 3);
    free(result);
}
