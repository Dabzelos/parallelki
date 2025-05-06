#include "stdio.h"
#include "stdlib.h"
#include "filter.h"
#include <string.h>

const double motion_blur_matrix[9][9] = {
    {1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1}};

const double blur_soft_matrix[3][3] = {
    {0.0, 0.2, 0.0},
    {0.2, 0.2, 0.2},
    {0.0, 0.2, 0.0}};

const double blur_matrix[5][5] = {
    {0, 0, 1, 0, 0},
    {0, 1, 1, 1, 0},
    {1, 1, 1, 1, 1},
    {0, 1, 1, 1, 0},
    {0, 0, 1, 0, 0}};

const double gaussian_small_blur_matrix[3][3] = {
    {1, 2, 1},
    {2, 4, 2},
    {1, 2, 1}};

const double gaussian_big_blur_matrix[5][5] = {
    {1, 4, 6, 4, 1},
    {4, 16, 24, 16, 4},
    {6, 24, 36, 24, 6},
    {4, 16, 24, 16, 4},
    {1, 4, 6, 4, 1}};

const double ID_matrix[3][3] = {
    {0, 0, 0},
    {0, 1, 0},
    {0, 0, 0}};

const double sharpen_matrix[3][3] = {
    {-1, -1, -1},
    {-1, 9, -1},
    {-1, -1, -1}};

struct filter *filter_init(int size, double bias, double doubleCoeff, const double matrix[size][size])
{
    struct filter *f = malloc(sizeof(struct filter));
    f->bias = bias;
    f->doubleCoeff = doubleCoeff;
    f->size = size;

    f->matrix = malloc(size * sizeof(double *));
    if (!f->matrix)
    {
        free(f);
        return NULL;
    }

    for (int i = 0; i < size; i++)
    {
        f->matrix[i] = malloc(size * sizeof(double));

        if (!f->matrix)
        {
            for (int j = 0; j < i; ++j)
                free(f->matrix[j]);
            free(f->matrix);
            free(f);

            return NULL;
        }

        for (int k = 0; k < size; k++)
        {
            f->matrix[i][k] = matrix[i][k];
        }
    }

    return f;
}

struct filter *init_motion_from_top_left()
{
    int size = 9;
    double doubleCoeff = 1.0 / 9.0;
    double bias = 0.0;
    return filter_init(size, bias, doubleCoeff, motion_blur_matrix);
}

struct filter *init_blur_soft()
{
    int size = 3;
    double doubleCoeff = 1.0;
    double bias = 0.0;
    return filter_init(size, bias, doubleCoeff, blur_soft_matrix);
}

struct filter *init_blur()
{
    int size = 5;
    double doubleCoeff = 1.0 / 13.0;
    double bias = 0.0;
    return filter_init(size, bias, doubleCoeff, blur_matrix);
}

struct filter *init_gauss_smal_blur()
{
    int size = 3;
    double doubleCoeff = 1.0 / 16.0;
    double bias = 0.0;
    return filter_init(size, bias, doubleCoeff, gaussian_small_blur_matrix);
}

struct filter *init_gauss_big_blur()
{
    int size = 5;
    double doubleCoeff = 1.0 / 256.0;
    double bias = 0.0;
    return filter_init(size, bias, doubleCoeff, gaussian_big_blur_matrix);
}

struct filter *init_id()
{
    int size = 3;
    double doubleCoeff = 1.0;
    double bias = 0.0;
    return filter_init(size, bias, doubleCoeff, ID_matrix);
}

struct filter *init_sharpen()
{
    int size = 3;
    double doubleCoeff = 1.0;
    double bias = 0.0;
    return filter_init(size, bias, doubleCoeff, sharpen_matrix);
}

void pprint_filter(struct filter *filter)
{
    printf("Factor: %2f\n", filter->doubleCoeff);
    printf("Bias: %2f\n", filter->bias);
    printf("Size of filter matrix: %d\n", filter->size);

    printf("Matrix of filter: \n");
    for (int i = 0; i < filter->size; i++)
    {
        for (int j = 0; j < filter->size; j++)
        {
            printf("%.2f ", filter->matrix[i][j]);
        }
        printf("\n");
    }
}

void filter_free(struct filter *f)
{
    if (!f)
        return;
    for (int i = 0; i < f->size; ++i)
    {
        free(f->matrix[i]);
    }

    free(f->matrix);

    free(f);
}

struct filter *filter_composition(struct filter *f1, struct filter *f2)
{
    int size_1 = f1->size;
    int size_2 = f2->size;

    int r_size = size_1 + size_2 - 1;
    double r_doubleCoeff = f1->doubleCoeff * f2->doubleCoeff;

    double sum_f2 = 0.0;
    for (int y = 0; y < size_2; y++)
    {
        for (int x = 0; x < size_2; x++)
        {
            sum_f2 += f2->matrix[y][x];
        }
    }

    double r_bias = f2->bias + f2->doubleCoeff * f1->bias * sum_f2;

    double **matrix = malloc(r_size * sizeof(double *));

    for (int i = 0; i < r_size; i++)
    {
        matrix[i] = calloc(r_size, sizeof(double));
    }

    for (int y1 = 0; y1 < size_1; y1++)
    {
        for (int x1 = 0; x1 < size_1; x1++)
        {
            for (int y2 = 0; y2 < size_2; y2++)
            {
                for (int x2 = 0; x2 < size_2; x2++)
                {
                    int ry = y1 + y2;
                    int rx = x1 + x2;
                    matrix[ry][rx] += f1->matrix[y1][x1] * f2->matrix[y2][x2];
                }
            }
        }
    }

    return filter_init(r_size, r_bias, r_doubleCoeff, matrix);
}
