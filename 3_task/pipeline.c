#include "../1_task/seq.h"
#include "../2_task/mt.h"
#include "../filters/filter.h"
#include "../libs/stb_image.h"
#include "../libs/stb_image_write.h"
#include "../tests/utils/test_utils.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_THREADS 16

// оузел очеред
typedef struct Node {
    void *data;
    struct Node *next;
} Node;

// очеред
typedef struct {
    Node *head, *tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool closed;
} Queue;

void queue_init(Queue *q) {
    q->head = q->tail = NULL;
    q->closed = false;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
}

void queue_push(Queue *q, void *data) {
    Node *n = malloc(sizeof(Node));
    n->data = data;
    n->next = NULL;
    pthread_mutex_lock(&q->mutex);
    if (q->tail)
        q->tail->next = n;
    else
        q->head = n;
    q->tail = n;
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

void *queue_pop(Queue *q) {
    pthread_mutex_lock(&q->mutex);
    while (!q->head && !q->closed) {
        pthread_cond_wait(&q->cond, &q->mutex);
    }
    if (!q->head) {
        pthread_mutex_unlock(&q->mutex);
        return NULL;
    }
    Node *n = q->head;
    q->head = n->next;
    if (!q->head) q->tail = NULL;
    pthread_mutex_unlock(&q->mutex);

    void *data = n->data;
    free(n);
    return data;
}

void queue_close(Queue *q) {
    pthread_mutex_lock(&q->mutex);
    q->closed = true;
    pthread_cond_broadcast(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

typedef struct {
    char *path;
    unsigned char *data;
    int w, h;
} ImageTask;

typedef struct {
    char *compute_mode;
    char *filter_type;
    int threads_num;
    int block_size;
    char **input_files;
    int file_count;
} PipelineArgs;

Queue to_process;
Queue to_write;
filter *global_filter;

void *worker_thread(void *arg) {
    PipelineArgs *pargs = (PipelineArgs *)arg;
    while (true) {
        ImageTask *task = queue_pop(&to_process);
        if (!task) break;
        if (strcmp(pargs->compute_mode, "seq") == 0) {
            seq_convolution(task->data, task->w, task->h, *global_filter);
        } else {
            mt_convolution(task->data, task->w, task->h, *global_filter, pargs->threads_num,
                           MODE_ROW, pargs->block_size);
        }
        queue_push(&to_write, task);
    }
    return NULL;
}

void *writer_thread(void *arg) {
    (void)arg;
    while (true) {
        ImageTask *task = queue_pop(&to_write);
        if (!task) break;
        char outname[256];
        snprintf(outname, sizeof(outname), "out_%s", task->path);
        stbi_write_png(outname, task->w, task->h, 3, task->data, task->w * 3);
        printf("Saved %s\n", outname);
        stbi_image_free(task->data);
        free(task->path);
        free(task);
    }
    return NULL;
}

void run_pipeline(PipelineArgs *args) {
    queue_init(&to_process);
    queue_init(&to_write);

    global_filter = init_selected_filter(args->filter_type);

    pthread_t writer;
    pthread_create(&writer, NULL, writer_thread, NULL);

    pthread_t workers[MAX_THREADS];
    for (int i = 0; i < args->threads_num; i++) {
        pthread_create(&workers[i], NULL, worker_thread, args);
    }

    for (int i = 0; i < args->file_count; i++) {
        int w, h, n;
        unsigned char *data = stbi_load(args->input_files[i], &w, &h, &n, 3);
        if (!data) {
            fprintf(stderr, "Error: failed to load %s\n", args->input_files[i]);
            continue;
        }
        ImageTask *task = malloc(sizeof(ImageTask));
        task->path = strdup(args->input_files[i]);
        task->data = data;
        task->w = w;
        task->h = h;
        queue_push(&to_process, task);
    }

    queue_close(&to_process);
    for (int i = 0; i < args->threads_num; i++) pthread_join(workers[i], NULL);
    queue_close(&to_write);
    pthread_join(writer, NULL);

    filter_free(global_filter);
}
