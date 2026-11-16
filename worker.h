#include <stdio.h>
#include "worker.h"

int main(void)
{
    printf("========== WORKER MODULE TEST ==========\n");

    run_worker_demo();

    printf("\n========== TEST COMPLETED ==========\n");

    return 0;
}
