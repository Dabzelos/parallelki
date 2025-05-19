#include "stdlib.h"
#include "stdbool.h"
#include "stdio.h"

#include <time.h>
#include "math.h"
#include <string.h>

#include "../1_task/seq.h"
#include "../2_task/mt.h"
#include "../tests/utils/test_utils.h"
#include "../tests/mt_test/mt_test.h"
#include "../tests/seq_test/seq_test.h"

int main()
{
    test_random_filters_composition();

    /* tests for filters that results are known*/
    test_id_filter();
    test_shift_filter_left();

    /*test for filters which composition gives id*/
    test_negative_filter_composition_gives_id();
    test_opposite_shift_filters_composition_gives_id();
    test_id_filters_composition_gives_id();

    /* test for matrixes appended with zeros */
    test_predefined_filters_appended_with_zeros();
    test_random_filter_appended_with_zeros();

    /*mt tests*/
    test_mt_convolution(MODE_ROW, "MultiThread by_row convolution");
    test_mt_convolution(MODE_PIXEL, "MultiThread by_pixel convolution");
    test_mt_convolution(MODE_COLUMN, "MultiThread by_collumn convolution");
    test_mt_convolution(MODE_BLOCK, "MultiThread by_block convolution");
}