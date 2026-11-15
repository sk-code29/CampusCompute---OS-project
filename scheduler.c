
#include <stdio.h>
#include "scheduler.h"

/* ==========================================
   1. JOB VALIDATION
   ========================================== */

int validate_jobs(const Job jobs[], int n)
{
    if (jobs == NULL || n <= 0 || n > MAX_JOBS) {
        printf("Error: Invalid number of jobs.\n");
        return 0;
    }

    for (int i = 0; i < n; i++) {

        if (jobs[i].job_id <= 0) {
            printf("Error: Job ID must be positive.\n");
            return 0;
        }

        if (jobs[i].arrival_time < 0) {
            printf("Error: Arrival time cannot be negative.\n");
            return 0;
        }

        if (jobs[i].burst_time <= 0) {
            printf("Error: Burst time must be positive.\n");
            return 0;
        }

        if (jobs[i].priority < 0) {
            printf("Error: Priority cannot be negative.\n");
            return 0;
        }

        for (int j = i + 1; j < n; j++) {
            if (jobs[i].job_id == jobs[j].job_id) {
                printf("Error: Duplicate Job ID found.\n");
                return 0;
            }
        }
    }

    return 1;
}


/* ==========================================
   2. FIRST COME FIRST SERVE (FCFS)
   ========================================== */

void run_fcfs(Job jobs[], int n)
{
    if (!validate_jobs(jobs, n)) {
        return;
    }

    Job sorted[MAX_JOBS];

    for (int i = 0; i < n; i++) {
        sorted[i] = jobs[i];
    }

    /* Sort by arrival time, then Job ID */
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {

            if (sorted[i].arrival_time > sorted[j].arrival_time ||
               (sorted[i].arrival_time == sorted[j].arrival_time &&
                sorted[i].job_id > sorted[j].job_id)) {

                Job temp = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = temp;
            }
        }
    }

    int current_time = 0;
    float total_wt = 0;
    float total_tat = 0;

    printf("\n========== FCFS SCHEDULING ==========\n");
    printf("\nTie-breaking: Earlier arrival, then smaller Job ID.\n");
    printf("\nExecution Timeline:\n");

    for (int i = 0; i < n; i++) {

        if (current_time < sorted[i].arrival_time) {

            printf("| IDLE (%d-%d) ",
                   current_time, sorted[i].arrival_time);

            current_time = sorted[i].arrival_time;
        }

        int start_time = current_time;

        current_time += sorted[i].burst_time;

        sorted[i].completion_time = current_time;

        sorted[i].turnaround_time =
            sorted[i].completion_time - sorted[i].arrival_time;

        sorted[i].waiting_time =
            sorted[i].turnaround_time - sorted[i].burst_time;

        sorted[i].remaining_time = 0;

        total_wt += sorted[i].waiting_time;
        total_tat += sorted[i].turnaround_time;

        printf("| J%d (%d-%d) ",
               sorted[i].job_id, start_time, current_time);
    }

    printf("|\n");

    printf("\n%-8s %-8s %-8s %-8s %-8s %-8s\n",
           "Job", "AT", "BT", "CT", "TAT", "WT");

    for (int i = 0; i < n; i++) {

        printf("%-8d %-8d %-8d %-8d %-8d %-8d\n",
               sorted[i].job_id,
               sorted[i].arrival_time,
               sorted[i].burst_time,
               sorted[i].completion_time,
               sorted[i].turnaround_time,
               sorted[i].waiting_time);
    }

    printf("\nAverage Waiting Time: %.2f\n", total_wt / n);
    printf("Average Turnaround Time: %.2f\n", total_tat / n);

    printf("\n=====================================\n");
}


/* ==========================================
   3. ROUND ROBIN SCHEDULING
   ========================================== */

/* Add a job to the ready queue */
static void enqueue_rr(int queue[], int *rear,
                       int *count, int job_index)
{
    queue[*rear] = job_index;
    *rear = (*rear + 1) % MAX_JOBS;
    (*count)++;
}

/* Remove a job from the ready queue */
static int dequeue_rr(int queue[], int *front, int *count)
{
    int job_index = queue[*front];

    *front = (*front + 1) % MAX_JOBS;
    (*count)--;

    return job_index;
}

