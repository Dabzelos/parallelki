#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "../1_task/seq.h"
#include "../2_task/mt.h"
#include "../filters/filter.h"
#include "../libs/stb_image.h"
#include "../libs/stb_image_write.h"
#include "../tests/utils/test_utils.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include "../tests/utils/test_utils.h"

#define MAX_THREADS 16

const char *valid_filters[] = {"blur",      "soft_blur", "gaus_small_blur", "sharpen", "motion",
                               "gaus_blur", "negative",  "random",          NULL};
const char *valid_modes[] = {"by_row", "by_column", "by_pixel", "by_grid", "seq", NULL};

typedef struct {
    char *compute_mode;
    char *input_image_path;
    char *output_image_path;
    char *filter_type;
    int threads_num;
    int block_size;
} ProgramArgs;

const char *check_filter_arg(const char *filter) {
    for (int i = 0; valid_filters[i] != NULL; i++) {
        if (strcmp(filter, valid_filters[i]) == 0) {
            return valid_filters[i];
        }
    }
    fprintf(stderr, "Error: Invalid filter type '%s'\n", filter);
    return NULL;
}

const char *check_mode_arg(const char *mode) {
    for (int i = 0; valid_modes[i] != NULL; i++) {
        if (strcmp(mode, valid_modes[i]) == 0) {
            return valid_modes[i];
        }
    }
    fprintf(stderr, "Error: Invalid compute mode '%s'\n", mode);
    return NULL;
}

void print_usage(const char *program_name) {
    printf("Usage: %s [options] input_image_path output_image_path\n", program_name);
    printf("Options:\n");
    printf(
        "  --filter=<type>    Filter type (blur, soft_blur, gaus_small_blur, sharpen, motion, "
        "gaus_blur, negative, random)\n");
    printf("  --mode=<mode>      Compute mode (by_row, by_column, by_pixel, by_grid, seq)\n");
    printf("  --block=<size>     Block size (default: %d)\n", BLOCK_SIZE);
    printf("  --threads=<num>    Number of threads (default: %d)\n", THREAD_COUNT);
    printf("  --help             Show this help message\n");
}

bool parse_and_validate_args(int argc, const char *argv[], ProgramArgs *args) {
    args->compute_mode = NULL;
    args->input_image_path = NULL;
    args->output_image_path = NULL;
    args->filter_type = NULL;
    args->threads_num = THREAD_COUNT;
    args->block_size = BLOCK_SIZE;

    int path_count = 0;

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--help", 6) == 0) {
            print_usage(argv[0]);
            return false;
        } else if (strncmp(argv[i], "--filter=", 9) == 0) {
            args->filter_type = (char *)check_filter_arg(argv[i] + 9);
            if (!args->filter_type) return false;
            argv[i] = "_";
        } else if (strncmp(argv[i], "--mode=", 7) == 0) {
            args->compute_mode = (char *)check_mode_arg(argv[i] + 7);
            if (!args->compute_mode) return false;
            argv[i] = "_";
        } else if (strncmp(argv[i], "--block=", 8) == 0) {
            args->block_size = atoi(argv[i] + 8);
            if (args->block_size <= 0) {
                fprintf(stderr, "Error: Block size must be positive\n");
                return false;
            }
            argv[i] = "_";
        } else if (strncmp(argv[i], "--threads=", 10) == 0) {
            args->threads_num = atoi(argv[i] + 10);
            if (args->threads_num <= 0 || args->threads_num > MAX_THREADS) {
                fprintf(stderr, "Error: Threads number must be between 1 and %d\n", MAX_THREADS);
                return false;
            }
            argv[i] = "_";
        } else if (argv[i][0] != '-') {
            if (path_count == 0) {
                args->input_image_path = (char *)argv[i];
                path_count++;
            } else if (path_count == 1) {
                args->output_image_path = (char *)argv[i];
                path_count++;
            } else {
                fprintf(stderr, "Error: Too many path arguments specified\n");
                return false;
            }
            argv[i] = "_";
        }
    }

    if (args->input_image_path == NULL) {
        fprintf(stderr, "Error: Input image path not specified\n");
        return false;
    }
    if (args->output_image_path == NULL) {
        fprintf(stderr, "Error: Output image path not specified\n");
        return false;
    }
    if (args->filter_type == NULL) {
        fprintf(stderr, "Error: Filter type not specified\n");
        return false;
    }
    if (args->compute_mode == NULL) {
        fprintf(stderr, "Error: Computation mode not specified\n");
        return false;
    }

    return true;
}

void process_image(const ProgramArgs *args, unsigned char *data, int w, int h) {
    filter *selected_filter = init_selected_filter(args->filter_type);
    if (!selected_filter) {
        fprintf(stderr, "Error: Failed to initialize filter '%s'\n", args->filter_type);
        exit(EXIT_FAILURE);
    }

    if (strcmp(args->compute_mode, "seq") == 0) {
        double start = get_time_in_seconds();
        seq_convolution(data, w, h, *selected_filter);
        double finish = get_time_in_seconds();
        printf("convo time %.9lf\n", finish - start);

    } else {
        if (strcmp(args->compute_mode, "by_row") == 0) {
            mt_convolution(data, w, h, *selected_filter, args->threads_num, MODE_ROW,
                           args->block_size);
        } else if (strcmp(args->compute_mode, "by_column") == 0) {
            mt_convolution(data, w, h, *selected_filter, args->threads_num, MODE_COLUMN,
                           args->block_size);
        } else if (strcmp(args->compute_mode, "by_pixel") == 0) {
            mt_convolution(data, w, h, *selected_filter, args->threads_num, MODE_PIXEL,
                           args->block_size);
        } else if (strcmp(args->compute_mode, "by_grid") == 0) {
            mt_convolution(data, w, h, *selected_filter, args->threads_num, MODE_BLOCK,
                           args->block_size);
        }
    }

    filter_free(selected_filter);
}

int main(int argc, const char *argv[]) {
    ProgramArgs args;

    if (!parse_and_validate_args(argc, argv, &args)) {
        return EXIT_FAILURE;
    }

    printf("Processing image with parameters:\n");
    printf("  Input image path: %s\n", args.input_image_path);
    printf("  Output image path: %s\n", args.output_image_path);
    printf("  Filter type: %s\n", args.filter_type);
    printf("  Compute mode: %s\n", args.compute_mode);
    printf("  Threads number: %d\n", args.threads_num);

    if (strcmp(args.compute_mode, "by_grid") == 0) {
        printf("  Block size: %d\n", args.block_size);
    }

    int w, h, n;
    unsigned char *original_data = stbi_load(args.input_image_path, &w, &h, &n, 3);
    if (!original_data) {
        fprintf(stderr, "Error: Input image loading failed\n");
        return EXIT_FAILURE;
    }
    printf("  Image resolution: %dx%d\n", w, h);

    int image_size = w * h * 3;

    unsigned char *processed_data = malloc(image_size);
    if (!processed_data) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        stbi_image_free(original_data);
        return EXIT_FAILURE;
    }
    memcpy(processed_data, original_data, image_size);

    process_image(&args, processed_data, w, h);

    if (!stbi_write_png(args.output_image_path, w, h, 3, processed_data, w * 3)) {
        fprintf(stderr, "Error: Failed to save processed image\n");
        stbi_image_free(original_data);
        free(processed_data);
        return EXIT_FAILURE;
    }

    printf("Successfully processed image saved to: %s\n", args.output_image_path);

    stbi_image_free(original_data);
    free(processed_data);

    return EXIT_SUCCESS;
}