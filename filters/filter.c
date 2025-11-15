#include "filter.h"

#include "stdio.h"
#include "stdlib.h"

#include <string.h>

const double motion_blur_matrix[9][9] = {
    {1, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 1, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1}};

const double blur_soft_matrix[3][3] = {{0.0, 0.2, 0.0}, {0.2, 0.2, 0.2}, {0.0, 0.2, 0.0}};

const double blur_matrix[5][5] = {
    {0, 0, 1, 0, 0}, {0, 1, 1, 1, 0}, {1, 1, 1, 1, 1}, {0, 1, 1, 1, 0}, {0, 0, 1, 0, 0}};

const double gaussian_small_blur_matrix[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};

const double gaussian_big_blur_matrix[5][5] = {
    {1, 4, 6, 4, 1}, {4, 16, 24, 16, 4}, {6, 24, 36, 24, 6}, {4, 16, 24, 16, 4}, {1, 4, 6, 4, 1}};

const double ID_matrix[3][3] = {{0, 0, 0}, {0, 1, 0}, {0, 0, 0}};

const double sharpen_matrix[3][3] = {{-1, -1, -1}, {-1, 9, -1}, {-1, -1, -1}};

const double shift_left_matrix[3][3] = {{0, 0, 0}, {1, 0, 0}, {0, 0, 0}};

const double shift_right_matrix[3][3] = {{0, 0, 0}, {0, 0, 1}, {0, 0, 0}};

const double gaus_large_blur[9][9] = {
    {1, 1, 2, 2, 2, 2, 2, 1, 1}, {1, 2, 2, 3, 3, 3, 2, 2, 1}, {2, 2, 3, 4, 5, 4, 3, 2, 2},
    {2, 3, 4, 5, 6, 5, 4, 3, 2}, {2, 3, 5, 6, 7, 6, 5, 3, 2}, {2, 3, 4, 5, 6, 5, 4, 3, 2},
    {2, 2, 3, 4, 5, 4, 3, 2, 2}, {1, 2, 2, 3, 3, 3, 2, 2, 1}, {1, 1, 2, 2, 2, 2, 2, 1, 1}};

filter *filter_init(int size, double bias, double doubleCoeff, const double matrix[size][size]) {
    filter *f = malloc(sizeof(filter));
    f->bias = bias;
    f->doubleCoeff = doubleCoeff;
    f->size = size;

    f->matrix = malloc(size * sizeof(double *));
    if (!f->matrix) {
        free(f);

        return NULL;
    }

    for (int i = 0; i < size; i++) {
        f->matrix[i] = malloc(size * sizeof(double));

        if (!f->matrix[i]) {
            for (int j = 0; j < i; ++j) free(f->matrix[j]);
            free(f->matrix);
            free(f);

            return NULL;
        }

        for (int k = 0; k < size; k++) {
            f->matrix[i][k] = matrix[i][k];
        }
    }

    return f;
}

filter *init_negative_filter() {
    int size = 3;
    double doubleCoeff = -1.0;
    double bias = 255.0;

    return filter_init(size, bias, doubleCoeff, ID_matrix);
}

filter *init_shift_right() {
    int size = 3;
    double doubleCoeff = 1.0;
    double bias = 0.0;
    return filter_init(size, bias, doubleCoeff, shift_right_matrix);
}

filter *init_shift_left() {
    int size = 3;
    double doubleCoeff = 1.0;
    double bias = 0.0;
    return filter_init(size, bias, doubleCoeff, shift_left_matrix);
}

filter *init_motion_from_top_left() {
    int size = 9;
    double doubleCoeff = 1.0 / 9.0;
    double bias = 0.0;
    return filter_init(size, bias, doubleCoeff, motion_blur_matrix);
}

filter *init_gauss_large_blur() {
    int size = 9;
    double doubleCoeff = 1.0 / 213.0;
    double bias = 0.0;
    return filter_init(size, bias, doubleCoeff, gaussian_big_blur_matrix);
}

filter *init_blur_soft() {
    int size = 3;
    double doubleCoeff = 1.0;
    double bias = 0.0;
    return filter_init(size, bias, doubleCoeff, blur_soft_matrix);
}

filter *init_blur() {
    int size = 5;
    double doubleCoeff = 1.0 / 13.0;
    double bias = 0.0;
    return filter_init(size, bias, doubleCoeff, blur_matrix);
}

filter *init_gauss_small_blur() {
    int size = 3;
    double doubleCoeff = 1.0 / 16.0;
    double bias = 0.0;
    return filter_init(size, bias, doubleCoeff, gaussian_small_blur_matrix);
}

filter *init_gauss_big_blur() {
    int size = 5;
    double doubleCoeff = 1.0 / 256.0;
    double bias = 0.0;
    return filter_init(size, bias, doubleCoeff, gaussian_big_blur_matrix);
}

filter *init_id() {
    int size = 3;
    double doubleCoeff = 1.0;
    double bias = 0.0;
    return filter_init(size, bias, doubleCoeff, ID_matrix);
}

filter *init_sharpen() {
    int size = 3;
    double doubleCoeff = 1.0;
    double bias = 0.0;
    return filter_init(size, bias, doubleCoeff, sharpen_matrix);
}

