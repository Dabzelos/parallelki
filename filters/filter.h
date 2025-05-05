#pragma once

struct filter
{
    double doubleCoeff;
    double bias;
    double **matrix;
    int size;
};

void pprint_filter(struct filter *filter);

struct filter *filter_init(int size, double bias, double doubleCoeff, const double matrix[size][size]);

struct filter *init_motion_from_top_left();

struct filter *init_blur_soft();

struct filter *init_blur();

struct filter *init_gauss_smal_blur();

struct filter *init_gauss_big_blur();

struct filter *init_sharpen();

void filter_free(struct filter *f);

void pprint_filter(struct filter *filter);