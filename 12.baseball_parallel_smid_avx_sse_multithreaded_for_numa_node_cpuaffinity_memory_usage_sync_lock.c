#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <immintrin.h>
#include <numa.h>
#include <sched.h>
#include <time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <semaphore.h>

#define MAX_OPERATIONS 1000000
#define NUM_THREADS 4
#define SIMD_THRESHOLD 500

pthread_mutex_t sum_mutex; // Mutex for synchronization

// Data structure for thread parameters
typedef struct {
    int *records;
    int start, end;
    int partial_sum;
    int numa_node;
} ThreadData;

// Bind thread to specific NUMA node
void bindThreadToNUMANode(int node) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(node, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
}

// SIMD optimized sum for NUMA nodes
void *numaSimdSum(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    bindThreadToNUMANode(data->numa_node);

    __m256i sumVec = _mm256_setzero_si256();
    int i;
    for (i = data->start; i + 7 < data->end; i += 8) {
        __m256i values = _mm256_loadu_si256((__m256i*)&data->records[i]);
        sumVec = _mm256_add_epi32(sumVec, values);
    }

    int sumArray[8];
    _mm256_storeu_si256((__m256i*)sumArray, sumVec);

    int sum = 0;
    for (int k = 0; k < 8; k++) sum += sumArray[k];
    for (; i < data->end; i++) sum += data->records[i];

    pthread_mutex_lock(&sum_mutex);
    data->partial_sum = sum;
    pthread_mutex_unlock(&sum_mutex);

    pthread_exit(NULL);
}

// Calculate points conditionally using threads and SIMD
int calPoints(char *ops[], int size, int useMultithreading) {
    static int records[MAX_OPERATIONS];
    int index = 0;
    for (int i = 0; i < size; i++) {
        records[index++] = atoi(ops[i]);
    }

    clock_t sync_start, sync_end;

    if (!useMultithreading || index < SIMD_THRESHOLD) { // Single-threaded SIMD for small workloads
        ThreadData single_thread = {records, 0, index, 0, 0};
        sync_start = clock();
        numaSimdSum(&single_thread);
        sync_end = clock();
        printf("Thread synchronization overhead (single-threaded): %.3f ms\n", 
               ((double)(sync_end - sync_start) * 1000 / CLOCKS_PER_SEC));
        return single_thread.partial_sum;
    }

    pthread_t threads[NUM_THREADS];
    ThreadData data[NUM_THREADS];
    int chunk = index / NUM_THREADS;

    pthread_mutex_init(&sum_mutex, NULL);

    sync_start = clock();

    for (int i = 0; i < NUM_THREADS; i++) {
        data[i].records = records;
        data[i].start = i * chunk;
        data[i].end = (i == NUM_THREADS - 1) ? index : (i + 1) * chunk;
        data[i].numa_node = i % numa_num_configured_nodes();
        pthread_create(&threads[i], NULL, numaSimdSum, &data[i]);
    }

    int totalSum = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        totalSum += data[i].partial_sum;
    }

    sync_end = clock();
    pthread_mutex_destroy(&sum_mutex);

    printf("Thread synchronization overhead (multi-threaded): %.3f ms\n", 
           ((double)(sync_end - sync_start) * 1000 / CLOCKS_PER_SEC));

    return totalSum;
}

// Set CPU Affinity explicitly
void setCPUAffinity(int startCPU, int endCPU) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    for (int i = startCPU; i <= endCPU; i++) {
        CPU_SET(i, &cpuset);
    }
    sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);
}

// Utility: Memory usage reporting
void printMemoryUsage(const char* phase) {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("%s - Memory usage: %ld KB\n", phase, usage.ru_maxrss);
}

// Utility: CPU time reporting
void printCPUTime(const char* phase, clock_t start, clock_t end) {
    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("%s - CPU Time used: %.3f sec\n", phase, cpu_time_used);
}

// Main entry point
int main() {
    static char *largeOps[MAX_OPERATIONS];

    for (int i = 0; i < 500000; i++) {
        largeOps[i * 2] = "10";
        largeOps[i * 2 + 1] = "D";
    }

    if (numa_available() == -1) {
        printf("NUMA is not available on this system.\n");
        return EXIT_FAILURE;
    }

    printMemoryUsage("Before Execution");

    clock_t start = clock();
    int result = calPoints(largeOps, MAX_OPERATIONS, 1);
    clock_t end = clock();

    printCPUTime("Conditional SIMD & Multi-threaded Execution", start, end);
    printf("Final Result: %d\n", result);

    printMemoryUsage("After Execution");

    struct bitmask *nodes = numa_get_mems_allowed();
    printf("Memory allowed on NUMA nodes: ");
    for (unsigned i = 0; i < numa_num_configured_nodes(); i++) {
        if (numa_bitmask_isbitset(nodes, i))
            printf("%u ", i);
    }
    printf("\n");

    numa_free_nodemask(nodes);

    return EXIT_SUCCESS;
}
