
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#include "ipc.h"

#define SHM_NAME "/campuscompute_shm"
#define READY_SEM "/campuscompute_ready"
#define DONE_SEM "/campuscompute_done"

#define MESSAGE_SIZE 256

typedef struct {
    char message[MESSAGE_SIZE];
    char response[MESSAGE_SIZE];
} SharedData;

/* Handle interrupted semaphore waits */
static int wait_semaphore(sem_t *sem)
{
    while (sem_wait(sem) == -1) {
        if (errno != EINTR)
            return -1;
    }
    return 0;
}

/* Wait for a semaphore with a timeout */
static int timed_wait_semaphore(sem_t *sem)
{
    struct timespec deadline;

    if (clock_gettime(CLOCK_REALTIME, &deadline) == -1)
        return -1;

    deadline.tv_sec += 5;

    while (sem_timedwait(sem, &deadline) == -1) {
        if (errno != EINTR)
            return -1;
    }

    return 0;
}

/* Reap a child process safely */
static int reap_child(pid_t pid, int *status)
{
    pid_t result;

    do {
        result = waitpid(pid, status, 0);
    } while (result == -1 && errno == EINTR);

    return result == pid ? 0 : -1;
}

void run_ipc_demo(void)
{
    int shm_fd = -1;
    SharedData *shared = MAP_FAILED;

    sem_t *ready = SEM_FAILED;
    sem_t *done = SEM_FAILED;

    int shm_created = 0;
    int ready_created = 0;
    int done_created = 0;

    int child_status = 0;
    int child_reaped = 0;
    pid_t pid = -1;

    printf("\n========== SHARED MEMORY IPC ==========\n");

    /* Create shared memory without overwriting existing objects */
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, 0600);

    if (shm_fd == -1) {
        perror("shm_open");
        goto cleanup;
    }

    shm_created = 1;

    if (ftruncate(shm_fd, sizeof(SharedData)) == -1) {
        perror("ftruncate");
        goto cleanup;
    }

    shared = mmap(NULL, sizeof(SharedData),
                  PROT_READ | PROT_WRITE,
                  MAP_SHARED, shm_fd, 0);

    if (shared == MAP_FAILED) {
        perror("mmap");
        goto cleanup;
    }

    memset(shared, 0, sizeof(SharedData));

    /* Create named semaphores */
    ready = sem_open(READY_SEM, O_CREAT | O_EXCL, 0600, 0);

    if (ready == SEM_FAILED) {
        perror("sem_open ready");
        goto cleanup;
    }

    ready_created = 1;

    done = sem_open(DONE_SEM, O_CREAT | O_EXCL, 0600, 0);

    if (done == SEM_FAILED) {
        perror("sem_open done");
        goto cleanup;
    }

    done_created = 1;

    /* Create cooperating child process */
    pid = fork();

    if (pid == -1) {
        perror("fork");
        goto cleanup;
    }

    if (pid == 0) {

        /* Child waits until parent sends data */
        if (wait_semaphore(ready) == -1) {
            perror("Child sem_wait");
            _exit(EXIT_FAILURE);
        }

        printf("Child received: %s\n", shared->message);

        snprintf(shared->response, MESSAGE_SIZE,
                 "Message successfully received by child.");

        if (sem_post(done) == -1) {
            perror("Child sem_post");
            _exit(EXIT_FAILURE);
        }

        fflush(stdout);
        _exit(EXIT_SUCCESS);
    }

    /* Parent sends message */
    snprintf(shared->message, MESSAGE_SIZE,
             "Hello from the parent process!");

    printf("Parent sent: %s\n", shared->message);

    if (sem_post(ready) == -1) {
        perror("Parent sem_post");
        goto stop_child;
    }

    /* Parent waits for child response */
    if (timed_wait_semaphore(done) == -1) {
        perror("Parent timed semaphore wait");
        goto stop_child;
    }

    printf("Parent received: %s\n", shared->response);

    if (reap_child(pid, &child_status) == -1) {
        perror("waitpid");
        goto cleanup;
    }

    child_reaped = 1;

    if (WIFEXITED(child_status) &&
        WEXITSTATUS(child_status) == EXIT_SUCCESS) {
        printf("IPC communication completed successfully.\n");
    } else {
        printf("Child process did not complete successfully.\n");
    }

    goto cleanup;

stop_child:

    /* Prevent a blocked child from remaining alive */
    kill(pid, SIGTERM);

    if (reap_child(pid, &child_status) == 0)
        child_reaped = 1;

cleanup:

    if (pid > 0 && !child_reaped) {
        kill(pid, SIGTERM);

        if (reap_child(pid, &child_status) == 0)
            child_reaped = 1;
    }

    if (ready != SEM_FAILED)
        sem_close(ready);

    if (done != SEM_FAILED)
        sem_close(done);

    if (shared != MAP_FAILED)
        munmap(shared, sizeof(SharedData));

    if (shm_fd != -1)
        close(shm_fd);

    /* Unlink only resources created by this execution */
    if (ready_created)
        sem_unlink(READY_SEM);

    if (done_created)
        sem_unlink(DONE_SEM);

    if (shm_created)
        shm_unlink(SHM_NAME);

    printf("IPC resources created by this execution cleaned up.\n");
    printf("=======================================\n");
}
