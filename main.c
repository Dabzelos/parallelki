#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdio.h>
#include "math.h"

#include "../libs/stb_image.h"
#include "../libs/stb_image_write.h"

#include "../filters/filter.h"

#include "../1_task/seq.h"

int main()
{
    int w, h, n;

    unsigned char *data = stbi_load("/home/dabzelos/Desktop/paralelki/images/mypersonalphoto.bmp", &w, &h, &n, 3);
    printf("%d %d %d\n", w, h, n);

    if (data == NULL)
    {
        printf("naaah we trippin");

        return 0;
    }

    struct filter *filter = init_sharpen();

    seq_convolution(data, w, h, *filter);

    filter_free(filter);

    // stbi_write_png("../images/result.png", w, h, 3, data, w * 3);
    stbi_write_bmp("/home/dabzelos/Desktop/paralelki/images/result.bmp", w, h, 3, data);
    free(data);
}