#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>

#include "../libs/stb_image.h"
#include "../libs/stb_image_write.h"

#include "../filters/filter.h"

#include "../1_task/seq.h"
#include "../2_task/mt.h"

#include "../tests/utils/test_utils.h"

#define MAX_THREADS 16

const char *valid_filters[] = {"bb", "mb", "em", "gg", "gb", "co", "sh", "mm", "bo", "mg", NULL};
const char *valid_modes[] = {"by_row", "by_column", "by_pixel", "by_grid", NULL};

typedef struct
{
    char *compute_mode;
    char *image_path;
    char *filter_type;
    int threads_num;
    int block_size;
} ProgramArgs;

const char *check_filter_arg(const char *filter)
{
    for (int i = 0; valid_filters[i] != NULL; i++)
    {
        if (strcmp(filter, valid_filters[i]) == 0)
        {
            return valid_filters[i];
        }
    }
    fprintf(stderr, "Error: Invalid filter type '%s'\n", filter);
    return NULL;
}

const char *check_mode_arg(const char *mode)
{
    for (int i = 0; valid_modes[i] != NULL; i++)
    {
        if (strcmp(mode, valid_modes[i]) == 0)
        {
            return valid_modes[i];
        }
    }
    fprintf(stderr, "Error: Invalid compute mode '%s'\n", mode);
    return NULL;
}

void print_usage(const char *program_name)
{
    printf("Usage: %s [options] image_path\n", program_name);
    printf("Options:\n");
    printf("  --filter=<type>    Filter type (bb, mb, em, gg, gb, co, sh, mm, bo, mg)\n");
    printf("  --mode=<mode>      Compute mode (by_row, by_column, by_pixel, by_grid)\n");
    printf("  --block=<size>     Block size (default: %d)\n", BLOCK_SIZE);
    printf("  --threads=<num>    Number of threads (default: 1)\n");
}

bool parse_and_validate_args(int argc, const char *argv[], ProgramArgs *args)
{
    args->compute_mode = NULL;
    args->image_path = NULL;
    args->filter_type = NULL;
    args->threads_num = THREAD_COUNT;
    args->block_size = BLOCK_SIZE;

    for (int i = 1; i < argc; i++)
    {
        if (strncmp(argv[i], "--help", 6) == 0)
        {
            print_usage(argv[0]);
        }
        if (strncmp(argv[i], "--filter=", 9) == 0)
        {
            args->filter_type = (char *)check_filter_arg(argv[i] + 9);
            if (!args->filter_type)
                return false;
            argv[i] = "_";
        }
        else if (strncmp(argv[i], "--mode=", 7) == 0)
        {
            args->compute_mode = (char *)check_mode_arg(argv[i] + 7);
            if (!args->compute_mode)
                return false;
            argv[i] = "_";
        }
        else if (strncmp(argv[i], "--block=", 8) == 0)
        {
            args->block_size = atoi(argv[i] + 8);
            if (args->block_size <= 0)
            {
                fprintf(stderr, "Error: Block size must be positive\n");
                return false;
            }
            argv[i] = "_";
        }
        else if (strncmp(argv[i], "--threads=", 10) == 0)
        {
            args->threads_num = atoi(argv[i] + 10);
            if (args->threads_num <= 0)
            {
                fprintf(stderr, "Error: Threads number must be positive\n");
                return false;
            }
            argv[i] = "_";
        }
        else if (argv[i][0] != '-')
        {
            if (args->image_path != NULL)
            {
                fprintf(stderr, "Error: Multiple image paths specified\n");
                return false;
            }
            args->image_path = (char *)argv[i];
            argv[i] = "_";
        }
    }

    if (args->image_path == NULL)
    {
        fprintf(stderr, "Error: Image path not specified\n");
        return false;
    }
    if (args->filter_type == NULL)
    {
        fprintf(stderr, "Error: Filter type not specified\n");
        return false;
    }
    if (args->compute_mode == NULL)
    {
        fprintf(stderr, "Error: Computation mode not specified\n");
        return false;
    }

    return true;
}

int main(int argc, const char *argv[])
{
    ProgramArgs args;

    if (!parse_and_validate_args(argc, argv, &args))
    {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    printf("Successfully parsed arguments:\n");
    printf("  Image path: %s\n", args.image_path);
    printf("  Filter type: %s\n", args.filter_type);
    if (args.compute_mode)
        printf("  Compute mode: %s\n", args.compute_mode);
    printf("  Threads number: %d\n", args.threads_num);
    printf("  Block size: %d\n", args.block_size);

    int w, h, n;

    unsigned char *original_data = stbi_load(args.image_path, &w, &h, &n, 3);
    if (!original_data)
    {
        printf("Image loading failed\n");
        exit(1);
    }
    int image_size = w * h * 3;

    unsigned char *data1 = malloc(image_size);
    if (!data1)
    {
        printf("Memory allocation failed\n");
        exit(1);
    }

    memcpy(data1, original_data, image_size);

    if (data1 == NULL)
    {
        printf("naaah we trippin");

        return 0;
    }

    filter *filter1 = init_sharpen();

    measure_performance(data1, w, h, 8, *filter1);

    free(original_data);
    free(data1);
    filter_free(filter1);
}
