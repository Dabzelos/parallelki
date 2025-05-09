#include "../1_task/seq.h"
#include "../filters/filter.h"
#include "../tests/utils/test_utils.h"

void test_filter_composition_apply()
{
    int w, h, n;

    unsigned char *data1 = stbi_load("/home/dabzelos/Desktop/paralelki/images/mypersonalphoto.bmp", &w, &h, &n, 3);
    unsigned char *data2 = stbi_load("/home/dabzelos/Desktop/paralelki/images/mypersonalphoto.bmp", &w, &h, &n, 3);

    filter *filter_1 = init_blur();
    filter *filter_2 = init_blur();
    filter *filter_c = filter_composition(filter_1, filter_2);

    seq_convolution(data1, w, h, filter_1);
    seq_convolution(data1, w, h, filter_2);

    seq_convolution(data1, w, h, filter_c);

    if (!compare_images(data1, data2, w, h, 3))
    {
        printf("Test_Filter_Composition_Apply Failed\n On these filters:\n");

        pprint_filter(filter_1);
        pprint_filter(filter_2);
        pprint_filter(filter_c);
    }
    else
    {
        printf("Test_Filter_Composition_Apply Failed\n On these filters:\n");
    }

    filter_free(filter_1);
    filter_free(filter_2);
    filter_free(filter_c);
}

int main() {}