void run_round_robin(Job jobs[], int n, int quantum)
{
    if (!validate_jobs(jobs, n)) {
        return;
    }

    if (quantum <= 0) {
        printf("Error: Time quantum must be positive.\n");
        return;
    }

    Job sorted[MAX_JOBS];

    for (int i = 0; i < n; i++) {
        sorted[i] = jobs[i];
        sorted[i].remaining_time = sorted[i].burst_time;
    }

    /* Sort by arrival time, then Job ID */
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {

            if (sorted[i].arrival_time > sorted[j].arrival_time ||
               (sorted[i].arrival_time == sorted[j].arrival_time &&
                sorted[i].job_id > sorted[j].job_id)) {

                Job temp = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = temp;
            }
        }
    }

    int queue[MAX_JOBS];

    int front = 0;
    int rear = 0;
    int count = 0;

    int current_time = 0;
    int next_job = 0;
    int completed = 0;

    float total_wt = 0;
    float total_tat = 0;

    printf("\n========== ROUND ROBIN SCHEDULING ==========\n");
    printf("Time Quantum: %d\n", quantum);
    printf("\nExecution Timeline:\n");

    while (completed < n) {

        /* If ready queue is empty, move to next arrival */
        if (count == 0) {

            if (next_job < n &&
                current_time < sorted[next_job].arrival_time) {

                printf("| IDLE (%d-%d) ",
                       current_time,
                       sorted[next_job].arrival_time);

                current_time = sorted[next_job].arrival_time;
            }

            while (next_job < n &&
                   sorted[next_job].arrival_time <= current_time) {

                enqueue_rr(queue, &rear, &count, next_job);
                next_job++;
            }
        }

        int index = dequeue_rr(queue, &front, &count);

        int start_time = current_time;
        int execution_time;

        if (sorted[index].remaining_time > quantum) {
            execution_time = quantum;
        } else {
            execution_time = sorted[index].remaining_time;
        }

        current_time += execution_time;
        sorted[index].remaining_time -= execution_time;

        printf("| J%d (%d-%d) ",
               sorted[index].job_id,
               start_time,
               current_time);

        /* Add newly arrived jobs to the queue */
        while (next_job < n &&
               sorted[next_job].arrival_time <= current_time) {

            enqueue_rr(queue, &rear, &count, next_job);
            next_job++;
        }

        /* Check whether current job is completed */
        if (sorted[index].remaining_time == 0) {

            sorted[index].completion_time = current_time;

            sorted[index].turnaround_time =
                sorted[index].completion_time -
                sorted[index].arrival_time;

            sorted[index].waiting_time =
                sorted[index].turnaround_time -
                sorted[index].burst_time;

            total_wt += sorted[index].waiting_time;
            total_tat += sorted[index].turnaround_time;

            completed++;

        } else {

            /* Put unfinished job at the end of queue */
            enqueue_rr(queue, &rear, &count, index);
        }
    }

    printf("|\n");

    printf("\n%-8s %-8s %-8s %-8s %-8s %-8s\n",
           "Job", "AT", "BT", "CT", "TAT", "WT");

    for (int i = 0; i < n; i++) {

        printf("%-8d %-8d %-8d %-8d %-8d %-8d\n",
               sorted[i].job_id,
               sorted[i].arrival_time,
               sorted[i].burst_time,
               sorted[i].completion_time,
               sorted[i].turnaround_time,
               sorted[i].waiting_time);
    }

    printf("\nAverage Waiting Time: %.2f\n", total_wt / n);
    printf("Average Turnaround Time: %.2f\n", total_tat / n);

    printf("\n============================================\n");
}


/* ==========================================
   4. NON-PREEMPTIVE PRIORITY SCHEDULING
   ========================================== */

