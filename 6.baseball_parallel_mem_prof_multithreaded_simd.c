#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <immintrin.h>
#include <sys/resource.h>
#include <time.h>
#include <ctype.h>

#define MAX_OPERATIONS 1000000
#define NUM_THREADS 2

typedef struct {
    int *records;
    int start, end;
    int result;
} ThreadData;

// Function to print memory usage
void printMemoryUsage(const char *label) {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("%s - Memory Usage: %ld KB\n", label, usage.ru_maxrss);
}

// SIMD-optimized sum using AVX
void *simdSum(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    __m256i sumVec = _mm256_setzero_si256();
    int i;

    for (i = data->start; i + 7 < data->end; i += 8) {
        __m256i values = _mm256_loadu_si256((__m256i*)&data->records[i]);
        sumVec = _mm256_add_epi32(sumVec, values);
    }

    int sumArray[8];
    _mm256_storeu_si256((__m256i*)sumArray, sumVec);
    int sum = sumArray[0] + sumArray[1] + sumArray[2] + sumArray[3] +
              sumArray[4] + sumArray[5] + sumArray[6] + sumArray[7];

    for (; i < data->end; i++) {
        sum += data->records[i];
    }

    data->result = sum;
    pthread_exit(NULL);
}

// Function to compute baseball score using SIMD + Multi-threading
int calPoints(char *ops[], int size, int useMultithreading) {
    static int records[MAX_OPERATIONS];
    int index = 0;

    for (int i = 0; i < size; i++) {
        if (isdigit(ops[i][0]) || (ops[i][0] == '-' && isdigit(ops[i][1]))) {
            records[index++] = atoi(ops[i]);
        } else if (strcmp(ops[i], "C") == 0 && index > 0) {
            index--;
        } else if (strcmp(ops[i], "D") == 0 && index > 0) {
            records[index] = 2 * records[index - 1];
            index++;
        } else if (strcmp(ops[i], "+") == 0 && index > 1) {
            records[index] = records[index - 1] + records[index - 2];
            index++;
        }
    }

    printMemoryUsage("After Record Creation");

    if (!useMultithreading || index < 500) {
        ThreadData data = {records, 0, index, 0};
        simdSum(&data);
        return data.result;
    }

    pthread_t threads[NUM_THREADS];
    ThreadData data[NUM_THREADS];
    int mid = index / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        data[i].records = records;
        data[i].start = i * mid;
        data[i].end = (i == NUM_THREADS - 1) ? index : (i + 1) * mid;
        pthread_create(&threads[i], NULL, simdSum, &data[i]);
    }

    int totalSum = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        totalSum += data[i].result;
    }

    return totalSum;
}

int main() {
    static char *largeOps[MAX_OPERATIONS];

    for (int i = 0; i < 500000; i++) {
        largeOps[i * 2] = "10";
        largeOps[i * 2 + 1] = "D";
    }

    printMemoryUsage("Before Execution");

    clock_t start = clock();
    int result = calPoints(largeOps, MAX_OPERATIONS, 1);
    clock_t end = clock();

    printMemoryUsage("After Execution");
    printf("Multi-threaded SIMD: %ld ms, Result: %d\n", (end - start) * 1000 / CLOCKS_PER_SEC, result);
    return 0;
}

