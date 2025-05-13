#pragma once

#define MIN_FACTOR 0.0005
#define MAX_FACTOR 1.0

#pragma pack(push, 1)
typedef struct
{
    unsigned char r, g, b;
} pixel;
#pragma pack(pop)

typedef struct
{
    double doubleCoeff;
    double bias;
    double **matrix;
    int size;
} filter;

void pprint_filter(filter *filter);

filter *filter_init(int size, double bias, double doubleCoeff, const double matrix[size][size]);

filter *init_motion_from_top_left();

filter *init_blur_soft();

filter *init_blur();

filter *init_gauss_smal_blur();

filter *init_gauss_big_blur();

filter *init_sharpen();

filter *init_id();

filter *init_shift_left();

filter *init_shift_right();

filter *init_negative_filter();

void filter_free(filter *f);

void pprint_filter(filter *filter);

filter *filter_composition(filter *f1, filter *f2);

filter *append_filter_matrix_with_zeros(int appendix, filter *f);

filter *generate_random_filter(int size);
