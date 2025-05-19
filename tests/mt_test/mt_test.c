#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "../1_task/seq.h"
#include "../2_task/mt.h"
#include "../filters/filter.h"
#include "../tests/utils/test_utils.h"

void test_mt_convolution(ConvolutionMode MODE, const char *test_name)
{
    for (int size = MIN_IMAGE_SIZE; size < MAX_IMAGE_SIZE; size += STEP_SIZE)
    {
        unsigned char *orig_image = generate_random_image(size, size);
        int img_size = size * size * 3;
        for (int kern_size = 1; kern_size <= 5; kern_size += 2)
        {
            filter *filter = generate_random_filter(kern_size);

            unsigned char *seq_data = malloc(img_size);
            unsigned char *mt_data = malloc(img_size);
            memcpy(seq_data, orig_image, img_size);
            memcpy(mt_data, orig_image, img_size);

            seq_convolution(seq_data, size, size, *filter);
            mt_convolution(mt_data, size, size, *filter, THREAD_COUNT, MODE, 16);

            if (!compare_images(mt_data, seq_data, size, size, 3))
            {
                printf("FAILED: %s, image size:%d x %d, matrix size: %d\n", test_name, size, size, kern_size);
                pprint_filter(filter);
            }
            else
            {
                printf("PASSED: %s, image size:%d x %d, matrix size: %d\n", test_name, size, size, kern_size);
            }

            filter_free(filter);
            free(mt_data);
            free(seq_data);
        }

        free(orig_image);
    }
}