void pprint_filter(filter *filter) {
    printf("Factor: %2f\n", filter->doubleCoeff);
    printf("Bias: %2f\n", filter->bias);
    printf("Size of filter matrix: %d\n", filter->size);

    printf("Matrix of filter: \n");
    for (int i = 0; i < filter->size; i++) {
        for (int j = 0; j < filter->size; j++) {
            printf("%.2f ", filter->matrix[i][j]);
        }
        printf("\n");
    }
}

void filter_free(filter *f) {
    if (!f) return;
    for (int i = 0; i < f->size; ++i) {
        free(f->matrix[i]);
    }

    free(f->matrix);

    free(f);
}

filter *filter_composition(filter *f1, filter *f2) {
    int size1 = f1->size;
    int size2 = f2->size;
    int new_size = size1 + size2 - 1;

    double **new_matrix = (double **)malloc(new_size * sizeof(double *));
    if (new_matrix == NULL) {
        fprintf(stderr, "Memory allocation failed for new matrix\n");
        return NULL;
    }

    for (int i = 0; i < new_size; i++) {
        new_matrix[i] = (double *)malloc(new_size * sizeof(double));
        if (new_matrix[i] == NULL) {
            fprintf(stderr, "Memory allocation failed for matrix row\n");
            for (int j = 0; j < i; j++) {
                free(new_matrix[j]);
            }
            free(new_matrix);
            return NULL;
        }

        for (int j = 0; j < new_size; j++) {
            new_matrix[i][j] = 0.0;
        }
    }

    for (int i = 0; i < new_size; i++) {
        for (int j = 0; j < new_size; j++) {
            for (int fi = 0; fi < size1; fi++) {
                for (int fj = 0; fj < size1; fj++) {
                    int i2 = i - fi;
                    int j2 = j - fj;

                    if (i2 >= 0 && i2 < size2 && j2 >= 0 && j2 < size2) {
                        new_matrix[i][j] += f1->matrix[fi][fj] * f2->matrix[i2][j2];
                    }
                }
            }
        }
    }

    double new_factor = f1->doubleCoeff * f2->doubleCoeff;
    double new_bias = f1->bias * f2->doubleCoeff + f2->bias;

    filter *composed = (filter *)malloc(sizeof(filter));
    if (composed == NULL) {
        fprintf(stderr, "Memory allocation failed for filter\n");
        for (int i = 0; i < new_size; i++) {
            free(new_matrix[i]);
        }
        free(new_matrix);
        return NULL;
    }

    composed->size = new_size;
    composed->doubleCoeff = new_factor;
    composed->bias = new_bias;
    composed->matrix = new_matrix;

    return composed;
}

filter *append_filter_matrix_with_zeros(int appendix, filter *f) {
    int size = f->size;
    double new_matrix[2 * appendix + size][2 * appendix + size];
    memset(new_matrix, 0.0, sizeof(new_matrix));

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            new_matrix[i + appendix][j + appendix] = f->matrix[i][j];
        }
    }

    filter *new_filter = filter_init(size + 2 * appendix, f->bias, f->doubleCoeff, new_matrix);

    return new_filter;
}

filter *generate_random_filter(int size) {
    filter *random_filter = (filter *)malloc(sizeof(filter));
    if (!random_filter) return NULL;

    random_filter->size = size;
    random_filter->matrix = (double **)malloc(size * sizeof(double *));
    if (!random_filter->matrix) {
        free(random_filter);
        return NULL;
    }

    double sum = 0.0;
    for (int i = 0; i < size; i++) {
        random_filter->matrix[i] = (double *)malloc(size * sizeof(double));
        if (!random_filter->matrix[i]) {
            for (int j = 0; j < i; j++) free(random_filter->matrix[j]);
            free(random_filter->matrix);
            free(random_filter);
            return NULL;
        }

        for (int j = 0; j < size; j++) {
            random_filter->matrix[i][j] =
                MIN_FACTOR + ((double)rand() / RAND_MAX) * (MAX_FACTOR - MIN_FACTOR);
            sum += random_filter->matrix[i][j];
        }
    }

    if (sum != 0.0) {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                random_filter->matrix[i][j] /= sum;
            }
        }
    }
    random_filter->doubleCoeff =
        MIN_FACTOR + ((double)rand() / RAND_MAX) * (MAX_FACTOR - MIN_FACTOR);

    random_filter->bias = 0.0;

    return random_filter;
}

filter *init_selected_filter(const char *filter_type) {
    if (strcmp(filter_type, "blur") == 0) {
        return init_blur();
    } else if (strcmp(filter_type, "soft_blur") == 0) {
        return init_blur_soft();
    } else if (strcmp(filter_type, "gaus_small_blur") == 0) {
        return init_gauss_small_blur();
    } else if (strcmp(filter_type, "sharpen") == 0) {
        return init_sharpen();
    } else if (strcmp(filter_type, "motion") == 0) {
        return init_motion_from_top_left();
    } else if (strcmp(filter_type, "gaus_blur") == 0) {
        return init_gauss_big_blur();
    } else if (strcmp(filter_type, "negative") == 0) {
        return init_negative_filter();
    } else if (strcmp(filter_type, "random") == 0) {
        return generate_random_filter(RANDOM_FILTER_SIZE);
    } else if (strcmp(filter_type, "gaus_large_blur") == 0) {
        return init_gauss_large_blur();
    }
    return NULL;
}