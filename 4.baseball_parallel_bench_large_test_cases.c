/* Problem Statement:
   The provided C program implements a simulation of baseball scoring rules, efficiently handling
   a large number of operations. Each operation is a string, interpreted as follows:

   Integer ("x"): Record a new score of x points.

   "+": Record a new score equal to the sum of the previous two scores.

   "D": Record a new score equal to double the previous score.

   "C": Remove the previously recorded score.

   Given a potentially large set of such operations (up to 1,000,000), the program computes 
   the total score efficiently using parallel processing (multi-threading).

   Input Explanation:
   ------------------
   The program generates a large number of operations to benchmark performance:

   generate 1,000,000 operations ("10", "D", "10", "D", ..., repeating 500,000 times).

   Expected Outputs (Benchmark Results):
   -------------------------------------
   
   Operations: "10", "D" repeated 500,000 times.

   Final Scores: Alternating between 10 and double previous repeatedly:
   → 10, 20, 10, 20, ..., repeated 500,000 times.

   Total Score Calculation per Pair (10 + 20 = 30):
   → 500,000 pairs × 30 points each = 15,000,000 points.

   Single-threaded: [time_ms] ms, Result: 15000000
   Multi-threaded: [faster_time_ms] ms, Result: 15000000

   Single-threaded: 7 ms, Result: 15000000
   Multi-threaded: 6 ms, Result: 15000000   

   The multi-threaded execution should be faster, leveraging CPU parallelism.

   Performance Advantage:
   ---------------------
   By dividing the summation into parallel tasks, the multi-threaded version significantly 
   reduces computation time, especially with large data sets.

   This program effectively illustrates parallel computation using pthreads to efficiently 
   handle large-scale data processing, benchmarking clearly the performance advantages 
   of parallel over single-threaded computations in C.

*/   

#include <stdio.h>     // Standard I/O for printf()
#include <stdlib.h>    // atoi()
#include <string.h>    // strcmp()
#include <pthread.h>   // pthreads for parallelism
#include <time.h>      // clock() for benchmarking

#define MAX_OPERATIONS 1000000 // Supports up to 1 million operations
#define NUM_THREADS 2          // Number of threads for parallel computation

typedef struct {
    int *records;   // Pointer to the score array
    int start, end; // Start and end indices for partial sum calculation
    int result;     // Partial sum result computed by each thread
} ThreadData;

// Function for thread to compute partial sum
// Each thread computes a partial sum of an assigned portion of the scores array.

void *parallelSum(void *arg) {
    ThreadData *data = (ThreadData *)arg; // Thread-specific data
    int sum = 0;

    // Compute partial sum
    for (int i = data->start; i < data->end; i++) {
        sum += data->records[i];
    }

    data->result = sum;    // Store partial result
    pthread_exit(NULL);    // Exit thread safely
}

// Function to process operations and compute sum
// Main Processing Function (calPoints):
int calPoints(char *ops[], int size, int useMultithreading) {
    static int records[MAX_OPERATIONS];  // Static large array to avoid stack overflow
    int index = 0;                       // Tracks current position in records 

    // Processing Operations:Processes each operation and populates the scores array (records)
	
    for (int i = 0; i < size; i++) {
        if (ops[i][0] >= '0' && ops[i][0] <= '9') {
            records[index++] = atoi(ops[i]);  // Numeric operation
        } else if (strcmp(ops[i], "C") == 0 && index > 0) {
            index--;                          // Remove previous score
        } else if (strcmp(ops[i], "D") == 0 && index > 0) {
            records[index] = 2 * records[index - 1]; // Double last score
            index++;
        } else if (strcmp(ops[i], "+") == 0 && index > 1) {
            records[index] = records[index - 1] + records[index - 2]; // Sum last two
            index++;
        }
    }

    // Summation:
    // The sum of recorded scores can be computed in two ways:
    // - Single-threaded for fewer scores (less than 500 scores).
    // - Multi-threaded when many scores are involved (≥ 500 scores).
   
    // Single-threaded: 
    if (!useMultithreading || index < 500) {
        int sum = 0;
        for (int i = 0; i < index; i++) sum += records[i];
        return sum;
    }

    // Multi-threaded sum calculation
    // Multi-threaded Parallel Summation
    //
    pthread_t threads[NUM_THREADS];     // Array of threads
    ThreadData data[NUM_THREADS];       // Data for each thread
    int mid = index / NUM_THREADS;      // Divide array among threads

    for (int i = 0; i < NUM_THREADS; i++) {
        data[i].records = records;      // Assign shared array
        data[i].start = i * mid;        // Start index
        data[i].end = (i == NUM_THREADS - 1) ? index : (i + 1) * mid; // End index
        pthread_create(&threads[i], NULL, parallelSum, &data[i]); // Start thread
    }

    int totalSum = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL); // Wait for thread completion
        totalSum += data[i].result;     // Add partial sums
    }

    return totalSum; // Return total computed sum
}

// Measures execution time for both single-threaded and multi-threaded computations for comparison.
int main() {
    static char *largeOps[MAX_OPERATIONS];

    // Generating 1,000,000 operations
    //  // Generate large input: ["10", "D", "10", "D", ...]
    for (int i = 0; i < 500000; i++) {
        largeOps[i * 2] = "10";
        largeOps[i * 2 + 1] = "D";
    }

    // Single-threaded execution
    //
    clock_t start = clock();
    int result1 = calPoints(largeOps, MAX_OPERATIONS, 0);
    clock_t end = clock();
    printf("Single-threaded: %ld ms, Result: %d\n", (end - start) * 1000 / CLOCKS_PER_SEC, result1);

    // Multi-threaded execution
    start = clock();
    int result2 = calPoints(largeOps, MAX_OPERATIONS, 1);
    end = clock();
    printf("Multi-threaded: %ld ms, Result: %d\n", (end - start) * 1000 / CLOCKS_PER_SEC, result2);

    return 0;
}

