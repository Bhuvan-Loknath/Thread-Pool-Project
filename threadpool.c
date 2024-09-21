#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "threadpool.h"

#define MAX_QUEUE_SIZE 10

// Structure for a task in the queue
typedef struct Task {
    void (*function)(void *p);
    void *parameter;
    struct Task *next;
} Task;

// Thread pool structure
typedef struct {
    pthread_t *threads;
    Task *queue;
    pthread_mutex_t queue_lock;
    sem_t task_semaphore;
    int shutdown;
} ThreadPool;

static ThreadPool pool;

// Add a new task to the queue
static void enqueue(void (*function)(void *p), void *p) {
    Task *newTask = (Task *)malloc(sizeof(Task));
    newTask->function = function;
    newTask->parameter = p;
    newTask->next = NULL;

    pthread_mutex_lock(&pool.queue_lock);

    if (pool.queue == NULL) {
        pool.queue = newTask;
    } else {
        Task *current = pool.queue;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newTask;
    }

    sem_post(&pool.task_semaphore);  // Signal that a new task is available
    pthread_mutex_unlock(&pool.queue_lock);
}

// Remove and return a task from the queue
static Task *dequeue(void) {
    pthread_mutex_lock(&pool.queue_lock);
    while (pool.queue == NULL && !pool.shutdown) {
        pthread_mutex_unlock(&pool.queue_lock);
        sem_wait(&pool.task_semaphore);
        pthread_mutex_lock(&pool.queue_lock);
    }

    if (pool.shutdown) {
        pthread_mutex_unlock(&pool.queue_lock);
        return NULL;
    }

    Task *task = pool.queue;
    pool.queue = task->next;
    pthread_mutex_unlock(&pool.queue_lock);
    return task;
}

// Worker thread function
static void *worker(void *arg) {
    while (1) {
        Task *task = dequeue();
        if (task == NULL) {
            pthread_exit(NULL);
        }
        task->function(task->parameter);
        free(task);
    }
    return NULL;
}

// Initialize the thread pool
void pool_init(void) {
    pool.threads = (pthread_t *)malloc(sizeof(pthread_t) * MAX_QUEUE_SIZE);
    pool.queue = NULL;
    pool.shutdown = 0;

    pthread_mutex_init(&pool.queue_lock, NULL);
    sem_init(&pool.task_semaphore, 0, 0);

    for (int i = 0; i < MAX_QUEUE_SIZE; i++) {
        pthread_create(&pool.threads[i], NULL, worker, NULL);  // Create worker threads
    }
}

// Submit a task to the pool
int pool_submit(void (*function)(void *p), void *p) {
    if (pool.shutdown) {
        return 1;  // Cannot submit tasks if the pool is shutting down
    }
    enqueue(function, p);
    return 0;
}

// Shutdown the thread pool
void pool_shutdown(void) {
    pool.shutdown = 1;
    for (int i = 0; i < MAX_QUEUE_SIZE; i++) {
        sem_post(&pool.task_semaphore);  // Signal all worker threads to stop
    }
    for (int i = 0; i < MAX_QUEUE_SIZE; i++) {
        pthread_join(pool.threads[i], NULL);  // Wait for all threads to finish
    }

    free(pool.threads);
    while (pool.queue != NULL) {
        Task *task = pool.queue;
        pool.queue = task->next;
        free(task);
    }

    pthread_mutex_destroy(&pool.queue_lock);
    sem_destroy(&pool.task_semaphore);
}
