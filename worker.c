
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "worker.h"

/* Run one child process and inspect its exit status */
static void run_child(const char *name, int simulate_failure)
{
    pid_t pid;
    int status;

    printf("\nStarting %s...\n", name);

    pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        /* Child process */
        printf("%s: Worker process started. PID = %d\n",
               name, getpid());

        if (simulate_failure) {
            printf("%s: Simulating a worker failure.\n", name);
            fflush(stdout);
            exit(EXIT_FAILURE);
        }

        printf("%s: Task completed successfully.\n", name);
        fflush(stdout);
        exit(EXIT_SUCCESS);
    }

    /* Parent process */
    printf("Parent: Waiting for %s (PID = %d)...\n", name, pid);

    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return;
    }

    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);

        if (exit_code == EXIT_SUCCESS) {
            printf("Parent: %s completed normally.\n", name);
        } else {
            printf("Parent: %s failed with exit code %d.\n",
                   name, exit_code);
        }
    } else if (WIFSIGNALED(status)) {
        printf("Parent: %s terminated by signal %d.\n",
               name, WTERMSIG(status));
    }
}

void run_worker_demo(void)
{
    printf("\n========== WORKER PROCESS MANAGEMENT ==========\n");

    /* Case 1: Normal completion */
    run_child("Worker 1", 0);

    /* Case 2: Controlled simulated failure */
    run_child("Worker 2", 1);

    printf("\nWorker process management demonstration completed.\n");
    printf("================================================\n");
}
