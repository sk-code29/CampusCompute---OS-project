#ifndef SCHEDULER_H
#define SCHEDULER_H

#define MAX_JOBS 20

typedef struct {
    int job_id;
    int arrival_time;
    int burst_time;
    int priority;

    int remaining_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
} Job;

/* Job validation */
int validate_jobs(const Job jobs[], int n);

/* CPU scheduling algorithms */
void run_fcfs(Job jobs[], int n);
void run_round_robin(Job jobs[], int n, int quantum);
void run_priority(Job jobs[], int n);

/* Compare scheduling algorithms */
void compare_schedulers(const Job jobs[], int n);

#endif