/*
 *
 * Problem Statement:
 * ------------------
   The provided C program efficiently computes the total score for a baseball game simulation. 
   It interprets an array of string operations with the following rules:

   Numeric String ("10", "-5", etc.):** Adds the number as a score.

   "+": Adds the sum of the previous two scores.

   "D": Adds double the previous score.

   "C": Removes the previous score.

   Given potentially 1,000,000 operations, the program calculates the total efficiently using 
   parallelism (multi-threading) and vectorized SIMD instructions (AVX).

   Input Explained:
   ----------------
   The program generates a large test case with 1,000,000 operations:

   Operations generated: "10", "D", "10", "D", ... repeated 500,000 times.

   Expected Output (Benchmark Result):
   -----------------------------------
   Step-by-step Calculation:

   Operations: "10", "D" repeated 500,000 times.

   Scores generated: 10, 20, 10, 20, ..., repeating.

   Sum per pair: (10+20) = 30.

   Total sum: 500,000 pairs × 30 points each = 15,000,000.

   Multi-threaded SIMD: [time_ms] ms, Result: 15000000

   Multi-threaded SIMD: 3 ms, Result: 15000000

   [time_ms] will reflect execution time (fast due to SIMD parallelism).


   Performance Considerations:
   ---------------------------
   Multi-threading reduces overall execution time by splitting work across CPU cores.

   AVX SIMD provides vectorized computation, summing multiple integers simultaneously in 
   fewer CPU cycles.

   Combining both optimizations yields superior performance for large-scale datasets.

   This program exemplifies how to effectively combine SIMD vectorization with multi-threading in C, 
   dramatically accelerating large-scale numeric computations for real-world scenarios like this 
   baseball scoring system.

   My CPU supports:

   gcc -mavx2 -pthread -O3 5.baseball_parallel_optimization_simd_avx_sse.c -o baseball_simd
  
*/

#include <stdio.h>      // printf()
#include <stdlib.h>     // atoi()
#include <string.h>     // strcmp()
#include <ctype.h>      // isdigit() 
#include <pthread.h>    // pthreads for threading
#include <immintrin.h>  // AVX/SIMD instructions
#include <time.h>       // timing performance

#define MAX_OPERATIONS 1000000  // 1 million operations supported
#define NUM_THREADS 2           // 2 threads for parallel computation

typedef struct {
    int *records;      // Pointer to scores array
    int start, end;    // Indices for partial sum calculation
    int result;        // Partial sum result for each thread
} ThreadData;

// SIMD-Optimized Thread Function (simdSum)
// SIMD-optimized sum using AVX
// Purpose: Efficiently computes partial sums using AVX SIMD instructions, 
// significantly speeding up summation.

void *simdSum(void *arg) {
    ThreadData *data = (ThreadData *)arg; // Cast argument
    __m256i sumVec = _mm256_setzero_si256(); // Initialize AVX vector to zero
    int i;

    // SIMD Vectorized sum (AVX, 8 integers per loop)
    for (i = data->start; i + 7 < data->end; i += 8) {
        __m256i values = _mm256_loadu_si256((__m256i*)&data->records[i]); // Load 8 integers
        sumVec = _mm256_add_epi32(sumVec, values);  // Add them vectorially
    }

    // Storing intermediate vector sums into array
    int sumArray[8];
    _mm256_storeu_si256((__m256i*)sumArray, sumVec);

    // Reducing vector sum into a single integer sum
    int sum = sumArray[0] + sumArray[1] + sumArray[2] + sumArray[3] +
              sumArray[4] + sumArray[5] + sumArray[6] + sumArray[7];

    // Handling remaining elements individually
    for (; i < data->end; i++) {
        sum += data->records[i];
    }

    data->result = sum;   // Store partial sum result
    pthread_exit(NULL);   // Thread exits
}


// Main Score Calculation Function (calPoints):
// Function to compute baseball score using SIMD + Multi-threading

int calPoints(char *ops[], int size, int useMultithreading) {
    static int records[MAX_OPERATIONS];  // Large static array for scores
    int index = 0;  // Tracks current position in records
    
    // Processing Input Operations:
    for (int i = 0; i < size; i++) {
        if (isdigit(ops[i][0]) || (ops[i][0] == '-' && isdigit(ops[i][1]))) {
            records[index++] = atoi(ops[i]);  // Numeric score
        } else if (strcmp(ops[i], "C") == 0 && index > 0) {
            index--;  // Cancel last score
        } else if (strcmp(ops[i], "D") == 0 && index > 0) {
            records[index] = 2 * records[index - 1]; // Double previous score
            index++;
        } else if (strcmp(ops[i], "+") == 0 && index > 1) {
            records[index] = records[index - 1] + records[index - 2]; // Sum previous two
            index++;
        }
    }
    
    //  Conditional SIMD and Multi-threading:
    
    //  Small cases (<500) computed by single-threaded SIMD:
    if (!useMultithreading || index < 500) {
        ThreadData data = {records, 0, index, 0};
        simdSum(&data);
        return data.result;
    }

    // Large cases computed via Multi-threaded SIMD:
    pthread_t threads[NUM_THREADS];  // Threads array
    ThreadData data[NUM_THREADS];    // Thread data
    int mid = index / NUM_THREADS;   // Split array

    for (int i = 0; i < NUM_THREADS; i++) {
        data[i].records = records;
        data[i].start = i * mid;
        data[i].end = (i == NUM_THREADS - 1) ? index : (i + 1) * mid;
        pthread_create(&threads[i], NULL, simdSum, &data[i]);
    }

    int totalSum = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);   // Wait for threads
        totalSum += data[i].result;       // Aggregate partial sums
    }

    return totalSum;  // Final sum result
}


//  Function (Benchmarking SIMD+Multi-threading):
//  Measures performance of optimized SIMD multi-threaded summation.
int main() {
    static char *largeOps[MAX_OPERATIONS];

    // Generating large input: "10", "D", "10", "D",...
    for (int i = 0; i < 500000; i++) {
        largeOps[i * 2] = "10";
        largeOps[i * 2 + 1] = "D";
    }

    clock_t start = clock();
    int result = calPoints(largeOps, MAX_OPERATIONS, 1); // SIMD + Multi-threaded
    clock_t end = clock();

    // Measures performance of optimized SIMD multi-threaded summation.
    printf("Multi-threaded SIMD: %ld ms, Result: %d\n", (end - start) * 1000 / CLOCKS_PER_SEC, result);
    return 0;
}



