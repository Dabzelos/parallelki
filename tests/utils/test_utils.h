#include "../filters/filter.h"

#include <stdbool.h>

#define MAX_APPEND 3
#define MIN_IMAGE_SIZE 300
#define MAX_IMAGE_SIZE 1000
#define STEP_SIZE 100

bool compare_images(const unsigned char *img1, const unsigned char *img2, int width, int height,
                    int channels);

double get_time_in_seconds();

void measure_performance(unsigned char *data, int w, int h, int threads, filter filter);

unsigned char *generate_random_image(int w, int h);

void test_filter_composition(filter *f1, filter *f2, const char *test_name);
