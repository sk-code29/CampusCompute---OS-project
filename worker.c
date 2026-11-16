#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include "worker.h"

#define MAX_WORKERS 10

/* Create one child process and check its exit status */
static void run_child(int worker_no, int simulate_failure)
{
    pid_t pid;
    int status;

    printf("\nStarting Worker %d...\n", worker_no);
    fflush(NULL);

    pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        /* Child process */
        printf("Worker %d started. PID = %ld\n",
               worker_no, (long)getpid());

        if (simulate_failure) {
            printf("Worker %d: Simulating failure.\n", worker_no);
            fflush(stdout);
            _exit(EXIT_FAILURE);
        }

        printf("Worker %d: Task completed successfully.\n", worker_no);
        fflush(stdout);
        _exit(EXIT_SUCCESS);
    }

    /* Parent process */
    printf("Parent: Waiting for Worker %d (PID = %ld)...\n",
           worker_no, (long)pid);

    /* Wait until the child finishes */
    while (waitpid(pid, &status, 0) == -1) {
        if (errno == EINTR)
            continue;

        perror("waitpid");
        return;
    }

    /* Check how the child terminated */
    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);

        if (exit_code == EXIT_SUCCESS)
            printf("Parent: Worker %d completed normally.\n", worker_no);
        else
            printf("Parent: Worker %d failed with exit code %d.\n",
                   worker_no, exit_code);
    }
    else if (WIFSIGNALED(status)) {
        printf("Parent: Worker %d terminated by signal %d.\n",
               worker_no, WTERMSIG(status));
    }
}

void run_worker_demo(void)
{
    int num_workers;
    char choice;

    printf("\n========== WORKER PROCESS MANAGEMENT ==========\n");

    printf("Enter number of workers (1-%d): ", MAX_WORKERS);

    if (scanf("%d", &num_workers) != 1 ||
        num_workers < 1 || num_workers > MAX_WORKERS) {
        printf("Invalid number of workers.\n");
        return;
    }

    for (int i = 1; i <= num_workers; i++) {
        printf("\nShould Worker %d simulate failure? (y/n): ", i);

        if (scanf(" %c", &choice) != 1) {
            printf("Invalid input. Stopping worker demonstration.\n");
            return;
        }

        if (choice != 'y' && choice != 'Y' &&
            choice != 'n' && choice != 'N') {
            printf("Please enter y or n.\n");
            i--;
            continue;
        }

        run_child(i, choice == 'y' || choice == 'Y');
    }

    printf("\nWorker process management demonstration completed.\n");
    printf("================================================\n");
}
