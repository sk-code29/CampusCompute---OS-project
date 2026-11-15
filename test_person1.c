
#include <stdio.h>
#include "scheduler.h"

int main(void)
{
    Job jobs[] = {
        {101, 0, 5, 2, 5, 0, 0, 0},
        {102, 1, 3, 1, 3, 0, 0, 0},
        {103, 2, 8, 3, 8, 0, 0, 0},
        {104, 3, 2, 2, 2, 0, 0, 0}
    };

    int n = sizeof(jobs) / sizeof(jobs[0]);

    printf("Testing Job Validation...\n");

    if (validate_jobs(jobs, n)) {
        printf("All jobs are valid!\n");
    }

    run_fcfs(jobs, n);
    run_round_robin(jobs, n, 2);
    run_priority(jobs, n);
    compare_schedulers(jobs, n);

    return 0;
}
