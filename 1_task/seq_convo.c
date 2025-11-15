#include "../filters/filter.h"
#include "math.h"
#include "seq.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void seq_convolution(unsigned char *pixel_array, int w, int h, filter filter) {
    pixel *result = malloc(w * h * sizeof(pixel));

    pixel *image = (pixel *)pixel_array;

    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            double red = 0.0, green = 0.0, blue = 0.0;

            for (int filterY = 0; filterY < filter.size; filterY++) {
                for (int filterX = 0; filterX < filter.size; filterX++) {
                    int imageX = (x - filter.size / 2 + filterX + w) % w;
                    int imageY = (y - filter.size / 2 + filterY + h) % h;

                    red += image[imageY * w + imageX].r * filter.matrix[filterY][filterX];
                    green += image[imageY * w + imageX].g * filter.matrix[filterY][filterX];
                    blue += image[imageY * w + imageX].b * filter.matrix[filterY][filterX];
                }
            }

            result[(y * w + x)].r =
                fmin(fmax((int)round(filter.doubleCoeff * red + filter.bias), 0), 255);
            result[(y * w + x)].g =
                fmin(fmax((int)round(filter.doubleCoeff * green + filter.bias), 0), 255);
            result[(y * w + x)].b =
                fmin(fmax((int)round(filter.doubleCoeff * blue + filter.bias), 0), 255);
        }
    }

    memcpy(pixel_array, result, w * h * 3);
    free(result);
}
