#include "threadpool.h"
#include <stdio.h>

// Function to execute a task that prints an integer argument
void example_task(void *p) {
    int *num = (int *)p;
    printf("Task executing with argument: %d\n", *num);
}

int main() {
    pool_init();  // Initialize the thread pool

    int arg1 = 1, arg2 = 2, arg3 = 3;

    pool_submit(example_task, &arg1);  // Submit tasks to the pool
    pool_submit(example_task, &arg2);
    pool_submit(example_task, &arg3);

    sleep(2);  // Wait for threads to complete

    pool_shutdown();  // Shutdown the thread pool
    return 0;
}
