#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdio.h>
#include "../libs/stb_image.h"
#include "../libs/stb_image_write.h"
#include "../filters/filter.h"
#include "math.h"

typedef struct
{
    char r;
    char g;
    char b;
} pixel_struct;

void inverse_image(unsigned char *pixel_array, size_t pixel_count)
{
    for (size_t i = 0; i < pixel_count; i++)
    {
        pixel_struct *pixel = (pixel_struct *)(&pixel_array[i * 3]);
        pixel->r = 255 - pixel->r;
        pixel->g = 255 - pixel->g;
        pixel->b = 255 - pixel->b;
    }
}

void convolution(unsigned char *pixel_array, int w, int h, struct filter filter)
{
    struct pixel
    {
        unsigned char r;
        unsigned char g;
        unsigned char b;
    };

    struct pixel *result = malloc(w * h * 3);
    struct pixel *image = (struct pixel *)pixel_array;

    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            double red = 0.0, green = 0.0, blue = 0.0;

            for (int filterY = 0; filterY < filter.size; filterY++)
            {
                for (int filterX = 0; filterX < filter.size; filterX++)
                {
                    int imageX = (x - filter.size / 2 + filterX + w) % w;
                    int imageY = (y - filter.size / 2 + filterY + h) % h;

                    red += image[imageY * w + imageX].r * filter.matrix[filterY][filterX];
                    green += image[imageY * w + imageX].g * filter.matrix[filterY][filterX];
                    blue += image[imageY * w + imageX].b * filter.matrix[filterY][filterX];
                }
            }

            result[(y * w + x)].r = fmin(fmax((int)(filter.doubleCoeff * red + filter.bias), 0), 255);
            result[(y * w + x)].g = fmin(fmax((int)(filter.doubleCoeff * green + filter.bias), 0), 255);
            result[(y * w + x)].b = fmin(fmax((int)(filter.doubleCoeff * blue + filter.bias), 0), 255);
        }
    }

    memcpy(pixel_array, result, w * h * 3);
    free(result);
}

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

    struct filter *filter = init_motion_from_top_left();

    convolution(data, w, h, *filter);

    // stbi_write_png("../images/result.png", w, h, 3, data, w * 3);
    stbi_write_bmp("/home/dabzelos/Desktop/paralelki/images/result.bmp", w, h, 3, data);
}