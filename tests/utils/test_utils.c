#include "stdlib.h"
#include "stdbool.h"
#include "stdio.h"

#include <time.h>
#include "math.h"
#include <string.h>

#include "../1_task/seq.h"
#include "../2_task/mt.h"
#include "../tests/utils/test_utils.h"

bool compare_images(const unsigned char *img1, const unsigned char *img2, int width, int height, int channels)
{
    bool is_equal = true;
    int mismatches = 0;
    char maxdif = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            for (int c = 0; c < channels; c++)
            {
                int idx = (y * width + x) * channels + c;
                if (abs(img1[idx] - img2[idx]) > 1)
                {
                    maxdif = fmax(maxdif, abs(img1[idx] - img2[2]));
                    mismatches++;

                    is_equal = false;
                }
            }
        }
    }

    if (!is_equal)
    {
        printf("Maxdiff : %d\n", maxdif);

        printf("Total mismatches in center region: %d\n", mismatches);
    }

    return is_equal;
}

double get_time_in_seconds()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

/*
void measure_performance(unsigned char *data, int w, int h, int threads, filter filter)
{
    double start_time, end_time;

    start_time = get_time_in_seconds();
    seq_convolution(data, w, h, filter);
    end_time = get_time_in_seconds();
    printf("Sequential time: %f seconds\n", end_time - start_time);

    start_time = get_time_in_seconds();
    pthread_convolution(data, w, h, filter, threads, MODE_ROW);
    end_time = get_time_in_seconds();
    printf("Parallel (by row) time: %f seconds\n", end_time - start_time);

    start_time = get_time_in_seconds();
    pthread_convolution(data, w, h, filter, threads, MODE_COLUMN);
    end_time = get_time_in_seconds();
    printf("Parallel (by collumn) time: %f seconds\n", end_time - start_time);

    start_time = get_time_in_seconds();
    pthread_convolution(data, w, h, filter, threads, MODE_PIXEL);
    end_time = get_time_in_seconds();
    printf("Parallel (by pixel) time: %f seconds\n", end_time - start_time);
}
*/

unsigned char *generate_random_image(int w, int h)
{
    int image_size = w * h * 3;

    unsigned char *rand_img = malloc(image_size);
    for (int i = 0; i < image_size; i += 3)
    {
        rand_img[i] = rand() % 256;     // stands for r
        rand_img[i + 1] = rand() % 256; // for g
        rand_img[i + 2] = rand() % 256; // for b
    }
    return rand_img;
}

void test_filter_composition(filter *f1, filter *f2, const char *test_name)
{
    for (int size = MIN_IMAGE_SIZE; size <= MAX_IMAGE_SIZE; size += STEP_SIZE)
    {
        unsigned char *original_data = generate_random_image(size, size);
        int image_size = size * size * 3;

        unsigned char *data = malloc(image_size);
        memcpy(data, original_data, image_size);

        filter *composition = filter_composition(f1, f2);

        seq_convolution(data, size, size, *f1);
        seq_convolution(data, size, size, *f2);

        if (!compare_images(original_data, data, size, size, 3))
        {
            printf("FAILED: %s (size %dx%d)\n", test_name, size, size);
        }
        else
        {
            printf("PASSED: %s (size %dx%d)\n", test_name, size, size);
        }

        filter_free(composition);
        free(original_data);
        free(data);
    }
    filter_free(f1);
    filter_free(f2);
}
