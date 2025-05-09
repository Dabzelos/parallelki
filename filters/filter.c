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

filter *filter_init(int size, double bias, double doubleCoeff, const double matrix[size][size])
{
    filter *f = malloc(sizeof(filter));
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

        if (!f->matrix[i])
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

filter *init_motion_from_top_left()
{
    int size = 9;
    double doubleCoeff = 1.0 / 9.0;
    double bias = 0.0;
    return filter_init(size, bias, doubleCoeff, motion_blur_matrix);
}

filter *init_blur_soft()
{
    int size = 3;
    double doubleCoeff = 1.0;
    double bias = 0.0;
    return filter_init(size, bias, doubleCoeff, blur_soft_matrix);
}

filter *init_blur()
{
    int size = 5;
    double doubleCoeff = 1.0 / 13.0;
    double bias = 0.0;
    return filter_init(size, bias, doubleCoeff, blur_matrix);
}

filter *init_gauss_smal_blur()
{
    int size = 3;
    double doubleCoeff = 1.0 / 16.0;
    double bias = 0.0;
    return filter_init(size, bias, doubleCoeff, gaussian_small_blur_matrix);
}

filter *init_gauss_big_blur()
{
    int size = 5;
    double doubleCoeff = 1.0 / 256.0;
    double bias = 0.0;
    return filter_init(size, bias, doubleCoeff, gaussian_big_blur_matrix);
}

filter *init_id()
{
    int size = 3;
    double doubleCoeff = 1.0;
    double bias = 0.0;
    return filter_init(size, bias, doubleCoeff, ID_matrix);
}

filter *init_sharpen()
{
    int size = 3;
    double doubleCoeff = 1.0;
    double bias = 0.0;
    return filter_init(size, bias, doubleCoeff, sharpen_matrix);
}

void pprint_filter(filter *filter)
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

void filter_free(filter *f)
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

filter *filter_composition(filter *f1, filter *f2)
{
    int size1 = f1->size;
    int size2 = f2->size;
    int new_size = size1 + size2 - 1;

    double tmp[new_size][new_size];

    for (int i = 0; i < new_size; i++)
        for (int j = 0; j < new_size; j++)
            tmp[i][j] = 0.0;

    for (int i = 0; i < new_size; i++)
    {
        for (int j = 0; j < new_size; j++)
        {
            for (int y1 = 0; y1 < size1; y1++)
            {
                for (int x1 = 0; x1 < size1; x1++)
                {
                    int y2 = i - y1;
                    int x2 = j - x1;

                    if (y2 >= 0 && y2 < size2 && x2 >= 0 && x2 < size2)
                    {
                        tmp[i][j] += f1->matrix[y1][x1] * f2->matrix[y2][x2];
                    }
                }
            }
        }
    }

    double sum = 0.0;
    for (int i = 0; i < new_size; i++)
        for (int j = 0; j < new_size; j++)
            sum += tmp[i][j];

    double new_coeff = (sum != 0.0) ? 1.0 / sum : 1.0;

    double sum_f2 = 0.0;
    for (int i = 0; i < size2; i++)
        for (int j = 0; j < size2; j++)
            sum_f2 += f2->matrix[i][j];

    double new_bias = f2->bias + f2->doubleCoeff * f1->bias * sum_f2;

    return filter_init(new_size, new_bias, new_coeff, tmp);
}

filter *append_filter_matrix_with_zeros(int appendix, filter *f)
{
    int size = f->size;
    double new_matrix[2 * appendix + size][2 * appendix + size];
    memset(new_matrix, 0.0, sizeof(new_matrix));

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            new_matrix[i + appendix][j + appendix] = f->matrix[i][j];
        }
    }

    filter *new_filter = filter_init(size + 2 * appendix, f->bias, f->doubleCoeff, new_matrix);

    return new_filter;
}

filter *generate_random_filter(int size)
{
    double matrix[size][size];

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            matrix[i][j] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
        }
    }

    double bias = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
    double coeff = ((double)rand() / RAND_MAX) * 2.0;

    return filter_init(size, bias, coeff, matrix);
}