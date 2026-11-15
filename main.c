
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "scheduler.h"
#include "logger.h"
#include "ipc.h"
#include "banker.h"
#include "worker.h"
#include "resource.h"

/* Read an integer safely from the terminal */
static int read_integer(const char *prompt, int *value)
{
    char input[100];
    char *end;
    long number;

    printf("%s", prompt);

    if (fgets(input, sizeof(input), stdin) == NULL) {
        return 0;
    }

    errno = 0;
    number = strtol(input, &end, 10);

    if (end == input || errno == ERANGE) {
        return 0;
    }

    while (*end == ' ' || *end == '\t') {
        end++;
    }

    if (*end != '\n' && *end != '\0') {
        return 0;
    }

    if (number < -2147483647L - 1 || number > 2147483647L) {
        return 0;
    }

    *value = (int)number;
    return 1;
}

/* Sample jobs used for scheduling demonstrations */
static void load_sample_jobs(Job jobs[])
{
    jobs[0] = (Job){101, 0, 5, 2, 5, 0, 0, 0};
    jobs[1] = (Job){102, 1, 3, 1, 3, 0, 0, 0};
    jobs[2] = (Job){103, 2, 8, 3, 8, 0, 0, 0};
    jobs[3] = (Job){104, 3, 2, 2, 2, 0, 0, 0};
}

/* Display CLI menu */
static void display_menu(void)
{
    printf("\n");
    printf("============================================\n");
    printf("       CAMPUSCOMPUTE RESOURCE MANAGER       \n");
    printf("============================================\n");
    printf("1.  FCFS Scheduling\n");
    printf("2.  Round Robin Scheduling\n");
    printf("3.  Priority Scheduling\n");
    printf("4.  Compare Scheduling Algorithms\n");
    printf("5.  Concurrent Logger Demonstration\n");
    printf("6.  Shared Memory IPC Demonstration\n");
    printf("7.  Banker's Algorithm - Safe State\n");
    printf("8.  Banker's Algorithm - Unsafe State\n");
    printf("9.  Worker Process Demonstration\n");
    printf("10. Resource Limit Demonstration\n");
    printf("0.  Exit\n");
    printf("============================================\n");
}

int main(void)
{
    int choice;
    int quantum;
    Job jobs[4];

    printf("\nWelcome to CampusCompute!\n");
    printf("Intelligent Job Scheduling and Resource Management\n");

    while (1) {
        display_menu();

        if (!read_integer("Enter your choice: ", &choice)) {
            printf("Invalid input. Please enter a valid integer.\n");
            continue;
        }

        load_sample_jobs(jobs);

        switch (choice) {

            case 1:
                printf("\nRunning FCFS Scheduler...\n");
                run_fcfs(jobs, 4);
                break;

            case 2:
                if (!read_integer("Enter time quantum: ", &quantum)) {
                    printf("Invalid quantum. Please enter an integer.\n");
                    break;
                }

                if (quantum <= 0) {
                    printf("Error: Time quantum must be positive.\n");
                    break;
                }

                printf("\nRunning Round Robin Scheduler...\n");
                run_round_robin(jobs, 4, quantum);
                break;

            case 3:
                printf("\nRunning Priority Scheduler...\n");
                run_priority(jobs, 4);
                break;

            case 4:
                printf("\nComparing Scheduling Algorithms...\n");
                compare_schedulers(jobs, 4);
                break;

            case 5:
                printf("\nStarting Concurrent Logger...\n");
                run_logger_demo();
                break;

            case 6:
                printf("\nStarting Shared Memory IPC...\n");
                run_ipc_demo();
                break;

            case 7:
                printf("\nChecking Banker's Safe State...\n");
                run_banker_safe();
                break;

            case 8:
                printf("\nChecking Banker's Unsafe State...\n");
                run_banker_unsafe();
                break;

            case 9:
                printf("\nStarting Worker Process...\n");
                run_worker_demo();
                break;

            case 10:
                printf("\nStarting Resource Limit Demonstration...\n");
                run_resource_demo();
                break;

            case 0:
                printf("\nExiting CampusCompute. Goodbye!\n");
                return 0;

            default:
                printf("Invalid choice. Select a number from 0 to 10.\n");
        }
    }

    return 0;
}
