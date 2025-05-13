#include "../1_task/seq.h"
#include "../filters/filter.h"
#include "../tests/utils/test_utils.h"
#include "stdlib.h"

#define MAX_APPEND 3

void test_predefined_filters_appended_with_zeros()
{
    srand(time(NULL));

    int w, h, n;

    unsigned char *original_data = stbi_load("images/mypersonalphoto.bmp", &w, &h, &n, 3);
    if (!original_data)
    {
        printf("Failed to load image\n");
        return;
    }

    int image_size = w * h * 3;

    filter *(*filter_initializers[])() = {
        init_motion_from_top_left,
        init_blur_soft,
        init_blur,
        init_gauss_smal_blur,
        init_gauss_big_blur,
        init_sharpen};

    const char *filter_names[] = {
        "motion_from_top_left",
        "blur_soft",
        "blur",
        "gauss_small_blur",
        "gauss_big_blur",
        "sharpen"};

    const int num_filters = sizeof(filter_initializers) / sizeof(filter_initializers[0]);

    for (int i = 0; i < num_filters; i++)
    {
        for (int append = 0; append <= MAX_APPEND; append++)
        {
            unsigned char *data1 = malloc(image_size);
            unsigned char *data2 = malloc(image_size);
            if (!data1 || !data2)
            {
                printf("Memory allocation failed\n");
                break;
            }

            memcpy(data1, original_data, image_size);
            memcpy(data2, original_data, image_size);

            filter *original_filter = filter_initializers[i]();
            filter *appended_filter = append_filter_matrix_with_zeros(append, original_filter);

            seq_convolution(data1, w, h, *original_filter);
            seq_convolution(data2, w, h, *appended_filter);

            if (!compare_images(data1, data2, w, h, 3))
            {
                printf("FAILED: %s with %d zero-append(s)\n", filter_names[i], append);
                pprint_filter(original_filter);
                pprint_filter(appended_filter);
            }
            else
            {
                printf("PASSED: %s with %d zero-append(s)\n", filter_names[i], append);
            }

            if (original_filter)
                filter_free(original_filter);
            if (appended_filter && appended_filter != original_filter)
                filter_free(appended_filter);

            free(data1);
            free(data2);
        }
    }

    stbi_image_free(original_data);
}

void test_random_filter_appended_with_zeros()
{
    srand(time(NULL));

    int w, h, n;

    unsigned char *original_data = stbi_load("/home/dabzelos/Desktop/paralelki/images/mypersonalphoto.bmp", &w, &h, &n, 3);
    if (!original_data)
    {
        printf("Failed to load image\n");
        return 0;
    }

    int image_size = w * h * 3;

    for (int kernel_size = 0; kernel_size < 7; kernel_size++)
    {
        unsigned char *data1 = malloc(image_size);
        unsigned char *data2 = malloc(image_size);
        if (!data1 || !data2)
        {
            printf("Memory allocation failed\n");
            break;
        }

        memcpy(data1, original_data, image_size);
        memcpy(data2, original_data, image_size);

        filter *f1 = generate_random_filter(kernel_size);

        filter *f2 = append_filter_matrix_with_zeros(2, f1);

        seq_convolution(data1, w, h, *f1);
        seq_convolution(data2, w, h, *f2);

        if (!compare_images(data1, data2, w, h, 3))
        {
            printf("Test_Filter_Composition_Apply Failed\n On these filters:\n");

            pprint_filter(f1);
            pprint_filter(f2);
        }
        else
        {
            printf("Test_Filter_Composition_Apply Passed\n");
        }

        filter_free(f1);
        filter_free(f2);
    }
}

void test_negative_filter_composition_gives_id()
{
    int w, h, n;

    unsigned char *original_data = stbi_load("images/mypersonalphoto.bmp", &w, &h, &n, 3);
    if (!original_data)
    {
        printf("Failed to load image\n");
        return;
    }

    int image_size = w * h * 3;

    unsigned char *data1 = malloc(image_size);
    memcpy(data1, original_data, image_size);

    filter *f1 = init_negative_filter();
    filter *f2 = init_negative_filter();

    filter *compositon = filter_composition(f1, f2);

    seq_convolution(data1, w, h, *f1);

    seq_convolution(data1, w, h, *f2);

    if (!compare_images(original_data, data1, w, h, 3))
    {
        printf("FAILED: Composition of negative filters\n");
    }
    else
    {
        printf("PASSED: Composition of negative filters\n");
    }

    filter_free(f1);
    filter_free(f2);
    filter_free(compositon);
    stbi_image_free(original_data);
    stbi_image_free(data1);
}

void test_opposite_shift_filters_composition_gives_id()
{
    int w, h, n;

    unsigned char *original_data = stbi_load("images/mypersonalphoto.bmp", &w, &h, &n, 3);
    if (!original_data)
    {
        printf("Failed to load image\n");
        return;
    }

    int image_size = w * h * 3;

    unsigned char *data1 = malloc(image_size);
    memcpy(data1, original_data, image_size);

    filter *f1 = init_shift_left();
    filter *f2 = init_shift_right();

    filter *compositon = filter_composition(f1, f2);

    seq_convolution(data1, w, h, *f1);

    seq_convolution(data1, w, h, *f2);

    if (!compare_images(original_data, data1, w, h, 3))
    {
        printf("FAILED: Composition of opposite shift filters\n");
    }
    else
    {
        printf("PASSED: Composition of opposite shift filters\n");
    }

    filter_free(f1);
    filter_free(f2);
    filter_free(compositon);
    stbi_image_free(original_data);
    stbi_image_free(data1);
}

void test_id_filter()
{
    int w, h, n;

    unsigned char *original_data = stbi_load("images/mypersonalphoto.bmp", &w, &h, &n, 3);
    if (!original_data)
    {
        printf("Failed to load image\n");
        return;
    }

    int image_size = w * h * 3;

    unsigned char *data1 = malloc(image_size);
    memcpy(data1, original_data, image_size);

    filter *f1 = init_id();

    seq_convolution(data1, w, h, *f1);

    if (!compare_images(original_data, data1, w, h, 3))
    {
        printf("FAILED: ID filter equals origin image\n");
    }
    else
    {
        printf("PASSED: ID filter equals origin image\n");
    }
}

void test_shift_filter_left_gives_id()
{
    int w, h, n;
    unsigned char *original_data = stbi_load("images/mypersonalphoto.bmp", &w, &h, &n, 3);
    if (!original_data)
    {
        printf("Failed to load image\n");
        return;
    }

    int image_size = w * h * 3;
    unsigned char *shifted_data = malloc(image_size);
    memcpy(shifted_data, original_data, image_size);

    filter *shift_filter = init_shift_left();

    seq_convolution(shifted_data, w, h, *shift_filter);

    int passed = 1;
    for (int y = 0; y < h; y++)
    {
        for (int x = 1; x < w; x++)
        {
            for (int c = 0; c < 3; c++)
            {
                int original_idx = (y * w + (x - 1)) * 3 + c;
                int shifted_idx = (y * w + x) * 3 + c;

                if (original_data[original_idx] != shifted_data[shifted_idx])
                {
                    passed = 0;
                }
            }
        }
    }

    if (passed)
    {
        printf("PASSED: Shift filter moves pixels right by 1\n");
    }
    else
    {
        printf("FAILED: Shift filter doesn't move pixels correctly\n");
    }

    filter_free(shift_filter);
    stbi_image_free(original_data);
    free(shifted_data);
}

int main()
{
    srand(time(NULL));
    test_filter_appended_with_zeros();
}