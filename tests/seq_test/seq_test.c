#include "../1_task/seq.h"

#include "../filters/filter.h"
#include "../tests/seq_test/seq_test.h"
#include "../tests/utils/test_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
void compare_filter_results(filter *f1, filter *f2, int w, int h, const char *test_name) {
    unsigned char *original_data = generate_random_image(w, h);
    int image_size = w * h * 3;

    unsigned char *data1 = malloc(image_size);
    unsigned char *data2 = malloc(image_size);
    memcpy(data1, original_data, image_size);
    memcpy(data2, original_data, image_size);

    seq_convolution(data1, w, h, *f1);
    seq_convolution(data2, w, h, *f2);

    if (!compare_images(data1, data2, w, h, 3)) {
        printf("FAILED: %s (size %dx%d)\n", test_name, w, h);
        pprint_filter(f1);
        pprint_filter(f2);
    } else {
        printf("PASSED: %s (size %dx%d)\n", test_name, w, h);
    }

    free(original_data);
    free(data1);
    free(data2);
}

void test_random_filters_composition() {
    srand(time(NULL));

    for (int size = MIN_IMAGE_SIZE; size <= MAX_IMAGE_SIZE; size += STEP_SIZE) {
        unsigned char *original_data = generate_random_image(size, size);
        int image_size = size * size * 3;

        unsigned char *data_sequential = malloc(image_size);
        unsigned char *data_composition = malloc(image_size);
        memcpy(data_sequential, original_data, image_size);
        memcpy(data_composition, original_data, image_size);

        int kernel_size1 = 3 + 2 * (rand() % 2);
        int kernel_size2 = 3 + 2 * (rand() % 2);
        filter *f1 = generate_random_filter(kernel_size1);
        filter *f2 = generate_random_filter(kernel_size2);

        seq_convolution(data_sequential, size, size, *f1);
        seq_convolution(data_sequential, size, size, *f2);

        filter *composition = filter_composition(f1, f2);
        seq_convolution(data_composition, size, size, *composition);

        if (!compare_images(data_sequential, data_composition, size, size, 3)) {
            printf("FAILED: Random filters composition (size %dx%d, kernels %dx%d and %dx%d)\n",
                   size, size, kernel_size1, kernel_size1, kernel_size2, kernel_size2);
            pprint_filter(f1);
            pprint_filter(f2);
            pprint_filter(composition);
        } else {
            printf("PASSED: Random filters composition (size %dx%d, kernels %dx%d and %dx%d)\n",
                   size, size, kernel_size1, kernel_size1, kernel_size2, kernel_size2);
        }

        filter_free(f1);
        filter_free(f2);
        filter_free(composition);
        free(original_data);
        free(data_sequential);
        free(data_composition);
    }
}

/*
    Tests for predefined filters that apennding them with zeros gives id
*/
void test_predefined_filters_appended_with_zeros() {
    srand(time(NULL));

    filter *(*filter_initializers[])() = {
        init_motion_from_top_left, init_blur_soft,      init_blur,
        init_gauss_small_blur,     init_gauss_big_blur, init_sharpen};

    const char *filter_names[] = {"motion_from_top_left", "blur_soft",      "blur",
                                  "gauss_small_blur",     "gauss_big_blur", "sharpen"};

    const int num_filters = sizeof(filter_initializers) / sizeof(filter_initializers[0]);

    int size = 600;  // made this constant due to large time of computation

    for (int i = 0; i < num_filters; i++) {
        for (int append = 1; append <= MAX_APPEND; append++) {
            filter *original_filter = filter_initializers[i]();
            filter *appended_filter = append_filter_matrix_with_zeros(append, original_filter);

            char test_name[256];
            snprintf(test_name, sizeof(test_name), "%s with %d zero-append(s)", filter_names[i],
                     append);

            compare_filter_results(original_filter, appended_filter, size, size, test_name);

            filter_free(original_filter);
            if (appended_filter != original_filter) {
                filter_free(appended_filter);
            }
        }
    }
}

/*
 * Tests that if we append any matrix with zeros, nothing will change
 */
void test_random_filter_appended_with_zeros() {
    char test_name[256];

    srand(time(NULL));

    for (int size = MIN_IMAGE_SIZE; size <= MAX_IMAGE_SIZE; size += STEP_SIZE) {
        for (int kernel_size = 1; kernel_size <= 5; kernel_size = kernel_size + 2) {
            filter *f1 = generate_random_filter(kernel_size);
            filter *f2 = append_filter_matrix_with_zeros(1, f1);

            snprintf(test_name, sizeof(test_name),
                     "Random filter with %d x %d matrix appended with %d zeros", kernel_size,
                     kernel_size, 1);

            compare_filter_results(f1, f2, size, size, test_name);

            filter_free(f1);
            filter_free(f2);
        }
    }
}

/*
 * Tests for filters which composition gives id
 */
void test_negative_filter_composition_gives_id() {
    test_filter_composition(init_negative_filter(), init_negative_filter(),
                            "Composition of negative filters");
}

void test_opposite_shift_filters_composition_gives_id() {
    test_filter_composition(init_shift_left(), init_shift_right(),
                            "Composition of opposite shift filters");
}

void test_id_filters_composition_gives_id() {
    test_filter_composition(init_id(), init_id(), "Composition of id's");
}

/*
 * Tests for filters that results are known
 */
void test_id_filter() {
    srand(time(NULL));

    for (int size = MIN_IMAGE_SIZE; size <= MAX_IMAGE_SIZE; size += STEP_SIZE) {
        unsigned char *original_data = generate_random_image(size, size);
        int image_size = size * size * 3;

        unsigned char *data = malloc(image_size);
        memcpy(data, original_data, image_size);

        filter *f = init_id();
        seq_convolution(data, size, size, *f);

        if (!compare_images(original_data, data, size, size, 3)) {
            printf("FAILED: ID filter equals origin image (size %dx%d)\n", size, size);
        } else {
            printf("PASSED: ID filter equals origin image (size %dx%d)\n", size, size);
        }

        filter_free(f);
        free(original_data);
        free(data);
    }
}

void test_shift_filter_left() {
    srand(time(NULL));

    for (int size = MIN_IMAGE_SIZE; size <= MAX_IMAGE_SIZE; size += STEP_SIZE) {
        unsigned char *original_data = generate_random_image(size, size);
        int image_size = size * size * 3;

        unsigned char *shifted_data = malloc(image_size);
        memcpy(shifted_data, original_data, image_size);

        filter *shift_filter = init_shift_left();
        seq_convolution(shifted_data, size, size, *shift_filter);

        int passed = 1;
        for (int y = 0; y < size; y++) {
            for (int x = 1; x < size; x++) {
                for (int c = 0; c < 3; c++) {
                    int original_idx = (y * size + (x - 1)) * 3 + c;
                    int shifted_idx = (y * size + x) * 3 + c;

                    if (original_data[original_idx] != shifted_data[shifted_idx]) {
                        passed = 0;
                        break;
                    }
                }
                if (!passed) break;
            }
            if (!passed) break;
        }

        if (passed) {
            printf("PASSED: Shift filter moves pixels right by 1 (size %dx%d)\n", size, size);
        } else {
            printf("FAILED: Shift filter doesn't move pixels correctly (size %dx%d)\n", size, size);
        }

        filter_free(shift_filter);
        free(original_data);
        free(shifted_data);
    }
}
