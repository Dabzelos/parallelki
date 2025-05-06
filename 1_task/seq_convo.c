#include <stdio.h>
#include "../filters/filter.h"
#include "math.h"
#include <string.h>
#include <stdlib.h>
#include "seq.h"

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

void seq_convolution(unsigned char *pixel_array, int w, int h, struct filter filter)
{
#pragma pack(1)
    struct pixel
    {
        unsigned char r, g, b;
    };
#pragma pack()

    struct pixel *result = malloc(w * h * sizeof(struct pixel));

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

            result[(y * w + x)].r = fmin(fmax((int)round(filter.doubleCoeff * red + filter.bias), 0), 255);
            result[(y * w + x)].g = fmin(fmax((int)round(filter.doubleCoeff * green + filter.bias), 0), 255);
            result[(y * w + x)].b = fmin(fmax((int)round(filter.doubleCoeff * blue + filter.bias), 0), 255);
        }
    }

    memcpy(pixel_array, result, w * h * 3);
    free(result);
}
