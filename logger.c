#include <stdio.h>
#include <pthread.h>
#include "logger.h"

#define BUFFER_SIZE 5
#define TOTAL_ITEMS 10

/* Shared circular buffer */
static int buffer[BUFFER_SIZE];
static int front = 0;
static int rear = 0;
static int count = 0;
static int stop = 0;

/* Synchronization */
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
static pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

static FILE *log_file;

/* Print a message to the terminal and log file */
static void log_event(const char *message)
{
    printf("%s\n", message);
    fprintf(log_file, "%s\n", message);
    fflush(log_file);
}

/* Display the current buffer contents in FIFO order */
static void show_buffer(void)
{
    printf("Buffer: [");
    fprintf(log_file, "Buffer: [");

    for (int i = 0; i < count; i++) {
        int index = (front + i) % BUFFER_SIZE;

        printf("%d", buffer[index]);
        fprintf(log_file, "%d", buffer[index]);

        if (i < count - 1) {
            printf(", ");
            fprintf(log_file, ", ");
        }
    }

    printf("] (%d/%d occupied)\n", count, BUFFER_SIZE);
    fprintf(log_file, "] (%d/%d occupied)\n", count, BUFFER_SIZE);

    fflush(log_file);
}

/* Producer thread */
static void *producer(void *arg)
{
    (void)arg;

    for (int item = 1; item <= TOTAL_ITEMS; item++) {
        pthread_mutex_lock(&mutex);

        while (count == BUFFER_SIZE && !stop) {
            log_event("Producer waiting: buffer FULL. Waiting for consumer...");
            pthread_cond_wait(&not_full, &mutex);
        }

        if (stop) {
            pthread_mutex_unlock(&mutex);
            return NULL;
        }

        buffer[rear] = item;
        rear = (rear + 1) % BUFFER_SIZE;
        count++;

        char message[100];
        snprintf(message, sizeof(message),
                 "Producer produced item: %d", item);
        log_event(message);

        show_buffer();

        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

/* Consumer thread */
static void *consumer(void *arg)
{
    (void)arg;

    for (int i = 0; i < TOTAL_ITEMS; i++) {
        pthread_mutex_lock(&mutex);

        while (count == 0 && !stop) {
            log_event("Consumer waiting: buffer EMPTY. Waiting for producer...");
            pthread_cond_wait(&not_empty, &mutex);
        }

        if (stop && count == 0) {
            pthread_mutex_unlock(&mutex);
            return NULL;
        }

        int item = buffer[front];
        front = (front + 1) % BUFFER_SIZE;
        count--;

        char message[100];
        snprintf(message, sizeof(message),
                 "Consumer consumed item: %d", item);
        log_event(message);

        show_buffer();

        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

/* Run producer-consumer demonstration */
void run_logger_demo(void)
{
    pthread_t producer_thread;
    pthread_t consumer_thread;

    log_file = fopen("logs/system.log", "a");

    if (log_file == NULL) {
        perror("Unable to open log file");
        return;
    }

    pthread_mutex_lock(&mutex);

    front = 0;
    rear = 0;
    count = 0;
    stop = 0;

    pthread_mutex_unlock(&mutex);

    printf("\n========== PRODUCER-CONSUMER ==========\n");
    log_event("Initial buffer state:");
    show_buffer();

    /* Start consumer first */
    if (pthread_create(&consumer_thread, NULL, consumer, NULL) != 0) {
        log_event("Error creating consumer thread.");
        fclose(log_file);
        return;
    }

    /* Start producer */
    if (pthread_create(&producer_thread, NULL, producer, NULL) != 0) {
        pthread_mutex_lock(&mutex);

        stop = 1;
        pthread_cond_broadcast(&not_empty);

        pthread_mutex_unlock(&mutex);

        pthread_join(consumer_thread, NULL);

        log_event("Error creating producer thread.");
        fclose(log_file);
        return;
    }

    /* Wait for both threads */
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    log_event("Producer and Consumer completed successfully.");

    fclose(log_file);
    printf("Log saved in logs/system.log\n");
    printf("=======================================\n");
}
