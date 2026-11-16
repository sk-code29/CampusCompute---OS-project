
#include <stdio.h>

#define MAX_PROCESSES 10
#define MAX_RESOURCES 10

void run_banker(void)
{
    int allocation[MAX_PROCESSES][MAX_RESOURCES];
    int maximum[MAX_PROCESSES][MAX_RESOURCES];
    int need[MAX_PROCESSES][MAX_RESOURCES];

    int available[MAX_RESOURCES];
    int work[MAX_RESOURCES];

    int finish[MAX_PROCESSES] = {0};
    int safe_sequence[MAX_PROCESSES];

    int processes, resources;
    int completed = 0;

    printf("\n========== BANKER'S ALGORITHM ==========\n");

    /* Number of processes */
    printf("Enter number of processes (1-%d): ", MAX_PROCESSES);

    if (scanf("%d", &processes) != 1 ||
        processes < 1 || processes > MAX_PROCESSES) {
        printf("Error: Invalid number of processes.\n");
        return;
    }

    /* Number of resources */
    printf("Enter number of resource types (1-%d): ", MAX_RESOURCES);

    if (scanf("%d", &resources) != 1 ||
        resources < 1 || resources > MAX_RESOURCES) {
        printf("Error: Invalid number of resources.\n");
        return;
    }

    /* Allocation Matrix */
    printf("\nEnter Allocation Matrix:\n");

    for (int i = 0; i < processes; i++) {
        printf("P%d: ", i);

        for (int j = 0; j < resources; j++) {
            if (scanf("%d", &allocation[i][j]) != 1 ||
                allocation[i][j] < 0) {
                printf("Error: Allocation must contain non-negative integers.\n");
                return;
            }
        }
    }

    /* Maximum Matrix */
    printf("\nEnter Maximum Requirement Matrix:\n");

    for (int i = 0; i < processes; i++) {
        printf("P%d: ", i);

        for (int j = 0; j < resources; j++) {
            if (scanf("%d", &maximum[i][j]) != 1 ||
                maximum[i][j] < allocation[i][j]) {
                printf("Error: Maximum must be a valid integer and cannot be less than Allocation.\n");
                return;
            }

            need[i][j] = maximum[i][j] - allocation[i][j];
        }
    }

    /* Available Resources */
    printf("\nEnter Available Resources:\n");

    for (int j = 0; j < resources; j++) {
        printf("R%d: ", j);

        if (scanf("%d", &available[j]) != 1 ||
            available[j] < 0) {
            printf("Error: Available resources must be non-negative integers.\n");
            return;
        }

        work[j] = available[j];
    }

    /* Validate resource claims */
    for (int j = 0; j < resources; j++) {
        long long total = available[j];

        for (int i = 0; i < processes; i++) {
            total += allocation[i][j];
        }

        for (int i = 0; i < processes; i++) {
            if (maximum[i][j] > total) {
                printf("Error: Maximum claim of P%d exceeds total instances of R%d.\n",
                       i, j);
                return;
            }
        }
    }

    /* Display Need Matrix */
    printf("\n========== NEED MATRIX ==========\n");

    printf("Process\t");

    for (int j = 0; j < resources; j++) {
        printf("R%d\t", j);
    }

    printf("\n");

    for (int i = 0; i < processes; i++) {
        printf("P%d\t", i);

        for (int j = 0; j < resources; j++) {
            printf("%d\t", need[i][j]);
        }

        printf("\n");
    }

    /* Banker's Safety Algorithm */
    while (completed < processes) {
        int found = 0;

        for (int i = 0; i < processes; i++) {

            if (finish[i] == 0) {
                int possible = 1;

                for (int j = 0; j < resources; j++) {
                    if (need[i][j] > work[j]) {
                        possible = 0;
                        break;
                    }
                }

                if (possible) {

                    /* Release resources after process completion */
                    for (int j = 0; j < resources; j++) {
                        work[j] += allocation[i][j];
                    }

                    safe_sequence[completed] = i;
                    completed++;

                    finish[i] = 1;
                    found = 1;
                }
            }
        }

        if (found == 0) {
            break;
        }
    }

    /* Display Result */
    printf("\n========== BANKER'S ALGORITHM RESULT ==========\n");

    if (completed == processes) {

        printf("System is in a SAFE state.\n");

        printf("Safe Sequence: ");

        for (int i = 0; i < processes; i++) {
            printf("P%d", safe_sequence[i]);

            if (i != processes - 1) {
                printf(" -> ");
            }
        }

        printf("\nAll processes can complete successfully.\n");

    } else {

        printf("System is in an UNSAFE state.\n");

        printf("No complete safe sequence exists.\n");

        printf("Processes unable to complete: ");

        for (int i = 0; i < processes; i++) {
            if (finish[i] == 0) {
                printf("P%d ", i);
            }
        }

        printf("\nThe system may lead to deadlock.\n");
    }

    printf("================================================\n");
}
