
#include <stdio.h>
#include <sys/resource.h>

#include "resource.h"

void run_resource_demo(void)
{
    struct rlimit original_limit;
    struct rlimit new_limit;
    struct rlimit current_limit;

    printf("\n========== RESOURCE MANAGEMENT ==========\n");

    /* Read the original file descriptor limit */
    if (getrlimit(RLIMIT_NOFILE, &original_limit) == -1) {
        perror("getrlimit");
        return;
    }

    printf("Original soft limit: %llu\n",
           (unsigned long long)original_limit.rlim_cur);

    printf("Original hard limit: %llu\n",
           (unsigned long long)original_limit.rlim_max);

    /* Prepare a temporary lower soft limit */
    new_limit = original_limit;

    if (original_limit.rlim_cur > 64) {
        new_limit.rlim_cur = 64;
    }

    /* Apply the temporary limit */
    if (setrlimit(RLIMIT_NOFILE, &new_limit) == -1) {
        perror("setrlimit");
        return;
    }

    if (getrlimit(RLIMIT_NOFILE, &current_limit) == -1) {
        perror("getrlimit");
        setrlimit(RLIMIT_NOFILE, &original_limit);
        return;
    }

    printf("\nTemporary resource limit applied.\n");
    printf("Current soft limit: %llu\n",
           (unsigned long long)current_limit.rlim_cur);

    /* Restore the original resource limit */
    if (setrlimit(RLIMIT_NOFILE, &original_limit) == -1) {
        perror("Restoring original limit");
        return;
    }

    if (getrlimit(RLIMIT_NOFILE, &current_limit) == -1) {
        perror("getrlimit");
        return;
    }

    printf("\nOriginal resource limit restored.\n");
    printf("Restored soft limit: %llu\n",
           (unsigned long long)current_limit.rlim_cur);

    printf("Resource cleanup completed successfully.\n");
    printf("=========================================\n");
}