void run_priority(Job jobs[], int n)
{
    if (!validate_jobs(jobs, n)) {
        return;
    }

    Job result[MAX_JOBS];

    int completed_flags[MAX_JOBS] = {0};

    for (int i = 0; i < n; i++) {
        result[i] = jobs[i];
    }

    int current_time = 0;
    int completed = 0;

    float total_wt = 0;
    float total_tat = 0;

    printf("\n========== PRIORITY SCHEDULING ==========\n");
    printf("Smaller priority number = Higher priority\n");
    printf("\nExecution Timeline:\n");

    while (completed < n) {

        int selected = -1;

        /* Find the highest-priority arrived job */
        for (int i = 0; i < n; i++) {

            if (!completed_flags[i] &&
                result[i].arrival_time <= current_time) {

                if (selected == -1 ||
                    result[i].priority < result[selected].priority ||
                    (result[i].priority == result[selected].priority &&
                     result[i].arrival_time < result[selected].arrival_time) ||
                    (result[i].priority == result[selected].priority &&
                     result[i].arrival_time == result[selected].arrival_time &&
                     result[i].job_id < result[selected].job_id)) {

                    selected = i;
                }
            }
        }

        /* If no job is available, CPU remains idle */
        if (selected == -1) {

            int earliest = -1;

            for (int i = 0; i < n; i++) {

                if (!completed_flags[i]) {

                    if (earliest == -1 ||
                        result[i].arrival_time <
                        result[earliest].arrival_time) {

                        earliest = i;
                    }
                }
            }

            printf("| IDLE (%d-%d) ",
                   current_time,
                   result[earliest].arrival_time);

            current_time = result[earliest].arrival_time;

            continue;
        }

        int start_time = current_time;

        /* Execute the selected job completely */
        current_time += result[selected].burst_time;

        result[selected].completion_time = current_time;

        result[selected].turnaround_time =
            result[selected].completion_time -
            result[selected].arrival_time;

        result[selected].waiting_time =
            result[selected].turnaround_time -
            result[selected].burst_time;

        result[selected].remaining_time = 0;

        completed_flags[selected] = 1;
        completed++;

        total_wt += result[selected].waiting_time;
        total_tat += result[selected].turnaround_time;

        printf("| J%d (%d-%d) ",
               result[selected].job_id,
               start_time,
               current_time);
    }

    printf("|\n");

    printf("\n%-8s %-8s %-8s %-8s %-8s %-8s\n",
           "Job", "AT", "BT", "CT", "TAT", "WT");

    for (int i = 0; i < n; i++) {

        printf("%-8d %-8d %-8d %-8d %-8d %-8d\n",
               result[i].job_id,
               result[i].arrival_time,
               result[i].burst_time,
               result[i].completion_time,
               result[i].turnaround_time,
               result[i].waiting_time);
    }

    printf("\nAverage Waiting Time: %.2f\n", total_wt / n);
    printf("Average Turnaround Time: %.2f\n", total_tat / n);

    printf("\n=========================================\n");
}


/* ==========================================
   5. COMPARE ALL SCHEDULING ALGORITHMS
   ========================================== */

