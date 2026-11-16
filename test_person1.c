#include <stdio.h>
#include "scheduler.h"

int main(void)
{
    Job jobs[MAX_JOBS];
    int n, quantum;

    printf("\n========== CAMPUSCOMPUTE SCHEDULER ==========\n");

    printf("Enter number of jobs (1-%d): ", MAX_JOBS);

    if (scanf("%d", &n) != 1 || n < 1 || n > MAX_JOBS) {
        printf("Error: Invalid number of jobs.\n");
        return 1;
    }

    for (int i = 0; i < n; i++) {
        printf("\nEnter details for Job %d\n", i + 1);

        printf("Job ID: ");
        if (scanf("%d", &jobs[i].job_id) != 1) {
            printf("Error: Invalid Job ID.\n");
            return 1;
        }

        printf("Arrival Time: ");
        if (scanf("%d", &jobs[i].arrival_time) != 1) {
            printf("Error: Invalid arrival time.\n");
            return 1;
        }

        printf("Burst Time: ");
        if (scanf("%d", &jobs[i].burst_time) != 1) {
            printf("Error: Invalid burst time.\n");
            return 1;
        }

        printf("Priority (smaller number = higher priority): ");
        if (scanf("%d", &jobs[i].priority) != 1) {
            printf("Error: Invalid priority.\n");
            return 1;
        }
    }

    if (!validate_jobs(jobs, n))
        return 1;

    printf("\nEnter Round Robin time quantum: ");

    if (scanf("%d", &quantum) != 1 || quantum <= 0) {
        printf("Error: Time quantum must be a positive integer.\n");
        return 1;
    }

    compare_schedulers(jobs, n, quantum);

    printf("\nAll scheduling algorithms executed successfully.\n");

    return 0;
}
