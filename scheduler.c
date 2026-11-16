#include <stdio.h>
#include "scheduler.h"

/* Validate the user-entered jobs */
int validate_jobs(const Job jobs[], int n)
{
    if (n < 1 || n > MAX_JOBS) {
        printf("Error: Number of jobs must be between 1 and %d.\n", MAX_JOBS);
        return 0;
    }

    for (int i = 0; i < n; i++) {
        if (jobs[i].job_id <= 0 ||
            jobs[i].arrival_time < 0 ||
            jobs[i].burst_time <= 0 ||
            jobs[i].priority < 0) {
            printf("Error: Invalid details for Job %d.\n", i + 1);
            return 0;
        }

        for (int j = i + 1; j < n; j++) {
            if (jobs[i].job_id == jobs[j].job_id) {
                printf("Error: Duplicate Job ID %d.\n", jobs[i].job_id);
                return 0;
            }
        }
    }

    return 1;
}

/* First Come First Serve */
void run_fcfs(Job jobs[], int n)
{
    int completed[MAX_JOBS] = {0};
    int time = 0;

    printf("\n========== FCFS SCHEDULING ==========\n");

    for (int count = 0; count < n; count++) {
        int selected = -1;

        for (int i = 0; i < n; i++) {
            if (!completed[i] &&
                (selected == -1 ||
                 jobs[i].arrival_time < jobs[selected].arrival_time)) {
                selected = i;
            }
        }

        if (selected == -1)
            break;

        if (time < jobs[selected].arrival_time)
            time = jobs[selected].arrival_time;

        time += jobs[selected].burst_time;

        jobs[selected].completion_time = time;
        jobs[selected].turnaround_time =
            time - jobs[selected].arrival_time;
        jobs[selected].waiting_time =
            jobs[selected].turnaround_time - jobs[selected].burst_time;

        completed[selected] = 1;
    }

    printf("\nJob ID\tAT\tBT\tCT\tTAT\tWT\n");

    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\t%d\n",
               jobs[i].job_id,
               jobs[i].arrival_time,
               jobs[i].burst_time,
               jobs[i].completion_time,
               jobs[i].turnaround_time,
               jobs[i].waiting_time);
    }
}

/* Round Robin Scheduling */
void run_round_robin(Job jobs[], int n, int quantum)
{
    int queue[MAX_JOBS + 1];
    int added[MAX_JOBS] = {0};
    int front = 0, rear = 0, count = 0;
    int time = 0, completed = 0;

    if (quantum <= 0) {
        printf("Error: Time quantum must be positive.\n");
        return;
    }

    for (int i = 0; i < n; i++)
        jobs[i].remaining_time = jobs[i].burst_time;

    printf("\n========== ROUND ROBIN SCHEDULING ==========\n");

    while (completed < n) {

        /* Add arrived jobs in arrival-time order */
        while (1) {
            int next = -1;

            for (int i = 0; i < n; i++) {
                if (!added[i] && jobs[i].arrival_time <= time &&
                    (next == -1 ||
                     jobs[i].arrival_time < jobs[next].arrival_time)) {
                    next = i;
                }
            }

            if (next == -1)
                break;

            queue[rear] = next;
            rear = (rear + 1) % (MAX_JOBS + 1);
            count++;
            added[next] = 1;
        }

        /* If the queue is empty, move to the next arrival */
        if (count == 0) {
            int next = -1;

            for (int i = 0; i < n; i++) {
                if (!added[i] &&
                    (next == -1 ||
                     jobs[i].arrival_time < jobs[next].arrival_time)) {
                    next = i;
                }
            }

            if (next == -1)
                break;

            time = jobs[next].arrival_time;
            continue;
        }

        int current = queue[front];
        front = (front + 1) % (MAX_JOBS + 1);
        count--;

        int execute;

        if (jobs[current].remaining_time > quantum)
            execute = quantum;
        else
            execute = jobs[current].remaining_time;

        time += execute;
        jobs[current].remaining_time -= execute;

        /* Add newly arrived jobs before re-queuing current job */
        while (1) {
            int next = -1;

            for (int i = 0; i < n; i++) {
                if (!added[i] && jobs[i].arrival_time <= time &&
                    (next == -1 ||
                     jobs[i].arrival_time < jobs[next].arrival_time)) {
                    next = i;
                }
            }

            if (next == -1)
                break;

            queue[rear] = next;
            rear = (rear + 1) % (MAX_JOBS + 1);
            count++;
            added[next] = 1;
        }

        if (jobs[current].remaining_time == 0) {
            jobs[current].completion_time = time;
            jobs[current].turnaround_time =
                time - jobs[current].arrival_time;
            jobs[current].waiting_time =
                jobs[current].turnaround_time - jobs[current].burst_time;

            completed++;
        } else {
            queue[rear] = current;
            rear = (rear + 1) % (MAX_JOBS + 1);
            count++;
        }
    }

    printf("\nJob ID\tAT\tBT\tCT\tTAT\tWT\n");

    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\t%d\n",
               jobs[i].job_id,
               jobs[i].arrival_time,
               jobs[i].burst_time,
               jobs[i].completion_time,
               jobs[i].turnaround_time,
               jobs[i].waiting_time);
    }
}