void compare_schedulers(const Job jobs[], int n)
{
    if (!validate_jobs(jobs, n)) {
        return;
    }

    Job fcfs[MAX_JOBS];
    Job rr[MAX_JOBS];
    Job priority[MAX_JOBS];

    /* Copy original jobs */
    for (int i = 0; i < n; i++) {
        fcfs[i] = jobs[i];
        rr[i] = jobs[i];
        priority[i] = jobs[i];
    }

    /* Run FCFS */
    run_fcfs(fcfs, n);

    /* Run Round Robin */
    run_round_robin(rr, n, 2);

    /* Run Priority */
    run_priority(priority, n);

    /* Calculate averages from each algorithm's results */
    float fcfs_wt = 0, fcfs_tat = 0;
    float rr_wt = 0, rr_tat = 0;
    float priority_wt = 0, priority_tat = 0;

    /*
       The scheduling functions display results but use local
       copies internally. Recalculate averages independently
       using the same scheduling logic for comparison.
    */

    /* FCFS calculation */
    Job temp[MAX_JOBS];

    for (int i = 0; i < n; i++) {
        temp[i] = jobs[i];
    }

    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (temp[i].arrival_time > temp[j].arrival_time ||
               (temp[i].arrival_time == temp[j].arrival_time &&
                temp[i].job_id > temp[j].job_id)) {
                Job swap = temp[i];
                temp[i] = temp[j];
                temp[j] = swap;
            }
        }
    }

    int time = 0;

    for (int i = 0; i < n; i++) {
        if (time < temp[i].arrival_time) {
            time = temp[i].arrival_time;
        }

        time += temp[i].burst_time;

        int tat = time - temp[i].arrival_time;
        int wt = tat - temp[i].burst_time;

        fcfs_tat += tat;
        fcfs_wt += wt;
    }

    /* Round Robin calculation */
    for (int i = 0; i < n; i++) {
        rr[i] = jobs[i];
        rr[i].remaining_time = rr[i].burst_time;
    }

    int queue[MAX_JOBS];
    int front = 0, rear = 0, count = 0;
    int next_job = 0, completed = 0;
    time = 0;

    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (rr[i].arrival_time > rr[j].arrival_time ||
               (rr[i].arrival_time == rr[j].arrival_time &&
                rr[i].job_id > rr[j].job_id)) {
                Job swap = rr[i];
                rr[i] = rr[j];
                rr[j] = swap;
            }
        }
    }

    while (completed < n) {
        if (count == 0) {
            if (next_job < n && time < rr[next_job].arrival_time) {
                time = rr[next_job].arrival_time;
            }

            while (next_job < n && rr[next_job].arrival_time <= time) {
                enqueue_rr(queue, &rear, &count, next_job);
                next_job++;
            }
        }

        int index = dequeue_rr(queue, &front, &count);

        int execution = rr[index].remaining_time > 2
                        ? 2 : rr[index].remaining_time;

        time += execution;
        rr[index].remaining_time -= execution;

        while (next_job < n && rr[next_job].arrival_time <= time) {
            enqueue_rr(queue, &rear, &count, next_job);
            next_job++;
        }

        if (rr[index].remaining_time == 0) {
            rr[index].completion_time = time;
            rr[index].turnaround_time =
                time - rr[index].arrival_time;
            rr[index].waiting_time =
                rr[index].turnaround_time - rr[index].burst_time;

            rr_wt += rr[index].waiting_time;
            rr_tat += rr[index].turnaround_time;
            completed++;
        } else {
            enqueue_rr(queue, &rear, &count, index);
        }
    }

    /* Priority calculation */
    for (int i = 0; i < n; i++) {
        priority[i] = jobs[i];
    }

    int done[MAX_JOBS] = {0};
    completed = 0;
    time = 0;

    while (completed < n) {
        int selected = -1;

        for (int i = 0; i < n; i++) {
            if (!done[i] && priority[i].arrival_time <= time) {
                if (selected == -1 ||
                    priority[i].priority < priority[selected].priority ||
                    (priority[i].priority == priority[selected].priority &&
                     priority[i].arrival_time < priority[selected].arrival_time) ||
                    (priority[i].priority == priority[selected].priority &&
                     priority[i].arrival_time == priority[selected].arrival_time &&
                     priority[i].job_id < priority[selected].job_id)) {
                    selected = i;
                }
            }
        }

        if (selected == -1) {
            int earliest = -1;

            for (int i = 0; i < n; i++) {
                if (!done[i] &&
                    (earliest == -1 ||
                     priority[i].arrival_time <
                     priority[earliest].arrival_time)) {
                    earliest = i;
                }
            }

            time = priority[earliest].arrival_time;
            continue;
        }

        time += priority[selected].burst_time;

        int tat = time - priority[selected].arrival_time;
        int wt = tat - priority[selected].burst_time;

        priority_wt += wt;
        priority_tat += tat;

        done[selected] = 1;
        completed++;
    }

    /* Display comparison table */
    printf("\n========== SCHEDULING COMPARISON ==========\n");

    printf("\n%-20s %-20s %-20s\n",
           "Algorithm",
           "Avg Waiting Time",
           "Avg Turnaround Time");

    printf("%-20s %-20.2f %-20.2f\n",
           "FCFS", fcfs_wt / n, fcfs_tat / n);

    printf("%-20s %-20.2f %-20.2f\n",
           "Round Robin", rr_wt / n, rr_tat / n);

    printf("%-20s %-20.2f %-20.2f\n",
           "Priority", priority_wt / n, priority_tat / n);

    printf("\n===========================================\n");
}
