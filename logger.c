
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "logger.h"

#define BUFFER_SIZE 5
#define TOTAL_ITEMS 10

/* Shared bounded buffer */
int buffer[BUFFER_SIZE];

int front = 0;
int rear = 0;
int count = 0;

/* Synchronization variables */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

FILE *log_file;

/* Producer thread */
void *producer(void *arg)
{
    (void)arg;

    for (int item = 1; item <= TOTAL_ITEMS; item++) {

        pthread_mutex_lock(&mutex);

        /* Wait if buffer is full */
        while (count == BUFFER_SIZE) {
            printf("Producer waiting: Buffer is full.\n");
            pthread_cond_wait(&not_full, &mutex);
        }

        buffer[rear] = item;

        rear = (rear + 1) % BUFFER_SIZE;
        count++;

        printf("Producer produced item: %d\n", item);

        fprintf(log_file, "Produced item: %d\n", item);
        fflush(log_file);

        /* Notify consumer */
        pthread_cond_signal(&not_empty);

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

/* Consumer thread */
void *consumer(void *arg)
{
    (void)arg;

    for (int i = 1; i <= TOTAL_ITEMS; i++) {

        pthread_mutex_lock(&mutex);

        /* Wait if buffer is empty */
        while (count == 0) {
            printf("Consumer waiting: Buffer is empty.\n");
            pthread_cond_wait(&not_empty, &mutex);
        }

        int item = buffer[front];

        front = (front + 1) % BUFFER_SIZE;
        count--;

        printf("Consumer consumed item: %d\n", item);

        fprintf(log_file, "Consumed item: %d\n", item);
        fflush(log_file);

        /* Notify producer */
        pthread_cond_signal(&not_full);

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

/* Start producer-consumer demonstration */
void run_logger_demo(void)
{
    pthread_t producer_thread;
    pthread_t consumer_thread;

    log_file = fopen("logs/system.log", "a");

    if (log_file == NULL) {
        perror("Unable to open log file");
        return;
    }

    front = 0;
    rear = 0;
    count = 0;

    printf("\n========== PRODUCER-CONSUMER ==========\n");

    /* Create threads */
    if (pthread_create(&producer_thread, NULL, producer, NULL) != 0) {
        printf("Error creating producer thread.\n");
        fclose(log_file);
        return;
    }

    if (pthread_create(&consumer_thread, NULL, consumer, NULL) != 0) {
        printf("Error creating consumer thread.\n");

        pthread_mutex_lock(&mutex);
        pthread_cond_broadcast(&not_full);
        pthread_mutex_unlock(&mutex);

        pthread_join(producer_thread, NULL);
        fclose(log_file);
        return;
    }

    /* Wait for both threads to finish */
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    printf("\nProducer and Consumer completed successfully.\n");

    fclose(log_file);

    printf("Log saved in logs/system.log\n");
    printf("=======================================\n");
}