/* Non-preemptive Priority Scheduling */
void run_priority(Job jobs[], int n)
{
    int completed[MAX_JOBS] = {0};
    int time = 0;

    printf("\n========== PRIORITY SCHEDULING ==========\n");
    printf("Smaller priority number = Higher priority\n");

    for (int count = 0; count < n; count++) {
        int selected = -1;

        for (int i = 0; i < n; i++) {
            if (!completed[i] && jobs[i].arrival_time <= time) {
                if (selected == -1 ||
                    jobs[i].priority < jobs[selected].priority ||
                    (jobs[i].priority == jobs[selected].priority &&
                     jobs[i].arrival_time < jobs[selected].arrival_time)) {
                    selected = i;
                }
            }
        }

        if (selected == -1) {
            int next = -1;

            for (int i = 0; i < n; i++) {
                if (!completed[i] &&
                    (next == -1 ||
                     jobs[i].arrival_time < jobs[next].arrival_time)) {
                    next = i;
                }
            }

            if (next == -1)
                break;

            time = jobs[next].arrival_time;
            continue;
        }

        time += jobs[selected].burst_time;

        jobs[selected].completion_time = time;
        jobs[selected].turnaround_time =
            time - jobs[selected].arrival_time;
        jobs[selected].waiting_time =
            jobs[selected].turnaround_time - jobs[selected].burst_time;

        completed[selected] = 1;
    }

    printf("\nJob ID\tAT\tBT\tPriority\tCT\tTAT\tWT\n");

    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t\t%d\t%d\t%d\n",
               jobs[i].job_id,
               jobs[i].arrival_time,
               jobs[i].burst_time,
               jobs[i].priority,
               jobs[i].completion_time,
               jobs[i].turnaround_time,
               jobs[i].waiting_time);
    }
}

/* Compare all three scheduling algorithms */
void compare_schedulers(const Job jobs[], int n, int quantum)
{
    if (!validate_jobs(jobs, n))
        return;

    if (quantum <= 0) {
        printf("Error: Time quantum must be positive.\n");
        return;
    }

    Job fcfs[MAX_JOBS];
    Job rr[MAX_JOBS];
    Job priority[MAX_JOBS];

    float fcfs_wt = 0, fcfs_tat = 0;
    float rr_wt = 0, rr_tat = 0;
    float priority_wt = 0, priority_tat = 0;

    for (int i = 0; i < n; i++) {
        fcfs[i] = jobs[i];
        rr[i] = jobs[i];
        priority[i] = jobs[i];
    }

    run_fcfs(fcfs, n);
    run_round_robin(rr, n, quantum);
    run_priority(priority, n);

    for (int i = 0; i < n; i++) {
        fcfs_wt += fcfs[i].waiting_time;
        fcfs_tat += fcfs[i].turnaround_time;

        rr_wt += rr[i].waiting_time;
        rr_tat += rr[i].turnaround_time;

        priority_wt += priority[i].waiting_time;
        priority_tat += priority[i].turnaround_time;
    }

    printf("\n========== SCHEDULER COMPARISON ==========\n");
    printf("%-18s %-18s %-18s\n",
           "Algorithm", "Average WT", "Average TAT");

    printf("%-18s %-18.2f %-18.2f\n",
           "FCFS", fcfs_wt / n, fcfs_tat / n);

    printf("%-18s %-18.2f %-18.2f\n",
           "Round Robin", rr_wt / n, rr_tat / n);

    printf("%-18s %-18.2f %-18.2f\n",
           "Priority", priority_wt / n, priority_tat / n);

    printf("==========================================\n");
}
