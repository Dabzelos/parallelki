typedef struct
{
    char r;
    char g;
    char b;
} pixel_struct;

void seq_convolution(unsigned char *pixel_array, int w, int h, struct filter filter);
