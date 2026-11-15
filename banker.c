
#include <stdio.h>
#include "banker.h"

#define PROCESSES 5
#define RESOURCES 3

/* Check whether a process's remaining need can be satisfied */
int can_allocate(int need[], int available[])
{
    for (int j = 0; j < RESOURCES; j++) {
        if (need[j] > available[j]) {
            return 0;
        }
    }

    return 1;
}

/* Banker's Safety Algorithm */
void run_bankers_algorithm(
    int allocation[PROCESSES][RESOURCES],
    int maximum[PROCESSES][RESOURCES],
    int available[RESOURCES])
{
    int need[PROCESSES][RESOURCES];
    int work[RESOURCES];
    int finish[PROCESSES] = {0};
    int safe_sequence[PROCESSES];

    int count = 0;

    /* Step 1: Calculate Need matrix */
    for (int i = 0; i < PROCESSES; i++) {
        for (int j = 0; j < RESOURCES; j++) {
            need[i][j] = maximum[i][j] - allocation[i][j];
        }
    }

    /* Step 2: Initialize Work with Available */
    for (int j = 0; j < RESOURCES; j++) {
        work[j] = available[j];
    }

    printf("\nNeed Matrix:\n");

    for (int i = 0; i < PROCESSES; i++) {
        printf("P%d: ", i);

        for (int j = 0; j < RESOURCES; j++) {
            printf("%d ", need[i][j]);
        }

        printf("\n");
    }

    /* Step 3: Find a safe execution sequence */
    while (count < PROCESSES) {

        int found = 0;

        for (int i = 0; i < PROCESSES; i++) {

            if (finish[i] == 0 &&
                can_allocate(need[i], work)) {

                /* Release allocated resources after completion */
                for (int j = 0; j < RESOURCES; j++) {
                    work[j] += allocation[i][j];
                }

                safe_sequence[count] = i;
                count++;

                finish[i] = 1;
                found = 1;
            }
        }

        /* No process can proceed */
        if (found == 0) {
            break;
        }
    }

    /* Step 4: Display the result */
    if (count == PROCESSES) {

        printf("\nSystem is in a SAFE state.\n");

        printf("Safe Sequence: ");

        for (int i = 0; i < PROCESSES; i++) {
            printf("P%d", safe_sequence[i]);

            if (i != PROCESSES - 1) {
                printf(" -> ");
            }
        }

        printf("\n");

    } else {

        printf("\nSystem is in an UNSAFE state.\n");

        printf("No complete safe sequence exists.\n");

        printf("Processes unable to complete: ");

        for (int i = 0; i < PROCESSES; i++) {
            if (finish[i] == 0) {
                printf("P%d ", i);
            }
        }

        printf("\n");
    }
}


/* ==========================================
   TEST 1: SAFE RESOURCE ALLOCATION
   ========================================== */

void run_banker_safe(void)
{
    printf("\n========== BANKER'S ALGORITHM ==========\n");
    printf("TEST 1: SAFE RESOURCE ALLOCATION\n");

    int allocation[PROCESSES][RESOURCES] = {
        {0, 1, 0},
        {2, 0, 0},
        {3, 0, 2},
        {2, 1, 1},
        {0, 0, 2}
    };

    int maximum[PROCESSES][RESOURCES] = {
        {7, 5, 3},
        {3, 2, 2},
        {9, 0, 2},
        {2, 2, 2},
        {4, 3, 3}
    };

    int available[RESOURCES] = {3, 3, 2};

    run_bankers_algorithm(allocation, maximum, available);

    printf("\n========================================\n");
}


/* ==========================================
   TEST 2: UNSAFE RESOURCE ALLOCATION
   ========================================== */

void run_banker_unsafe(void)
{
    printf("\n========== BANKER'S ALGORITHM ==========\n");
    printf("TEST 2: UNSAFE RESOURCE ALLOCATION\n");

    int allocation[PROCESSES][RESOURCES] = {
        {0, 1, 0},
        {2, 0, 0},
        {3, 0, 2},
        {2, 1, 1},
        {0, 0, 2}
    };

    int maximum[PROCESSES][RESOURCES] = {
        {7, 5, 3},
        {3, 2, 2},
        {9, 0, 2},
        {2, 2, 2},
        {4, 3, 3}
    };

    /* Insufficient available resources */
    int available[RESOURCES] = {0, 0, 0};

    run_bankers_algorithm(allocation, maximum, available);

    printf("\n========================================\n");
}
