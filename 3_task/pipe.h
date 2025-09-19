typedef struct {
    char *compute_mode;
    char *filter_type;
    int threads_num;
    int block_size;
    char **input_files;
    int file_count;
} PipelineArgs;

void run_pipeline(PipelineArgs *args);
