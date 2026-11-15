
#include <stdio.h>
#include "banker.h"

int main(void)
{
    printf("\n========================================\n");
    printf("       BANKER'S ALGORITHM TESTING       \n");
    printf("========================================\n");

    /* Test safe state */
    run_banker_safe();

    /* Test unsafe state */
    run_banker_unsafe();

    printf("\nAll Banker's Algorithm tests completed.\n");

    return 0;
}
