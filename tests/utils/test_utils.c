#include "stdlib.h"
#include "stdbool.h"
#include "stdio.h"
#include <time.h>
#include "math.h"
#include "../1_task/seq.h"
#include "../2_task/mt.h"

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
    else
    {
        printf("Images match in center region!\n");
    }

    return is_equal;
}

double get_time_in_seconds()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

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