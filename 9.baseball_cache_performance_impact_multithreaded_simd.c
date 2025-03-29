// 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <immintrin.h>
#include <sys/time.h>
#include <sys/resource.h>

#define MAX_OPERATIONS 1000000
#define NUM_THREADS 2

typedef struct {
    int *records;
    int start, end;
    int result;
} ThreadData;

// SIMD-optimized sum function using AVX with cache optimization
void *simdSum(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    __m256i sumVec = _mm256_setzero_si256();
    int i;

    for (i = data->start; i + 7 < data->end; i += 8) {
        __m256i values = _mm256_load_si256((__m256i*)&data->records[i]); // Aligned load
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

