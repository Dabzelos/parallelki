#include <stdbool.h>
#include "../filters/filter.h"

bool compare_images(const unsigned char *img1, const unsigned char *img2, int width, int height, int channels);

double get_time_in_seconds();

void measure_performance(unsigned char *data, int w, int h, int threads, filter filter);