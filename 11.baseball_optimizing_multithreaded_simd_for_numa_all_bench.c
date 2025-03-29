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

#define MAX_OPERATIONS 1000000
#define NUM_THREADS 2

typedef struct {
    int *records;
    int start, end;
    int result;
    int numa_node;
} ThreadData;

// Bind thread to specific NUMA node
void bindThreadToNUMANode(int node) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(node, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
}

// SIMD optimized sum function for NUMA nodes
void *numaSimdSum(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    bindThreadToNUMANode(data->numa_node);

    __m256i sumVec = _mm256_setzero_si256();
    int i;

    for (i = data->start; i + 7 < data->end; i += 8) {
        __m256i values = _mm256_load_si256((__m256i*)&data->records[i]);
        sumVec = _mm256_add_epi32(sumVec, values);
    }

    int sumArray[8];
    _mm256_store_si256((__m256i*)sumArray, sumVec);

    int sum = sumArray[0] + sumArray[1] + sumArray[2] + sumArray[3] +
              sumArray[4] + sumArray[5] + sumArray[6] + sumArray[7];

    for (; i < data->end; i++) {
        sum += data->records[i];
    }

    data->result = sum;
    pthread_exit(NULL);
}

// Calculate points using threads and SIMD
int calPoints(char *ops[], int size) {
    static int records[MAX_OPERATIONS];
    int index = 0;

    for (int i = 0; i < size; i++) {
        records[index++] = atoi(ops[i]);
    }

    pthread_t threads[NUM_THREADS];
    ThreadData data[NUM_THREADS];
    int mid = index / NUM_THREADS;

    clock_t sync_start = clock();

    for (int i = 0; i < NUM_THREADS; i++) {
        data[i].records = records;
        data[i].start = i * mid;
        data[i].end = (i == NUM_THREADS - 1) ? index : (i + 1) * mid;
        data[i].numa_node = i; 
        pthread_create(&threads[i], NULL, numaSimdSum, &data[i]);
    }

    int totalSum = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        totalSum += data[i].result;
    }

    clock_t sync_end = clock();
    printf("Thread synchronization overhead: %.3f ms\n", ((double)(sync_end - sync_start) * 1000 / CLOCKS_PER_SEC));

    return totalSum;
}

// Set CPU Affinity
void setCPUAffinity(int startCPU, int endCPU) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    for (int i = startCPU; i <= endCPU; i++) {
        CPU_SET(i, &cpuset);
    }
    sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);
}

// Print memory usage
void printMemoryUsage(const char* phase) {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("%s - Memory usage: %ld KB\n", phase, usage.ru_maxrss);
}

// Print CPU time usage
void printCPUTime(const char* phase, clock_t start, clock_t end) {
    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("%s - CPU Time used: %.3f sec\n", phase, cpu_time_used);
}

int main() {
    static char *largeOps[MAX_OPERATIONS];

    // Generate large operations set
    for (int i = 0; i < 500000; i++) {
        largeOps[i * 2] = "10";
        largeOps[i * 2 + 1] = "D";
    }

    // Check NUMA availability
    if (numa_available() == -1) {
        printf("NUMA is not available on this system.\n");
        return EXIT_FAILURE;
    }

    printMemoryUsage("Before Execution");

    printf("Running benchmark without NUMA-awareness:\n");
    clock_t start = clock();
    int result_non_numa = calPoints(largeOps, MAX_OPERATIONS);
    clock_t end = clock();
    printCPUTime("Non-NUMA Execution", start, end);
    printf("Result: %d\n\n", result_non_numa);

    printf("Running benchmark with explicit NUMA-awareness:\n");
    numa_set_strict(1);
    numa_set_preferred(0);
    setCPUAffinity(0, 4);

    start = clock();
    int result_numa = calPoints(largeOps, MAX_OPERATIONS);
    end = clock();
    printCPUTime("NUMA-Aware Execution (Node 0 preferred, CPUs 0-4)", start, end);
    printf("Result: %d\n", result_numa);

    if (result_numa != result_non_numa) {
        fprintf(stderr, "Mismatch between NUMA and non-NUMA results!\n");
        return EXIT_FAILURE;
    }

    printMemoryUsage("After Execution");

    struct bitmask *nodes = numa_get_mems_allowed();
    printf("Memory allowed on nodes: ");
    for (unsigned i = 0; i < numa_num_configured_nodes(); i++) {
        if (numa_bitmask_isbitset(nodes, i))
            printf("%u ", i);
    }
    printf("\n");

    numa_free_nodemask(nodes);
    return EXIT_SUCCESS;
}

